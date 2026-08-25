
#include<LPC21xx.h>
#include"types.h"
#include "defines.h"
#include"key_defines.h"
#include"lcd_defines.h"
#include "lcd.h"
/*																					 
u32 kpmUT[4][4]=
{
    {1,2,3,4},
		{5,6,7,8},
		{9,10,11,12},
		{13,14,15,16}

};*/

u8 kpmLUT[4][4]=
{
     {'1','2','3','A'},
		 {'4','5','6','B'},
		 {'7','8','9','C'},
		 {'*','0','#','D'}
}; 

void Init_KPM(void)
{
      IODIR1|=(15<<ROWs_4);
}
u32 ColScan(void)
{
      return (READNIBBLE(IOPIN1,cOL_0)<15)?0:1;
}
u32 RowCheck(void)
{
      u32 r;
			for(r=0;r<=3;r++)
			{
			     IOPIN1=((IOPIN1&~(15<<ROWs_4))|((((~(1<<r))&0x0F)<<ROWs_4)));
					 if(ColScan()==0)
					 {
					      break;
					 }
			}
	   IOCLR1|=(15<<ROWs_4);

       return r;
}
u32 ColCheck(void)
{
       u32 c;
			 for(c=0;c<=3;c++)
			 {
			     if(((IOPIN1>>(cOL_0+c))&1)==0)
					 {
					       break;
					 }
			     
			}
		return c;
}
u8 KeyScan(void)
{
       u32 r,c;
			 u8 keyV;
			 while(ColScan());
			 r=RowCheck();
			 c=ColCheck();
			 keyV=kpmLUT[r][c];
			 while(ColScan()==0);
			 return keyV;
}
void ReadNum(u32 *num, u8 *key)
{
	*num = 0;
	while(1)
	{
		*key = KeyScan();
		if (*key >= '0' && *key <= '9')
		{
			*num = (*num * 10) + (*key - '0');
			Write_CMD_LCD(GOTO_LINE2_POS0);
			Write_int_LCD(*num);
			while(ColScan() == 0);
		}
		else if (*key == '*')
		{
			*num /= 10;
			Write_CMD_LCD(GOTO_LINE2_POS0);
			Write_str_LCD("    ");
			Write_CMD_LCD(GOTO_LINE2_POS0);
			if (*num > 0)
			{
				Write_int_LCD(*num);
			}
			while(ColScan() == 0);
		}
		else if (*key == '#')
		{
			while(ColScan() == 0);
			break;	 
		}
	}
}

