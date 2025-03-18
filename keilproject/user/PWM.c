#include <stm32f10x.h>

void tim2_pwm_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// TIM4 clock enable /
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	// GPIOB and AFIO clock enable /
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	//????????,???????
	//??JTAG PB4????IO
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	//TIM3_CH1 TIM3_CH2 ????????PB4 PB5
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
	//GPIOB Configuration: TIM4 channel1, 2, 3 and 4 /
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 20000 - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
	//???????,????????????0
	//?????????????????
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	//????
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	// PWM1 Mode configuration: Channel2 /
	//???????????????????,??2??
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1500;
	//??????????
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	//????????
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

	//??????????
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	// TIM3 enable counter /
	TIM_Cmd(TIM2, ENABLE);
	
	TIM_SetCompare2(TIM2, 1500);
}


void tim3_pwm_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// TIM4 clock enable /
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	// GPIOB and AFIO clock enable /
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	//????????,???????
	//??JTAG PB4????IO
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	//TIM3_CH1 TIM3_CH2 ????????PB4 PB5
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	//GPIOB Configuration: TIM4 channel1, 2, 3 and 4 /
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 20000 - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
	//???????,????????????0
	//?????????????????
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	//????
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	// PWM1 Mode configuration: Channel2 /
	//???????????????????,??2??
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1500;
	//??????????
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	//????????
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	//??????????
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	// TIM3 enable counter /
	TIM_Cmd(TIM3, ENABLE);
	
	TIM_SetCompare1(TIM3, 1500);
}

void tim4_pwm_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// TIM4 clock enable /
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	// GPIOB and AFIO clock enable /
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	//????????,???????
	//??JTAG PB4????IO
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	//TIM3_CH1 TIM3_CH2 ????????PB4 PB5
	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	
	
	//GPIOB Configuration: TIM4 channel1, 2, 3 and 4 /
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 20000 - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
	//???????,????????????0
	//?????????????????
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	//????
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	// PWM1 Mode configuration: Channel2 /
	//???????????????????,??2??
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1500;
	//??????????
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	
	//????????
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	//??????????
	TIM_ARRPreloadConfig(TIM4, ENABLE);

	// TIM3 enable counter /
	TIM_Cmd(TIM4, ENABLE);
	
	//??????
	//TIM_SetAutoreload(TIM4, 20000);
	
	TIM_SetCompare1(TIM4, 1500);
	TIM_SetCompare2(TIM4, 1500);
	TIM_SetCompare3(TIM4, 1500);
	TIM_SetCompare4(TIM4, 1500);
}
