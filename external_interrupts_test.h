#ifndef __EXTERNAL_INTERRUPTS_TEST_H__
#define __EXTERNAL_INTERRUPTS_TEST_H__

#include "types.h"

extern volatile u8 setpoint_trigger_flag;

void Enable_EINT0(void);
void SetPoint(void);

#endif
