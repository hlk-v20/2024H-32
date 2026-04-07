/**
 * @file LED.c
 * @brief LED指示灯模块
 * @details
 *   - 功能：LED初始化和控制
 *   - LED1：PA11，高电平灭，低电平亮（共阳）
 */

#include "stm32f10x.h"
#include "Delay.h"

/**
 * @brief LED初始化
 * @details
 *   初始化PA11为推挽输出模式，用于控制LED1
 */
void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  /* LED1: PA11 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_11);  /* 初始化为熄灭 */
}

/**
 * @brief LED1点亮
 * @details
 *   将PA11拉低，点亮LED
 */
void LED1_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}

/**
 * @brief LED1熄灭
 * @details
 *   将PA11拉高，熄灭LED
 */
void LED1_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_11);
}

/**
 * @brief LED1翻转（闪烁一次）
 * @details
 *   LED点亮100ms，然后熄灭
 */
void LED1_Turn(void)
{
	LED1_ON();
	Delay_ms(100);
	LED1_OFF();
}


