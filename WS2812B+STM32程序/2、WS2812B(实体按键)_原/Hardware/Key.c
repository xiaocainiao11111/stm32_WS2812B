#include "stm32f10x.h"
#include "IO.h"

uint8_t Key_KeyNumber;

void Key_Init(void)
{
	IO_InInit(GPIOA, GPIO_Pin_1);
	IO_InInit(GPIOA, GPIO_Pin_5);
	IO_InInit(GPIOB, GPIO_Pin_1);
}

uint8_t Key_GetKeyNumber(void)
{
	uint8_t Temp;
	Temp=Key_KeyNumber;
	Key_KeyNumber=0;
	return Temp;
}

void Key_Loop(void)
{
	static uint8_t KeyLast,KeyNow;
	KeyLast=KeyNow;
	KeyNow=0;
	if(IO_InGet(GPIOA, GPIO_Pin_1)==0)
	{
		KeyNow=1;
	}
	if(IO_InGet(GPIOA, GPIO_Pin_5)==0)
	{
		KeyNow=2;
	}
	if(IO_InGet(GPIOB, GPIO_Pin_1)==0)
	{
		KeyNow=3;
	}
	if(KeyLast==1 && KeyNow==0)
	{
		Key_KeyNumber=1;
	}
	if(KeyLast==2 && KeyNow==0)
	{
		Key_KeyNumber=2;
	}
	if(KeyLast==3 && KeyNow==0)
	{
		Key_KeyNumber=3;
	}
}
