#ifndef __KPM_H__
#define __KPM_H__

#include "types.h"

void Init_KPM(void);
u32 ColScan(void);
u32 RowCheck(void);
u32 ColCheck(void);
u8 KeyScan(void);
void ReadNum(u32 *, u8 *);

#endif
