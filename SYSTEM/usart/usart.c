#include "sys.h"
#include "usart.h"	
#include "delay.h"
#include "led.h"
#include "iap.h"
#include "stmflash.h"
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
    USART3->DR = (u8) ch;      
	return ch;
}
#endif 

 
u16 rec_count = 0;
u8	val[4];
u16	Lin = 0;
u16	Rin = 0;
u16 no_cmd_time = 0;
float vol;

u16  USART_RX_CNT=0;			//IAP���յ��ֽ���
u16  USART_IAP_BUF[USART_REC_LEN];
u16  USART_IAP_BUF[USART_REC_LEN] __attribute__ ((at(0X20001000)));//���ջ���,���USART_REC_LEN���ֽ�,��ʼ��ַΪ0X20001000
u8   state, page_erase;
u32 FLASH_addr = FLASH_APP1_ADDR;
  
void uart3_init(u32 bound){
	//GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1);
	
//	GPIO_SetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1);
	
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  
	//USART3_TX   GPIOB.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.10
   
  //USART3_RX	  GPIOB.11��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.11
  
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	
	//Usart3 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

  USART_Init(USART3, &USART_InitStructure); //��ʼ������3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������3�����ж�
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3
	
}

void uart3_send_char(u8 temp) 
{    
    USART_SendData(USART3,(u8)temp);        
    while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);          
}

void uart3_send_buff(u8 buf[],u32 len)
{    
    u32 i;    
    for(i=0;i<len;i++)    
    uart3_send_char(buf[i]);             
}


void USART3_IRQHandler(void)                	//����3�жϷ������
	{
	u8 Res;
	static u8 flag=0;
	static char Last_data;
		
		USART_ClearFlag(USART3,USART_FLAG_TC);

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res = USART_ReceiveData(USART3);	//��ȡ���յ�������
		if(state == 0)//��ͨ����
    {
      USART_IAP_BUF[USART_RX_CNT++] = Res;	
			if(Res == 0xaa)
			    rec_count = 1;
			else if (rec_count >= 1)
			{
				page_erase = Res;
				state = 1;
			}
			else iap_state = 2; //��תAPP
    }
		else
    {    
        if(flag==0)
        {
            flag=1;
            Last_data=Res;
        }
				 else
        {        
            flag=0;
            USART_IAP_BUF[USART_RX_CNT]=(u16)Res<<8;//����õ���������Ϊ��λ
            USART_IAP_BUF[USART_RX_CNT]+=(u16)Last_data;//���ȵõ���������Ϊ��λ
                    
					  FLASH_Unlock();
            FLASH_ProgramHalfWord(FLASH_addr,USART_IAP_BUF[USART_RX_CNT]);
        
            USART_RX_CNT++;
            FLASH_addr = FLASH_addr+0x02;                   
        }
			if(USART_RX_CNT >= USART_REC_LEN/2)  //��д��һҳ��
			{
				USART_RX_CNT = 0;
//				FlashPages++;
			}
		}



				
//		if ( Res == 'R'){
//			rec_count = 1;
//		}
//		else if (rec_count >=1 && rec_count <=4){
//			val[rec_count-1] = Res;
//			rec_count++;
//		}
//		if (rec_count == 5){
//			if ( (val[0]==val[2])&&(val[1]==val[3]) ){
//				if( angle >=0 ){
//					Lin = val[0];
//					Rin = val[1];
//				}
//				else{
//					Lin = val[1];
//					Rin = val[0];
//				}				
//				no_cmd_time = 0;
//				LED_WORK = !LED_WORK;
//			}			
//		}
	}
} 	

void USART3_Remote_Mode(u8 m0, u8 m1){
	M0 = m0;
	M1 = m1;
}

 void write_E32_mode(void){	 
	 u8 buf[6] = {0xc0,0x00,0x00,0x1c,0x0a,0x47};
	 u8 buf1[3] = {0xc1,0xc1,0xc1};	
	USART3_Remote_Mode(1,1);
	delay_ms(1000);	
	uart3_send_buff(buf,6);	
	delay_ms(2000);	
	uart3_send_buff(buf1,3);
	while(1){		
		delay_ms(2000);		
	}	
}
 
void Send_Controller_Vol(void){
	if (global_count%500==0) //ÿ5000msһ��
	{
		u8 buf[3];
		float temp_vol = vol*100;
		if (temp_vol>255)
			temp_vol = 255;
		if (temp_vol<=0)
			temp_vol = 1;
		buf[0] = 'W';
		buf[1] = 'B';
		buf[2] = temp_vol;			
		uart3_send_buff(buf, 3);	
	}	
}



