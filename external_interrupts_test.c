#include <lpc21xx.h>
#include "pin_function_defines.h"
#include "defines.h"
#include "lcd.h"
#include "kpm.h"
#include "global.h"
#include "i2c.h"
#include "i2c_eeprom.h"
#include "delay.h"
#include "external_interrupts_test.h"

#define I2C_EEPROM_SA 0x50

volatile u8 setpoint_trigger_flag = 0;

void eint0_isr(void) __irq
{
	setpoint_trigger_flag = 1; // Flag main loop to run SetPoint input
	EXTINT = 1 << 0;            // Clear EINT0 interrupt flag by writing 1 to bit 0
	VICVectAddr = 0;           // Dummy write to signal end of interrupt execution
}	

void Enable_EINT0(void)
{
	// P0.16 configured as EINT0 (FUNC2)
	CFGPIN(PINSEL1, 16, FUNC2);
	
	// Enable EINT0 (Interrupt 14) in VIC
	SSETBIT(VICIntEnable, 14);
	VICVectCntl1 = 0x20 | 14;
	VICVectAddr1 = (unsigned)eint0_isr;
	
	// EINT0 edge-triggered
	SETBIT(EXTMODE, 0);
}	

void SetPoint(void)
{
	u32 temp_sp = 0;
	u32 hum_sp = 0;
	u8 key = 0;

	// --- 1. Temperature SetPoint Entry ---
	Write_CMD_LCD(0x01);
	Write_CMD_LCD(0x80);
	Write_str_LCD("Set Temp SP:");
	Write_CMD_LCD(0xC0);

	ReadNum(&temp_sp, &key);
	if (temp_sp > 0 && temp_sp < 100)
	{
		setpoint = temp_sp;
		// Write temperature setpoint to AT24C256 EEPROM address 0x0000
		i2c_eeprom_write(I2C_EEPROM_SA, 0x0000, (u8)setpoint);
		delay_ms(200);

		Write_CMD_LCD(0x01);
		Write_CMD_LCD(0x80);
		Write_str_LCD("Temp SP Saved:");
		Write_CMD_LCD(0xC0);
		Write_str_LCD("T-SP: ");
		Write_int_LCD(setpoint);
		Write_DAT_LCD(0xDF);
		Write_str_LCD("C");
		delay_ms(1000);
	}
	else
	{
		Write_CMD_LCD(0x01);
		Write_CMD_LCD(0x80);
		Write_str_LCD("Invalid Temp SP!");
		Write_CMD_LCD(0xC0);
		Write_str_LCD("Retaining: ");
		Write_int_LCD(setpoint);
		Write_DAT_LCD(0xDF);
		Write_str_LCD("C");
		delay_ms(1000);
	}

	// --- 2. Humidity SetPoint Entry ---
	Write_CMD_LCD(0x01);
	Write_CMD_LCD(0x80);
	Write_str_LCD("Set Hum SP (%):");
	Write_CMD_LCD(0xC0);

	ReadNum(&hum_sp, &key);
	if (hum_sp > 0 && hum_sp <= 100)
	{
		humidity_setpoint = hum_sp;
		// Write humidity setpoint to AT24C256 EEPROM address 0x0001
		i2c_eeprom_write(I2C_EEPROM_SA, 0x0001, (u8)humidity_setpoint);
		delay_ms(200);

		Write_CMD_LCD(0x01);
		Write_CMD_LCD(0x80);
		Write_str_LCD("Hum SP Saved:");
		Write_CMD_LCD(0xC0);
		Write_str_LCD("H-SP: ");
		Write_int_LCD(humidity_setpoint);
		Write_str_LCD("%");
		delay_ms(1000);
	}
	else
	{
		Write_CMD_LCD(0x01);
		Write_CMD_LCD(0x80);
		Write_str_LCD("Invalid Hum SP!");
		Write_CMD_LCD(0xC0);
		Write_str_LCD("Retaining: ");
		Write_int_LCD(humidity_setpoint);
		Write_str_LCD("%");
		delay_ms(1000);
	}

	// --- 3. Final Confirmation Screen ---
	Write_CMD_LCD(0x01);
	Write_CMD_LCD(0x80);
	Write_str_LCD("T-SP:");
	Write_int_LCD(setpoint);
	Write_DAT_LCD(0xDF);
	Write_str_LCD("C Saved");

	Write_CMD_LCD(0xC0);
	Write_str_LCD("H-SP:");
	Write_int_LCD(humidity_setpoint);
	Write_str_LCD("% Saved");
	delay_ms(1500);
}

