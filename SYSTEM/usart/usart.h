#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include "iap.h"

#define USART_REC_LEN  			2*1024  	//һҳ2048�ֽ�

#define M0 PAout(0)
#define M1 PAout(1)

extern u16	Lin;
extern u16	Rin;
extern u16 no_cmd_time;
extern short angle;
extern float vol;
extern u32 global_count;
extern u16 USART_RX_CNT;				//IAP���յ��ֽ���
extern u16  USART_IAP_BUF[USART_REC_LEN];
extern u32 FLASH_addr;
extern u8   state, page_erase;

//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart3_init(u32 bound);
void uart3_send_char(u8 temp);
void uart3_send_buff(u8 buf[],u32 len);
void USART3_Remote_Mode(u8 m0, u8 m1);
void write_E32_mode(void);
void Send_Controller_Vol(void);
#endif


