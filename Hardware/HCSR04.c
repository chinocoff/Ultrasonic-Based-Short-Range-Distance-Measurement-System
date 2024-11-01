#include "stm32f10x.h"                  // Device header
#include "Delay.h"
//#include "Key.h"

u8 flag=0;				//用于记录中断信号是上升沿还是下降沿
u32 number=0;			//记录定时器中断的次数
u32 dis=0;			  //

void HCSR04_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;                    //Trig使用推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;        //Echo使用浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);           //使能AFIO的外设时针
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7);    //选择外部中断源和中断通道
	
	//EXTI中断配置
  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_InitStructure.EXTI_Line=EXTI_Line7;                      //选择7号中断线
  EXTI_InitStructure.EXTI_LineCmd=ENABLE;
  EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;  //上升沿和下降沿都触发中断
  EXTI_Init(&EXTI_InitStructure);
	
	//NVIC分配外部中断的中断优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //指定中断分组
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=EXTI9_5_IRQn;              //使用的端口7,因此选择这个参数
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;       //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;              //响应优先级 
	NVIC_Init(&NVIC_InitStructure);
	
	//配置定时器
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				    	//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;                 //计数周期，即ARR的值 
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;               //预分频器，即PSC的值  
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);             //将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元
		                                                              //计数周期为1us,计1000个数为1ms,进入一次中断
																																	
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                    		//使能中断输出信号
  TIM_InternalClockConfig(TIM3);                              		//选择内部时钟   

	//NVIC分配定时器的中断优先级
	NVIC_InitTypeDef NVIC_InitStructureTIM3;
	NVIC_InitStructureTIM3.NVIC_IRQChannel=TIM3_IRQn;                //指定Tim2的中断通道
	NVIC_InitStructureTIM3.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructureTIM3.NVIC_IRQChannelPreemptionPriority=1;      //抢占优先级
	NVIC_InitStructureTIM3.NVIC_IRQChannelSubPriority=1;             //响应优先级
	NVIC_Init(&NVIC_InitStructureTIM3);
	
}


//定时器中断函数
void TIM3_IRQHandler(void){
    if(SET==TIM_GetITStatus(TIM3,TIM_FLAG_Update))
		{
      number++;                                   //每次中断将次数++
      TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
     }
}


//外部中断函数
void EXTI9_5_IRQHandler(void)
{
	if(SET==EXTI_GetITStatus(EXTI_Line7))
	{
		if(flag==0)   //上升沿即回响电平开始,打开计数器
		{
			number=0;flag=1;
			TIM_SetCounter(TIM3,0);
			TIM_Cmd(TIM3,ENABLE);
				
		 }
		else    //下降沿即回响电平结束,统计高电平持续时长
		{ 
			TIM_Cmd(TIM3,DISABLE);
			flag=0;
			dis=(number*1000+TIM_GetCounter(TIM3))*0.34/2;  //得到距离mm
		 }
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
}

extern u8 Keynum;

u32 Distance_get(void)
{
	 u32 distance=0;
	 for(int i=0;i<10;++i)   //每次取10次测距数据,取平均值减少误差
	 {              
		 GPIO_SetBits(GPIOA,GPIO_Pin_6);
		 Delay_us(15);                   //根据说明书,需要提供至少10us的高电平
		 GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		 Delay_ms(65);                   //根据说明书,每个周期至少需要等待60ms
		 distance=distance+dis;          //根据说明书提供的公式,获取单位为mm的距离
		}
		 
   distance=distance/10;          //取十次的平均值
	 return distance;
}
