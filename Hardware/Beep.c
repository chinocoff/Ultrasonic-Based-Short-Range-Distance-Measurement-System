#include "stm32f10x.h"                  // Device header

/**
  * 函    数：Beep初始化
  * 参    数：无
  * 返 回 值：无
  */
void Beep_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	                        
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;  //PB14
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//将PB14引脚初始化为推挽输出
	          
	/*设置GPIO初始化后的默认电平*/
	GPIO_SetBits(GPIOB, GPIO_Pin_14);				//设置PB14引脚为高电平,此时不鸣叫
}

/**
  * 函    数：Beep开启
  * 参    数：无
  * 返 回 值：无
  */
void Beep_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);		//设置PB14引脚为高电平
}

/**
  * 函    数：Beep关闭
  * 参    数：无
  * 返 回 值：无
  */
void Beep_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_14);		//设置PB14引脚为低电平
}

/**
  * 函    数：Beep状态翻转
  * 参    数：无
  * 返 回 值：无
  */
void Beep_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_14) == 0)		//获取输出寄存器的状态，如果当前引脚输出低电平
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_14);					//则设置PB14引脚为高电平
	}
	else													//否则，即当前引脚输出高电平
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_14);					//则设置PB14引脚为低电平
	}
}
