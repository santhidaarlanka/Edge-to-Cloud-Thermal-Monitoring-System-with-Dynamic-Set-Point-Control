#ifndef __LCD_H__
#define __LCD_H__

void LCD_Init(void);
void Write_CMD_LCD(char cmd);
void Write_DAT_LCD(char dat);
void Write_LCD(char ch);
void Write_str_LCD(char *p);
void Write_int_LCD(signed int n);
void Write_float_LCD(float f, char i);

#endif
