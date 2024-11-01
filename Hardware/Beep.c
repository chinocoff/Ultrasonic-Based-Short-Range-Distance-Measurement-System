#include "stm32f10x.h"                  // Device header

/**
  * ��    ����Beep��ʼ��
  * ��    ������
  * �� �� ֵ����
  */
void Beep_Init(void)
{
	/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//����GPIOB��ʱ��
	                        
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;  //PB14
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//��PB14���ų�ʼ��Ϊ�������
	          
	/*����GPIO��ʼ�����Ĭ�ϵ�ƽ*/
	GPIO_SetBits(GPIOB, GPIO_Pin_14);				//����PB14����Ϊ�ߵ�ƽ,��ʱ������
}

/**
  * ��    ����Beep����
  * ��    ������
  * �� �� ֵ����
  */
void Beep_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);		//����PB14����Ϊ�ߵ�ƽ
}

/**
  * ��    ����Beep�ر�
  * ��    ������
  * �� �� ֵ����
  */
void Beep_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_14);		//����PB14����Ϊ�͵�ƽ
}

/**
  * ��    ����Beep״̬��ת
  * ��    ������
  * �� �� ֵ����
  */
void Beep_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_14) == 0)		//��ȡ����Ĵ�����״̬�������ǰ��������͵�ƽ
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_14);					//������PB14����Ϊ�ߵ�ƽ
	}
	else													//���򣬼���ǰ��������ߵ�ƽ
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_14);					//������PB14����Ϊ�͵�ƽ
	}
}
