#ifndef _ESP01_H_
#define _ESP01_H_

#include "types.h"

void esp01_clearBuffer(void);
u8 esp01_waitForResponse(const char *expected, u16 timeout_ms);
u8 esp01_waitForResponse2(const char *exp1, const char *exp2, u16 timeout_ms);
u8 esp01_waitForResponse3(const char *exp1, const char *exp2, const char *exp3, u16 timeout_ms);
u8 esp01_waitForResponse4(const char *exp1, const char *exp2, const char *exp3, const char *exp4, u16 timeout_ms);
u8 esp01_waitForResponse5(const char *exp1, const char *exp2, const char *exp3, const char *exp4, const char *exp5, u16 timeout_ms);
void esp01_connectAP(void);
void esp01_sendToThingspeak(int temp, int hum, int alert);

#endif
