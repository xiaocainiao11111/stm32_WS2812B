#include "stm32f10x.h"
#include "TIM2.h"
#include "DMA1.h"

//#define WS2812B_LED_QUANTITY	32  //��������
#define WS2812B_LED_QUANTITY	100  //��������


uint32_t WS2812B_Buf[WS2812B_LED_QUANTITY];	//0xGGRRBB
uint16_t WS2812B_Bit[24*WS2812B_LED_QUANTITY+1];//��������*24bit+1
uint8_t WS2812B_Flag;

void WS2812B_IRQHandler(void);

void WS2812B_Init(void)
{
	DMA1_SetIRQHandler(WS2812B_IRQHandler);//��WS2812B_IRQHandler����DMA1_Handler
	DMA1_Init((uint32_t)(&WS2812B_Bit));//DMA
	TIM2_Init();//��ʱ��
}

void WS2812B_ClearBuf(void)//ȫ���������ȹ�0
{
	uint8_t i;
	for(i=0;i<WS2812B_LED_QUANTITY;i++)
	{
		WS2812B_Buf[i]=0x000000;
	}
}

void WS2812B_SetBuf(uint32_t Color)//��ɫ
{
	uint8_t i;
	for(i=0;i<WS2812B_LED_QUANTITY;i++)
	{
		WS2812B_Buf[i]=Color;
	}
}

void WS2812B_UpdateBuf(void)
{
	uint8_t i,j;
	for(j=0;j<WS2812B_LED_QUANTITY;j++)
	{
		for(i=0;i<24;i++)
		{
			if(WS2812B_Buf[j]&(0x800000>>i)){WS2812B_Bit[j*24+i+1]=60;}//0x800000Ϊ�����Ƶ�1��23��0��>>:����
			else{WS2812B_Bit[j*24+i+1]=30;}
		}
	}
	DMA1_Start(24*WS2812B_LED_QUANTITY+1);
	TIM2_Cmd(ENABLE);
	while(WS2812B_Flag==0);
	WS2812B_Flag=0;
}

void WS2812B_IRQHandler(void)
{
	TIM2_SetCompare1(0);//pwm����
	TIM2_Cmd(DISABLE);//�رն�ʱ��
	WS2812B_Flag=1;
}
