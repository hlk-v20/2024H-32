/**
 * @file PWM.c
 * @brief PWM（脉宽调制）电机驱动模块
 * @details
 *   - 功能：初始化TIM1产生PWM信号，用于控制电机转速
 *   - 定时器：TIM1（AP2总线，频率72MHz）
 *   - PWM输出：PA8(TIM1_CH1-左电机), PA9(TIM1_CH2-右电机)
 *   - PWM频率：72MHz / 72(分频) / 100(周期) = 10KHz
 */

#include "stm32f10x.h"
#include "PWM.h"

/**
 * @brief PWM初始化
 * @details
 *   配置TIM1定时器产生两路PWM信号
 *   - 配置PA8和PA9为复用推挽输出（TIM1的CH1和CH2）
 *   - 设置PWM模式为PWM1
 *   - 分频系数：72（TIM_Prescaler = 71）
 *   - 自动重装载值：100（TIM_Period = 99）
 *   - TIM1属于高级定时器，需要使能主输出PWM
 */
void PWM_Init(void)
{
	/* 使能TIM1和GPIOA的时钟 */
	RCC_APB2PeriphClockCmd(PWM_TIM_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* 配置GPIO引脚：PA8(TIM1_CH1), PA9(TIM1_CH2)为复用推挽输出 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = PWM_PIN_A | PWM_PIN_B;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PWM_GPIO, &GPIO_InitStructure);
	
	/* 配置TIM1内部时钟 */
	TIM_InternalClockConfig(PWM_TIMX);
	
	/* 配置TIM1工作参数 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;      /* ARR = 99 */
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;    /* PSC = 71 */
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(PWM_TIMX, &TIM_TimeBaseInitStructure);
	
	/* 配置输出比较模式（PWM1模式） */
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;    /* CCR初值为0 */
	TIM_OC1Init(PWM_TIMX, &TIM_OCInitStructure);  /* PA8 配置为PWM输出 */
	TIM_OC2Init(PWM_TIMX, &TIM_OCInitStructure);  /* PA9 配置为PWM输出 */
	
	/* TIM1为高级定时器，需要使能主输出 */
	TIM_CtrlPWMOutputs(PWM_TIMX, ENABLE);
	
	/* 启动TIM1计数 */
	TIM_Cmd(PWM_TIMX, ENABLE);
}


