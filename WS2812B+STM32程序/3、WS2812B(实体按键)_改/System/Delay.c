#include "stm32f10x.h"
#define AHB_INPUT  72  //AHB时钟频率（单位MHz）

void Delay_us(uint32_t us)//ns微秒级延时程序（参考值即是延时数，72MHz时最大值233015）
{
	SysTick->LOAD=AHB_INPUT*us;//重装计数初值（当主频是72MHz，72次为1微秒）
	SysTick->VAL=0x00;        //清空定时器的计数器
	SysTick->CTRL=0x00000005;//时钟源HCLK，打开定时器
	while(!(SysTick->CTRL&0x00010000)); //等待计数到0
	SysTick->CTRL=0x00000004;//关闭定时器
}

void Delay_ms(uint16_t ms)//ms毫秒级延时程序（参考值即是延时数，最大值65535）
{
	while( ms-- != 0)
	{
		Delay_us(1000);//调用1000微秒的延时
	}
}
 
void Delay_s(uint16_t s)//s秒级延时程序（参考值即是延时数，最大值65535）
{
	while( s-- != 0)
	{
		Delay_ms(1000);//调用1000毫秒的延时
	}
} 
