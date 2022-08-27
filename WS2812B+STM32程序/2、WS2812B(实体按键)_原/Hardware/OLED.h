#ifndef __OLED_H
#define __OLED_H
#include "stm32f10x.h"
#include "stdio.h"

/*DisplayType宏定义*/
#define		NORMAL		8
#define		SMALL		6
#define		REVERSE		0x10

/*NumType宏定义*/
#define		SIGNED		0x10
#define		HEX			0x20

/*外部可调用变量声明*/
extern char OLED_String[25];

/*应用层函数*/
void OLED_Init(void);
void OLED_AllClear(void);
void OLED_AreaClear(uint8_t StartX,uint8_t StartY,uint8_t EndX,uint8_t EndY);
void OLED_ShowChar(uint8_t X,uint8_t Y,int8_t Char,uint8_t DisplayType);
void OLED_ShowString(uint8_t X,uint8_t Y,char *String,uint8_t DisplayType);
void OLED_ShowNumber(uint8_t X,uint8_t Y,int32_t Number,uint8_t NumType,uint8_t DisplayType);
void OLED_ShowChinese(uint8_t X,uint8_t Y,uint8_t *Chinese,uint8_t DisplayType);
void OLED_ShowImage(uint8_t X,uint8_t Y,uint8_t *Image,uint8_t DisplayType);

#endif
