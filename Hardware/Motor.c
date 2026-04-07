/**
 * @file Motor.c
 * @brief 电机驱动模块
 * @details
 *   - 功能：电机初始化、速度控制、方向控制
 *   - 电机A（左电机）：AIN1(PB0), AIN2(PB1), PWM(PA8-TIM1_CH1)
 *   - 电机B（右电机）：BIN1(PB10), BIN2(PB11), PWM(PA9-TIM1_CH2)
 *   - 工作原理：通过GPIO方向控制+PWM速度控制实现电机驱动
 */

#include "stm32f10x.h"
#include "PWM.h"

/**
 * @brief 电机初始化
 * @details
 *   初始化电机方向控制的GPIO引脚，并调用PWM初始化
 *   - 电机A方向控制引脚：PB0(AIN1), PB1(AIN2)
 *   - 电机B方向控制引脚：PB10(BIN1), PB11(BIN2)
 */
void Motor_Init(void)
{
	// 初始化电机方向控制引脚：PB0, PB1, PB10, PB11
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	// 电机A方向控制：PB0, PB1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// 电机B方向控制：PB10, PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    PWM_Init();
}


/**
 * @brief 取绝对值（备用函数）
 * @param a 输入值
 * @return 绝对值
 */
int myabs(int a)
{
	int temp;
	if(a >= 0)
		temp = a;
	else
		temp = -a;
	return temp;
}

/**
 * @brief 限制电机速度在范围内
 * @details
 *   检查电机速度值是否超出范围，超出范围则修正到最大或最小值
 *   - PWM_MAX 和 PWM_MIN 由PWM.h定义
 * @param motor_left 指向左电机速度的指针
 * @param motor_right 指向右电机速度的指针
 */
void Limit(int *motor_left, int *motor_right)
{
	if (*motor_left > PWM_MAX)  *motor_left = PWM_MAX;
	if (*motor_left < PWM_MIN)  *motor_left = PWM_MIN;
	
    if (*motor_right > PWM_MAX)  *motor_right = PWM_MAX;
	if (*motor_right < PWM_MIN)  *motor_right = PWM_MIN;
}

/**
 * @brief 设置电机转速和方向
 * @details
 *   通过GPIO方向控制引脚和PWM占空比控制两个电机的速度和方向
 *   - 正值：电机正向转动，占空比为速度值
 *   - 负值：电机反向转动，占空比为速度的倒数
 *   - 电机A（左）：GPIO方向(PB0/PB1) + PWM占空比(PA8-TIM1_CH1)
 *   - 电机B（右）：GPIO方向(PB10/PB11) + PWM占空比(PA9-TIM1_CH2)
 * @param motor_l 左电机速度值（范围由PWM_MAX/PWM_MIN定义）
 * @param motor_r 右电机速度值（范围由PWM_MAX/PWM_MIN定义）
 */
void Set_Speed(int motor_l, int motor_r)
{
   /* 电机A（左电机）：PB0-AIN1, PB1-AIN2，PWM-PA8(TIM1_CH1) */
   if (motor_l >= 0) {
       GPIO_SetBits(GPIOB, GPIO_Pin_0);      /* AIN1=1 */
       GPIO_ResetBits(GPIOB, GPIO_Pin_1);    /* AIN2=0，正向转动 */
       TIM_SetCompare1(TIM1, motor_l);       /* 设置PWM占空比 */
   }
   else {
       GPIO_ResetBits(GPIOB, GPIO_Pin_0);    /* AIN1=0 */
       GPIO_SetBits(GPIOB, GPIO_Pin_1);      /* AIN2=1，反向转动 */
       TIM_SetCompare1(TIM1, -motor_l);      /* 设置PWM占空比（取绝对值） */
   }
   
   /* 电机B（右电机）：PB10-BIN1, PB11-BIN2，PWM-PA9(TIM1_CH2) */
   if (motor_r >= 0) {
       GPIO_SetBits(GPIOB, GPIO_Pin_10);     /* BIN1=1 */
       GPIO_ResetBits(GPIOB, GPIO_Pin_11);   /* BIN2=0，正向转动 */
       TIM_SetCompare2(TIM1, motor_r);       /* 设置PWM占空比 */
   }
   else {
       GPIO_ResetBits(GPIOB, GPIO_Pin_10);   /* BIN1=0 */
       GPIO_SetBits(GPIOB, GPIO_Pin_11);     /* BIN2=1，反向转动 */
       TIM_SetCompare2(TIM1, -motor_r);      /* 设置PWM占空比（取绝对值） */
   }
}
