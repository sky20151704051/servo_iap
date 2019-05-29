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
//appxaddr:Ӧ�ó������ʼ��ַ
//appbuf:Ӧ�ó���CODE.
//appsize:Ӧ�ó����С(�ֽ�).
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	u16 t;
	u16 i=0;
	u16 temp;
	u32 fwaddr=appxaddr;//��ǰд��ĵ�ַ
	u8 *dfu=appbuf;
	for(t=0;t<appsize;t+=2)
	{						    
		temp=(u16)dfu[1]<<8;
		temp+=(u16)dfu[0];	  
		dfu+=2;//ƫ��2���ֽ�
		iapbuf[i++]=temp;	    
		if(i==1024)
		{
			i=0;
			STMFLASH_Write(fwaddr,iapbuf,1024);	
			fwaddr+=2048;//ƫ��2048  16=2*8.����Ҫ����2.
		}
	}
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//������һЩ�����ֽ�д��ȥ.  
}

//��ת��Ӧ�ó���APP��
//appxaddr:�û�������ʼ��ַ.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}
}		 

void judge_updata()
{
//		u16 buff[2];
//	uint16_t flag;	
//	buff[0] = 0xffff, buff[0] = 0xffff, STMFLASH_Write(UPDATA_FLAG_ADDR,buff,1);	//���ı�ʶλ������֮��ִ��APP����
//  buff[0] = 0, buff[0] = 0, STMFLASH_Write(UPDATA_FLAG_ADDR,buff,1);	//���ı�ʶλ������֮��ִ��IAP����
//	flag=(*(__IO uint16_t*)(UPDATA_FLAG_ADDR)); //������ʶλ���ñ�ʶΪ��APP�����޸�
//	if(flag==0xffff)
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) == 0)//�������
	{
		iap_state = 5; //ֱ��ִ��IAP����
//		buff[0] = 0xffff, buff[0] = 0xffff, STMFLASH_Write(UPDATA_FLAG_ADDR,buff,1);	//���ı�ʶλ������֮��ִ��APP����
	}
	else
	{
		USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);//��ֹ���շ����жϣ���ֹ��תʱ���������жϵ�������
    USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	  delay_ms(20);
    iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
	}
}












