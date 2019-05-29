#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "adxl345.h"
#include "pwm.h"
#include "adc.h"
#include "iap.h"

#define WRITE_E32_MODE 0				// �����Ҫ������ģ��д������������Ϊ1
#define ARR_50HZ 1999
#define PSC_50HZ 719


u32 global_count=0;
int main(void)
 {		
	u32 oldcount = 0;				//IAP�ϵĴ��ڽ�������ֵ
	u16 applenth = 0;				//���յ���app���볤��
  u8 i;
	iap_flag = 0;
	 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart3_init(9600);	 //���ڳ�ʼ��Ϊ9600
 	delay_init();	    //��ʱ������ʼ��	
	LED_Init();			     //LED�˿ڳ�ʼ��	
	KEY_Init();	      //������ʼ��	 
	if (WRITE_E32_MODE)	write_E32_mode();		// ��������ģ��Ĳ�������һ�Σ�
	judge_updata();   //�ж��Ƿ���³���
 	while(1)
	{		
  	global_count++;
		delay_ms(10);	
		if(global_count % 5 == 0)
		LED_WORK = !LED_WORK;
		
    if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) == 1)//���û������
		{
			iap_state = 2; //�����;�Ͽ�����תAPP
		}   
    else if(state == 1)//������յ�����ָ��
		{
			FLASH_Unlock();
			FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
			if(page_erase <= 100){//���������ݿ�����100k����
			for(i = 0;i < page_erase; i++)
					FLASH_ErasePage(FLASH_APP1_ADDR + i*1024);
      USART_SendData(USART3,'o'),delay_ms(20);
			USART_SendData(USART3,'k');
			state = 2;
			}
			else USART_SendData(USART3,'o'),delay_ms(2),USART_SendData(USART3,'u'),delay_ms(2),USART_SendData(USART3,'t'),state = 0;
		}			
		if((FLASH_addr != FLASH_APP1_ADDR) && (iap_state == 5)&&(global_count % 200 == 0))
		{
			if(oldcount == FLASH_addr)//��������,û���յ��κ�����,��Ϊ�������ݽ������.
			{
				applenth = FLASH_addr - FLASH_APP1_ADDR;
				oldcount = 0;
				FLASH_addr = FLASH_APP1_ADDR;
				USART_SendData(USART3,'1');
				iap_state = 2;
				if(applenth < 100)//С��100���ֽ�һ����ָ����ǳ����ֱ����ת
					iap_state = 2;
			}else oldcount = FLASH_addr;			
		}
		
//		else 	if(iap_state == 1)
//		{
//			if(applenth)
//			{
//				delay_ms(20);
//				USART_SendData(USART3,'2');	
// 				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
//				{	 
//					iap_write_appbin(FLASH_APP1_ADDR,USART_IAP_BUF,applenth);//����FLASH����   
//					USART_SendData(USART3,'3');		
//					iap_state = 2;
//				}else 
//				{	   
//					USART_SendData(USART3,'a');	
//					iap_state = 0;
//				}
// 			}else 
//			{
//				USART_SendData(USART3,'b');	
//				iap_state = 0;
//			}								 
//		} 
			if(iap_state == 2)
		{
			delay_ms(20);
			USART_SendData(USART3,'4');
			delay_ms(20);
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);//��ֹ���շ����жϣ���ֹ��תʱ���������жϵ�������
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
			delay_ms(20);
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
			}else 
			{
				USART_SendData(USART3,'c');  
			}									 
			iap_state = 3;	  
		}
		
	}	 
 }

