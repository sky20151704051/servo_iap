#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "adxl345.h"
#include "pwm.h"
#include "adc.h"
#include "iap.h"

#define WRITE_E32_MODE 0				// 如果需要对无线模块写参数，则设置为1
#define ARR_50HZ 1999
#define PSC_50HZ 719


u32 global_count=0;
int main(void)
 {		
	u32 oldcount = 0;				//IAP老的串口接收数据值
	u16 applenth = 0;				//接收到的app代码长度
  u8 i;
	iap_flag = 0;
	 
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart3_init(9600);	 //串口初始化为9600
 	delay_init();	    //延时函数初始化	
	LED_Init();			     //LED端口初始化	
	KEY_Init();	      //按键初始化	 
	if (WRITE_E32_MODE)	write_E32_mode();		// 更改无线模块的参数（用一次）
	judge_updata();   //判断是否更新程序
 	while(1)
	{		
  	global_count++;
		delay_ms(10);	
		if(global_count % 5 == 0)
		LED_WORK = !LED_WORK;
		
    if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) == 1)//如果没有吸合
		{
			iap_state = 2; //如果中途断开就跳转APP
		}   
    else if(state == 1)//如果接收到擦除指令
		{
			FLASH_Unlock();
			FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
			if(page_erase <= 100){//擦除的内容控制在100k以内
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
			if(oldcount == FLASH_addr)//新周期内,没有收到任何数据,认为本次数据接收完成.
			{
				applenth = FLASH_addr - FLASH_APP1_ADDR;
				oldcount = 0;
				FLASH_addr = FLASH_APP1_ADDR;
				USART_SendData(USART3,'1');
				iap_state = 2;
				if(applenth < 100)//小于100个字节一般是指令，不是程序就直接跳转
					iap_state = 2;
			}else oldcount = FLASH_addr;			
		}
		
//		else 	if(iap_state == 1)
//		{
//			if(applenth)
//			{
//				delay_ms(20);
//				USART_SendData(USART3,'2');	
// 				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
//				{	 
//					iap_write_appbin(FLASH_APP1_ADDR,USART_IAP_BUF,applenth);//更新FLASH代码   
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
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);//禁止接收发送中断，防止跳转时候有外设中断导致死机
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
			delay_ms(20);
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
			}else 
			{
				USART_SendData(USART3,'c');  
			}									 
			iap_state = 3;	  
		}
		
	}	 
 }

