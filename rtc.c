#include <lpc21xx.h>
#include "types.h"
#include "rtc.h"

void RTC_Init(void)
{
	CCR = 0x02;       // Reset RTC clock counters
	PREINT = 0x01C8;  // Prescaler Integer for PCLK = 15 MHz (60MHz / 4)
	PREFRAC = 0x3600; // Prescaler Fractional for PCLK = 15 MHz
	CCR = 0x01;       // Enable RTC peripheral
}

u32 RTC_GetTotalSeconds(void)
{
	return (u32)(MIN * 60 + SEC);
}

