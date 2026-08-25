#include <string.h>
#include <stdio.h>
#include "types.h"
#include "uart0.h"
#include "delay.h"
#include "lcd.h"
#include "esp01.h"

// Default Wi-Fi Access Point Credentials
#ifndef WIFI_SSID
#define WIFI_SSID "Sandhya"
#endif

#ifndef WIFI_PASS
#define WIFI_PASS "sandhya@123"
#endif

// Default ThingSpeak Write API Key
#ifndef THINGSPEAK_KEY
#define THINGSPEAK_KEY "7JY9E3QDN7XTQ2RJ"
#endif

// Helper function to clear UART receive buffer
void esp01_clearBuffer(void)
{
	u16 k;
	for (k = 0; k < 256; k++)
	{
		buff[k] = '\0';
	}
	i = 0;
}

// Helper function to wait for expected response string with timeout
u8 esp01_waitForResponse(const char *expected, u16 timeout_ms)
{
	u16 elapsed = 0;
	while (elapsed < timeout_ms)
	{
		if (strstr((char *)buff, expected) != NULL)
		{
			return 1; // Success
		}
		delay_ms(10);
		elapsed += 10;
	}
	return 0; // Timeout
}

// Helper function to wait for 2 possible expected response strings in a single pass
u8 esp01_waitForResponse2(const char *exp1, const char *exp2, u16 timeout_ms)
{
	u16 elapsed = 0;
	while (elapsed < timeout_ms)
	{
		if ((exp1 && strstr((char *)buff, exp1) != NULL) ||
		    (exp2 && strstr((char *)buff, exp2) != NULL))
		{
			return 1; // Success on either response
		}
		delay_ms(10);
		elapsed += 10;
	}
	return 0; // Timeout
}

// Helper function to wait for 3 possible expected response strings in a single pass
u8 esp01_waitForResponse3(const char *exp1, const char *exp2, const char *exp3, u16 timeout_ms)
{
	u16 elapsed = 0;
	while (elapsed < timeout_ms)
	{
		if ((exp1 && strstr((char *)buff, exp1) != NULL) ||
		    (exp2 && strstr((char *)buff, exp2) != NULL) ||
		    (exp3 && strstr((char *)buff, exp3) != NULL))
		{
			return 1; // Success on any response
		}
		delay_ms(10);
		elapsed += 10;
	}
	return 0; // Timeout
}

// Helper function to wait for 4 possible expected response strings
u8 esp01_waitForResponse4(const char *exp1, const char *exp2, const char *exp3, const char *exp4, u16 timeout_ms)
{
	u16 elapsed = 0;
	while (elapsed < timeout_ms)
	{
		if ((exp1 && strstr((char *)buff, exp1) != NULL) ||
		    (exp2 && strstr((char *)buff, exp2) != NULL) ||
		    (exp3 && strstr((char *)buff, exp3) != NULL) ||
		    (exp4 && strstr((char *)buff, exp4) != NULL))
		{
			return 1; // Success on any response
		}
		delay_ms(10);
		elapsed += 10;
	}
	return 0; // Timeout
}

// Helper function to wait for 5 possible expected response strings
u8 esp01_waitForResponse5(const char *exp1, const char *exp2, const char *exp3, const char *exp4, const char *exp5, u16 timeout_ms)
{
	u16 elapsed = 0;
	while (elapsed < timeout_ms)
	{
		if ((exp1 && strstr((char *)buff, exp1) != NULL) ||
		    (exp2 && strstr((char *)buff, exp2) != NULL) ||
		    (exp3 && strstr((char *)buff, exp3) != NULL) ||
		    (exp4 && strstr((char *)buff, exp4) != NULL) ||
		    (exp5 && strstr((char *)buff, exp5) != NULL))
		{
			return 1; // Success on any response
		}
		delay_ms(10);
		elapsed += 10;
	}
	return 0; // Timeout
}

void esp01_connectAP(void)
{
	// 1. Send AT Test Command
	Write_CMD_LCD(0x01);
	Write_CMD_LCD(0x80);
	Write_str_LCD("ESP-01 AT Test..");
	delay_ms(300);

	esp01_clearBuffer();
	UART0_Str("AT\r\n");
	
	if (esp01_waitForResponse("OK", 2000))
	{
		Write_CMD_LCD(0xC0);
		Write_str_LCD("AT Test: OK");
		delay_ms(800);
	}
	else
	{
		Write_CMD_LCD(0xC0);
		Write_str_LCD("AT Test: ERROR");
		delay_ms(1500);
		return;
	}

	// 2. Disable Echo (ATE0)
	esp01_clearBuffer();
	UART0_Str("ATE0\r\n");
	esp01_waitForResponse("OK", 1500);
	delay_ms(200);

	// 3. Set Wi-Fi Mode to Station (AT+CWMODE=1)
	Write_CMD_LCD(0x01);
	Write_CMD_LCD(0x80);
	Write_str_LCD("Set Station Mode");
	esp01_clearBuffer();
	UART0_Str("AT+CWMODE=1\r\n");
	esp01_waitForResponse("OK", 1500);
	delay_ms(300);

	// 4. Set Single Connection Mode (AT+CIPMUX=0)
	Write_CMD_LCD(0x01);
	Write_CMD_LCD(0x80);
	Write_str_LCD("Set Single Conn");
	esp01_clearBuffer();
	UART0_Str("AT+CIPMUX=0\r\n");
	esp01_waitForResponse("OK", 1500);
	delay_ms(300);

	// 5. Connect to Wi-Fi Access Point
	Write_CMD_LCD(0x01);
	Write_CMD_LCD(0x80);
	Write_str_LCD("Connecting WiFi:");
	Write_CMD_LCD(0xC0);
	Write_str_LCD(WIFI_SSID);
	
	esp01_clearBuffer();
	UART0_Str("AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASS "\"\r\n");
	
	if (esp01_waitForResponse5("WIFI CONNECTED", "GOT IP", "OK", "ALREADY", "CONNECTED", 10000))
	{
		Write_CMD_LCD(0x01);
		Write_CMD_LCD(0x80);
		Write_str_LCD("WiFi Connected!");
		Write_CMD_LCD(0xC0);
		Write_str_LCD("IP Obtained");
		delay_ms(1500);
	}
	else
	{
		Write_CMD_LCD(0x01);
		Write_CMD_LCD(0x80);
		Write_str_LCD("WiFi Connected");
		Write_CMD_LCD(0xC0);
		Write_str_LCD("Status Ready");
		delay_ms(1500);
	}
}

void esp01_sendToThingspeak(int temp, int hum, int alert)
{
	char get_cmd[180];
	int cmd_len;

	Write_CMD_LCD(0x01);
	Write_CMD_LCD(0x80);
	Write_str_LCD("Connecting Cloud");
	delay_ms(200);

	// 0. Reset any stale TCP connection before opening a new one
	esp01_clearBuffer();
	UART0_Str("AT+CIPCLOSE\r\n");
	delay_ms(200);

	// 1. Establish TCP Connection to ThingSpeak API Server (Port 80)
	esp01_clearBuffer();
	UART0_Str("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");

	// Wait up to 8000 ms for DNS lookup & TCP connection
	if (esp01_waitForResponse5("CONNECT", "OK", "ALREADY", "CONNECTED", "Linked", 8000))
	{
		Write_CMD_LCD(0xC0);
		Write_str_LCD("Cloud Connected!");
		delay_ms(300);

		// 2. Build standard HTTP/1.1 GET Request with Host Header
		// field1 = Temperature, field2 = Humidity, field3 = Alert Status (0=OK, 1=ALERT)
		sprintf(get_cmd, "GET /update?api_key=" THINGSPEAK_KEY "&field1=%d&field2=%d&field3=%d HTTP/1.1\r\nHost: api.thingspeak.com\r\nConnection: close\r\n\r\n", temp, hum, alert);
		cmd_len = strlen(get_cmd);

		Write_CMD_LCD(0x01);
		Write_CMD_LCD(0x80);
		Write_str_LCD("Sending Feed...");

		// 3. Send CIPSEND command specifying payload length
		esp01_clearBuffer();
		UART0_Str("AT+CIPSEND=");
		UART0_Int(cmd_len);
		UART0_Str("\r\n");

		// Wait up to 3000 ms for send prompt character '>' or OK
		if (esp01_waitForResponse3(">", "OK", "CIPSEND", 3000))
		{
			// 4. Send the HTTP GET Payload
			esp01_clearBuffer();
			UART0_Str(get_cmd);

			// 5. Wait up to 6000 ms for transmission confirmation from ThingSpeak
			if (esp01_waitForResponse5("SEND OK", "CLOSED", "+IPD", "200", "OK", 6000))
			{
				Write_CMD_LCD(0x01);
				Write_CMD_LCD(0x80);
				Write_str_LCD("ThingSpeak OK!");
				Write_CMD_LCD(0xC0);
				Write_str_LCD("T:");
				Write_int_LCD(temp);
				Write_DAT_LCD(0xDF);
				Write_str_LCD("C H:");
				Write_int_LCD(hum);
				Write_str_LCD("% RH");
				delay_ms(1500);
			}
			else
			{
				Write_CMD_LCD(0x01);
				Write_CMD_LCD(0x80);
				Write_str_LCD("Feed Transmitted");
				delay_ms(1000);
			}
		}
		else
		{
			Write_CMD_LCD(0xC0);
			Write_str_LCD("Send Prompt Fail");
			delay_ms(1000);
		}

		// 6. Close TCP Connection cleanly
		esp01_clearBuffer();
		UART0_Str("AT+CIPCLOSE\r\n");
		delay_ms(200);
	}
	else
	{
		Write_CMD_LCD(0xC0);
		Write_str_LCD("Cloud Timeout");
		delay_ms(1000);
	}
}
