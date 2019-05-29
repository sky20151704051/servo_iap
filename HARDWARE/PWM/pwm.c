#include "pwm.h"
#include "usart.h"

u8 Servo_state;
void PWM_Init(u16 arr,u16 psc)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;	
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE); //ʱ�Ӷ�ʱ����GPIOA��GPIOBʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3,ENABLE);	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 	
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE); 
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_11);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
		
	
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	//TIM_TimeBaseStructure.TIM_RepetitionCounter =0x00;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ�������ȵ���ģʽ1��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//ʹ�ܱȽ���
	TIM_OCInitStructure.TIM_Pulse = CCR_MIN;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3
	
	if (angle>=0)
		Servo_state = SERVO_0;	
	else 
		Servo_state = SERVO_180;

	TIM_OCInitStructure.TIM_Pulse = Servo_state;	
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM2
	
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR4�ϵ�Ԥװ�ؼĴ���	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR1�ϵ�Ԥװ�ؼĴ���
		 
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
  TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM3
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM3
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3	
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	Set_PWM(CCR_MIN,CCR_MIN, Servo_state, Servo_state);
}

void BLCD_Unlock(void)
{
    Set_PWM(CCR_MAX,CCR_MAX,Servo_state,Servo_state);
    delay_ms(1000);
		delay_ms(1000);
    Set_PWM(CCR_MIN,CCR_MIN,Servo_state,Servo_state);
    delay_ms(1000);
		delay_ms(1000);
}

void Set_PWM(u8 CH1,u8 CH2,u8 CH3,u8 CH4)
{
    TIM_SetCompare1(TIM3,CH1);
    TIM_SetCompare2(TIM3,CH2);
	
	  TIM_SetCompare4(TIM1,CH3);
		TIM_SetCompare2(TIM2,CH4);
}

void Set_PWM1(u8 CH1)
{
    TIM_SetCompare1(TIM3,CH1);
}

void Set_PWM2(u8 CH2)
{
    TIM_SetCompare2(TIM3,CH2);
}

void Set_PWM3(u8 CH3)
{
    TIM_SetCompare4(TIM1,CH3);
}

void Set_PWM4(u8 CH4)
{
    TIM_SetCompare2(TIM2,CH4);
}

void Set_Motor(void)
{
	u16 Lout, Rout;	
	no_cmd_time++;
	if (no_cmd_time> 200){
		Lin = 0;
		Rin = 0;
	  no_cmd_time = 0;			
	}
	Lout = Lin*(CCR_MAX-CCR_MIN)/200+CCR_MIN;			// �˴�200Ϊ���ǵ��ֱ�������ֵ���ܴﵽ255������Ϊ����200��Ϊ���ֵ
	Rout = Rin*(CCR_MAX-CCR_MIN)/200+CCR_MIN;
	
	if (Lout>CCR_MAX)
		Lout = CCR_MAX;
	else if (Lout<CCR_MIN)
		Lout = CCR_MIN;
	if (Rout>CCR_MAX)
		Rout = CCR_MAX;
	else if (Rout<CCR_MIN)
		Rout = CCR_MIN;
	Set_PWM1(Lout);
	Set_PWM2(Rout);
	
/*	if (Lin >150)
		Lout = SERVO_180;
	else if (Lin < 20)
		Lout = SERVO_0;
	else Lout = SERVO_90;*/
	
	if (angle>=0)
		Servo_state = SERVO_0;	
	else 
		Servo_state = SERVO_180;	
	Set_PWM3(Servo_state);
	Set_PWM4(Servo_state);
}











