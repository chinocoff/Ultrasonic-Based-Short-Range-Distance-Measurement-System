#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Key.h"
#include "LED.h"
#include "Beep.h"
#include "OLED.h"
#include "Serial.h"
#include <stdio.h>
#include "HCSR04.h"

u8 Keynum;
float Distance;

int main(void)
{
	LED_Init();
	Key_Init();
	Beep_Init();
	OLED_Init();
	Serial_Init();
	HCSR04_Init();
	OLED_ShowString(4,1,"HelloWorld");
	
	while (1)
	{
		Distance=Distance_get();
    OLED_ShowFloat(1,6,Distance/10,1);
		OLED_ShowString(1,5,":");
		OLED_ShowChinese(1,1,0);
		OLED_ShowChinese(1,2,1);
		OLED_ShowChinese(1,6,2);
		OLED_ShowChinese(1,7,3);
		
		if((Distance/10)<100)OLED_ShowString(1,10," "); //清除屏幕上残留的数字
		
		if(Distance<=200)   //距离小于20cm时
		{	
		  LED1_Turn();
      Beep_Turn();	
			LED2_OFF();
		 }			
		else 
		{
			LED1_OFF();
			Beep_OFF();
			LED2_ON();
		 }
		Delay_ms(100);
	}
}
