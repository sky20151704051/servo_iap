#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

#define VOL_HIGH 2.388				// 3.98v  80%
#define VOL_LOW 2.244					// 3.74   20%

extern u32 global_count;
extern float vol;

void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 
void Set_Vol_Led(void);
 
#endif 
