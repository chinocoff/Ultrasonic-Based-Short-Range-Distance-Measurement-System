#include "stm32f10x.h"                  // Device header
#include "Delay.h"
//#include "Key.h"

u8 flag=0;				//���ڼ�¼�ж��ź��������ػ����½���
u32 number=0;			//��¼��ʱ���жϵĴ���
u32 dis=0;			  //

void HCSR04_Init(void)
{
	/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//����GPIOA��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;                    //Trigʹ���������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;        //Echoʹ�ø�������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);           //ʹ��AFIO������ʱ��
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7);    //ѡ���ⲿ�ж�Դ���ж�ͨ��
	
	//EXTI�ж�����
  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_InitStructure.EXTI_Line=EXTI_Line7;                      //ѡ��7���ж���
  EXTI_InitStructure.EXTI_LineCmd=ENABLE;
  EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;  //�����غ��½��ض������ж�
  EXTI_Init(&EXTI_InitStructure);
	
	//NVIC�����ⲿ�жϵ��ж����ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //ָ���жϷ���
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=EXTI9_5_IRQn;              //ʹ�õĶ˿�7,���ѡ���������
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;       //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;              //��Ӧ���ȼ� 
	NVIC_Init(&NVIC_InitStructure);
	
	//���ö�ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				    	//����ṹ�����
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //ʱ�ӷ�Ƶ��ѡ�񲻷�Ƶ���˲������������˲���ʱ�ӣ���Ӱ��ʱ����Ԫ����
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //������ģʽ��ѡ�����ϼ���
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;                 //�������ڣ���ARR��ֵ 
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;               //Ԥ��Ƶ������PSC��ֵ  
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //�ظ����������߼���ʱ���Ż��õ�
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);             //���ṹ���������TIM_TimeBaseInit������TIM2��ʱ����Ԫ
		                                                              //��������Ϊ1us,��1000����Ϊ1ms,����һ���ж�
																																	
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                    		//ʹ���ж�����ź�
  TIM_InternalClockConfig(TIM3);                              		//ѡ���ڲ�ʱ��   

	//NVIC���䶨ʱ�����ж����ȼ�
	NVIC_InitTypeDef NVIC_InitStructureTIM3;
	NVIC_InitStructureTIM3.NVIC_IRQChannel=TIM3_IRQn;                //ָ��Tim2���ж�ͨ��
	NVIC_InitStructureTIM3.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructureTIM3.NVIC_IRQChannelPreemptionPriority=1;      //��ռ���ȼ�
	NVIC_InitStructureTIM3.NVIC_IRQChannelSubPriority=1;             //��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructureTIM3);
	
}


//��ʱ���жϺ���
void TIM3_IRQHandler(void){
    if(SET==TIM_GetITStatus(TIM3,TIM_FLAG_Update))
		{
      number++;                                   //ÿ���жϽ�����++
      TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
     }
}


//�ⲿ�жϺ���
void EXTI9_5_IRQHandler(void)
{
	if(SET==EXTI_GetITStatus(EXTI_Line7))
	{
		if(flag==0)   //�����ؼ������ƽ��ʼ,�򿪼�����
		{
			number=0;flag=1;
			TIM_SetCounter(TIM3,0);
			TIM_Cmd(TIM3,ENABLE);
				
		 }
		else    //�½��ؼ������ƽ����,ͳ�Ƹߵ�ƽ����ʱ��
		{ 
			TIM_Cmd(TIM3,DISABLE);
			flag=0;
			dis=(number*1000+TIM_GetCounter(TIM3))*0.34/2;  //�õ�����mm
		 }
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
}

extern u8 Keynum;

u32 Distance_get(void)
{
	 u32 distance=0;
	 for(int i=0;i<10;++i)   //ÿ��ȡ10�β������,ȡƽ��ֵ�������
	 {              
		 GPIO_SetBits(GPIOA,GPIO_Pin_6);
		 Delay_us(15);                   //����˵����,��Ҫ�ṩ����10us�ĸߵ�ƽ
		 GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		 Delay_ms(65);                   //����˵����,ÿ������������Ҫ�ȴ�60ms
		 distance=distance+dis;          //����˵�����ṩ�Ĺ�ʽ,��ȡ��λΪmm�ľ���
		}
		 
   distance=distance/10;          //ȡʮ�ε�ƽ��ֵ
	 return distance;
}
