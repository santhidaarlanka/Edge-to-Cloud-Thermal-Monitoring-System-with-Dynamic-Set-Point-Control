#include <lpc21xx.h>
#include <string.h>

#include "types.h"
#include "defines.h"
#include "pin_function_defines.h"
#include "lcd_defines.h"
#include "lcd.h"
#include "uart0.h"
#include "i2c.h"
#include "i2c_eeprom.h"
#include "dht11.h"
#include "kpm.h"
#include "esp01.h"
#include "external_interrupts_test.h"
#include "rtc.h"
#include "delay.h"
#include "global.h"

#define I2C_EEPROM_SA 0x50
#define BUZZER_PIN    7    // Buzzer connected to P0.7

// Global variable definitions
u32 setpoint = 35;           // Default temperature set-point (35C)
u32 humidity_setpoint = 70;  // Default humidity set-point (70%)
u8 alert_flag = 0;           // Alert status flag

void Buzzer_Init(void)
{
	WRITEBIT(IODIR0, BUZZER_PIN, 1); // Set P0.7 as output
	WRITEBIT(IOCLR0, BUZZER_PIN, 1); // Turn OFF Buzzer
}

void Buzzer_Beep(u8 count)
{
	u8 i;
	for(i = 0; i < count; i++)
	{
		WRITEBIT(IOSET0, BUZZER_PIN, 1); // Buzzer ON
		delay_ms(150);
		WRITEBIT(IOCLR0, BUZZER_PIN, 1); // Buzzer OFF
		delay_ms(150);
	}
}

int main(void)
{
	u8 hum_int = 0, hum_dec = 0, temp_int = 0, temp_dec = 0, checksum = 0;
	u8 last_valid_temp = 28, last_valid_hum = 55;
	u8 eeprom_sp = 0, eeprom_hum_sp = 0;
	u32 last_rtc_sec = 0, current_rtc_sec = 0, elapsed_rtc_sec = 0;

	// 1. Initialize Hardware Peripherals
	Buzzer_Init();
	LCD_Init();
	InitUART0();
	init_i2c();
	Init_KPM();
	RTC_Init(); // Initialize Hardware RTC peripheral

	// Display Startup Title
	Write_CMD_LCD(0x01);
	Write_CMD_LCD(0x80);
	Write_str_LCD("Edge-Cloud Thermo");
	Write_CMD_LCD(0xC0);
	Write_str_LCD("Monitoring Sys");
	delay_ms(2000);

	// 2. EEPROM Set-Point Initialization
	// Read saved temperature set-point from AT24C256 address 0x0000
	eeprom_sp = i2c_eeprom_read(I2C_EEPROM_SA, 0x0000);
	if (eeprom_sp > 0 && eeprom_sp < 100)
	{
		setpoint = eeprom_sp;
	}
	else
	{
		// Default temperature set-point if EEPROM is uninitialized
		setpoint = 35;
		i2c_eeprom_write(I2C_EEPROM_SA, 0x0000, (u8)setpoint);
	}

	// Read saved humidity set-point from AT24C256 address 0x0001
	eeprom_hum_sp = i2c_eeprom_read(I2C_EEPROM_SA, 0x0001);
	if (eeprom_hum_sp > 0 && eeprom_hum_sp <= 100)
	{
		humidity_setpoint = eeprom_hum_sp;
	}
	else
	{
		// Default humidity set-point if EEPROM is uninitialized
		humidity_setpoint = 70;
		i2c_eeprom_write(I2C_EEPROM_SA, 0x0001, (u8)humidity_setpoint);
	}

	Write_CMD_LCD(0x01);
	Write_CMD_LCD(0x80);
	Write_str_LCD("Temp SP: ");
	Write_int_LCD(setpoint);
	Write_DAT_LCD(0xDF);
	Write_str_LCD("C");

	Write_CMD_LCD(0xC0);
	Write_str_LCD("Hum  SP: ");
	Write_int_LCD(humidity_setpoint);
	Write_str_LCD("%");
	delay_ms(1500);

	// 3. Enable EINT0 External Interrupt for Dynamic Set-Point Configuration
	Enable_EINT0();

	// 4. Connect to Wi-Fi Access Point (Hotspot: Sandhya)
	esp01_connectAP();

	// Initial RTC timestamp & initial Cloud feed transmission
	last_rtc_sec = RTC_GetTotalSeconds();
	esp01_sendToThingspeak(28, 55, 0);

	// 5. Main Continuous Sensing & Monitoring Loop
	while(1)
	{
		// 5a. Check if EINT0 interrupt requested dynamic Set-Point change
		if (setpoint_trigger_flag)
		{
			SetPoint();
			setpoint_trigger_flag = 0;
		}

		// 5b. Request & Decode DHT11 Data with 8-Bit Checksum Validation
		if (dht11_read_all(&hum_int, &hum_dec, &temp_int, &temp_dec, &checksum))
		{
			// Checksum Valid: Update last valid sensor values
			last_valid_temp = temp_int;
			last_valid_hum  = hum_int;
		}
		else
		{
			// Checksum Mismatch or Sensor Timeout: Retain last valid readings
			temp_int = last_valid_temp;
			hum_int  = last_valid_hum;
		}

		// 6. Threshold Check & Alert Generation
		if (temp_int > setpoint || hum_int > humidity_setpoint)
		{
			alert_flag = 1;
			// Pulse Buzzer to alert nearby personnel
			Buzzer_Beep(2);
		}
		else
		{
			alert_flag = 0;
			WRITEBIT(IOCLR0, BUZZER_PIN, 1); // Ensure Buzzer is OFF
		}

		// 7. Display Live Telemetry on 16x2 LCD
		Write_CMD_LCD(0x01);

		// Line 1: Temperature and Temperature Setpoint (e.g. T:38°C  SP:35°C)
		Write_CMD_LCD(0x80);
		Write_str_LCD("T:");
		Write_int_LCD(temp_int);
		Write_DAT_LCD(0xDF); // Degree symbol °
		Write_str_LCD("C  SP:");
		Write_int_LCD(setpoint);
		Write_DAT_LCD(0xDF); // Degree symbol °
		Write_str_LCD("C");

		// Line 2: Humidity and Humidity Setpoint (e.g. H:55%   SP:70%)
		Write_CMD_LCD(0xC0);
		Write_str_LCD("H:");
		Write_int_LCD(hum_int);
		Write_str_LCD("%   SP:");
		Write_int_LCD(humidity_setpoint);
		Write_str_LCD("%");

		delay_ms(2000);

		// 8. Periodically Transmit Data Feed to ThingSpeak Cloud using RTC (Every 3 Minutes = 180 Sec)
		current_rtc_sec = RTC_GetTotalSeconds();
		if (current_rtc_sec >= last_rtc_sec)
		{
			elapsed_rtc_sec = current_rtc_sec - last_rtc_sec;
		}
		else
		{
			elapsed_rtc_sec = (current_rtc_sec + 3600) - last_rtc_sec; // Handle 60-min RTC rollover
		}

		if (elapsed_rtc_sec >= 180) // Strictly every 3 minutes (180 RTC seconds)
		{
			esp01_sendToThingspeak(temp_int, hum_int, alert_flag);
			last_rtc_sec = current_rtc_sec;
		}
	}
}


