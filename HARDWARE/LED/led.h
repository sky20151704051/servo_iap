#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED_WORK PCout(9)// PC9

/*
#define LED_VOL_HIGH PAout(10)// PA10	D5
#define LED_VOL_MID PAout(11)// PA11	D4
#define LED_VOL_LOW PAout(12)// PA12	D3
*/
void LED_Init(void);//≥ı ºªØ
void KEY_Init(void);
		 				    
#endif
