#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x_flash.h"
#include "iap.h"
#include "stmflash.h"

iapfun jump2app; 
u16 iapbuf[1024];   
u8 iap_flag;
u8 iap_state;
//appxaddr:应用程序的起始地址
//appbuf:应用程序CODE.
//appsize:应用程序大小(字节).
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	u16 t;
	u16 i=0;
	u16 temp;
	u32 fwaddr=appxaddr;//当前写入的地址
	u8 *dfu=appbuf;
	for(t=0;t<appsize;t+=2)
	{						    
		temp=(u16)dfu[1]<<8;
		temp+=(u16)dfu[0];	  
		dfu+=2;//偏移2个字节
		iapbuf[i++]=temp;	    
		if(i==1024)
		{
			i=0;
			STMFLASH_Write(fwaddr,iapbuf,1024);	
			fwaddr+=2048;//偏移2048  16=2*8.所以要乘以2.
		}
	}
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//将最后的一些内容字节写进去.  
}

//跳转到应用程序APP段
//appxaddr:用户代码起始地址.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
}		 

void judge_updata()
{
//		u16 buff[2];
//	uint16_t flag;	
//	buff[0] = 0xffff, buff[0] = 0xffff, STMFLASH_Write(UPDATA_FLAG_ADDR,buff,1);	//更改标识位，更改之后，执行APP程序
//  buff[0] = 0, buff[0] = 0, STMFLASH_Write(UPDATA_FLAG_ADDR,buff,1);	//更改标识位，更改之后，执行IAP程序
//	flag=(*(__IO uint16_t*)(UPDATA_FLAG_ADDR)); //读出标识位，该标识为由APP程序修改
//	if(flag==0xffff)
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) == 0)//如果吸合
	{
		iap_state = 5; //直接执行IAP程序
//		buff[0] = 0xffff, buff[0] = 0xffff, STMFLASH_Write(UPDATA_FLAG_ADDR,buff,1);	//更改标识位，更改之后，执行APP程序
	}
	else
	{
		USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);//禁止接收发送中断，防止跳转时候有外设中断导致死机
    USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	  delay_ms(20);
    iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
	}
}












