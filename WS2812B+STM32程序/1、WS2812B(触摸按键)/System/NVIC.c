#include "stm32f10x.h"

/**
  * @brief  中断分组
  * @param  无
  * @retval 无
  */
void NVIC_GroupConfig(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}
