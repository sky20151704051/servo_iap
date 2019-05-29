#include "led.h"

void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //LED0-->PC.9 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_SetBits(GPIOC,GPIO_Pin_9);						 //PC.9 �����

/*
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 |GPIO_Pin_12;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 
 GPIO_SetBits(GPIOA,GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12);	
*/	
}

void KEY_Init(void) //IO��ʼ��  PB4����
{ 
   GPIO_InitTypeDef  GPIO_InitStructure;   
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                  
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
}
 
