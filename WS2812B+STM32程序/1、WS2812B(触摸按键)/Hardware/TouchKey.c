#include "stm32f10x.h"
#include "ADC1.h"

uint8_t TouchKey_KeyNumber;

void TouchKey_Init(void)
{
	ADC1_Init();
}

uint8_t TouchKey_GetKeyNumber(void)
{
	uint8_t Temp;
	Temp=TouchKey_KeyNumber;
	TouchKey_KeyNumber=0;
	return Temp;
}

void TouchKey_Loop(void)
{
	uint8_t i;
	static uint16_t Max1[2];
	static uint16_t ValueBuf1[30];
	static uint8_t pValueBuf1;
	static uint16_t Max2[2];
	static uint16_t ValueBuf2[30];
	static uint8_t pValueBuf2;
	static uint16_t Max3[2];
	static uint16_t ValueBuf3[30];
	static uint8_t pValueBuf3;
	
	Max1[1]=Max1[0];
	Max1[0]=0;
	ValueBuf1[pValueBuf1]=ADC1_GetValue(ADC_Channel_5);
	pValueBuf1++;
	pValueBuf1%=30;
	for(i=0;i<30;i++)
	{
		if(ValueBuf1[i]>Max1[0])
		{
			Max1[0]=ValueBuf1[i];
		}
	}
	if(Max1[1]<4000 && Max1[0]>4000)
	{
		TouchKey_KeyNumber=1;
	}
	
	Max2[1]=Max2[0];
	Max2[0]=0;
	ValueBuf2[pValueBuf2]=ADC1_GetValue(ADC_Channel_6);
	pValueBuf2++;
	pValueBuf2%=30;
	for(i=0;i<30;i++)
	{
		if(ValueBuf2[i]>Max2[0])
		{
			Max2[0]=ValueBuf2[i];
		}
	}
	if(Max2[1]<4000 && Max2[0]>4000)
	{
		TouchKey_KeyNumber=2;
	}
	
	Max3[1]=Max3[0];
	Max3[0]=0;
	ValueBuf3[pValueBuf3]=ADC1_GetValue(ADC_Channel_7);
	pValueBuf3++;
	pValueBuf3%=30;
	for(i=0;i<30;i++)
	{
		if(ValueBuf3[i]>Max3[0])
		{
			Max3[0]=ValueBuf3[i];
		}
	}
	if(Max3[1]<4000 && Max3[0]>4000)
	{
		TouchKey_KeyNumber=3;
	}
}
