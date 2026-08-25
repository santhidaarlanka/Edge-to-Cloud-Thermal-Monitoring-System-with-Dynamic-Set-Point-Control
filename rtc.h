#ifndef _RTC_H_
#define _RTC_H_

#include <lpc21xx.h>
#include "types.h"

void RTC_Init(void);
u32 RTC_GetTotalSeconds(void);

#endif
