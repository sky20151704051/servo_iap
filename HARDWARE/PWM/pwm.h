#ifndef __PWM_H__
#define __PWM_H__

#include "stm32f10x.h"
#include  "sys.h"
#include <string.h>
#include "stm32f10x_tim.h"
#include "delay.h"
 
#define CCR_MIN 100 //��С����
#define CCR_MAX 200 // �������

#define SERVO_0 50 //���0��
#define SERVO_180 250 //���180��
#define SERVO_90	150 // ���90��

extern u32 global_count;
extern short angle;

void PWM_Init(u16 arr,u16 psc);
void BLCD_Unlock(void);
void Set_PWM(u8 CH1,u8 CH2, u8 CH3, u8 CH4);
void Set_PWM1(u8 CH1);
void Set_PWM2(u8 CH2);
void Set_PWM3(u8 CH3);
void Set_PWM4(u8 CH4);
void Set_Motor(void);
void Set_Servo(void);
#endif





