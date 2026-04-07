/**
 * @file buzzer.c
 * @brief 蜂鸣器模块
 * @details
 *   - 功能：蜂鸣器初始化和控制
 *   - 蜂鸣器：PA10，高电平灭，低电平鸣（共阳）
 */

#include "stm32f10x.h"
#include "Delay.h"

/**
 * @brief 蜂鸣器初始化
 * @details
 *   初始化PA10为推挽输出模式，用于控制蜂鸣器
 */
void buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  /* 蜂鸣器: PA10 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_10);  /* 初始化为关闭 */
}

/**
 * @brief 蜂鸣器打开
 * @details
 *   将PA10拉低，打开蜂鸣器
 */
void buzzer_on(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_10);
}

/**
 * @brief 蜂鸣器关闭
 * @details
 *   将PA10拉高，关闭蜂鸣器
 */
void buzzer_off(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_10);
}

/**
 * @brief 蜂鸣器翻转（鸣一声）
 * @details
 *   蜂鸣器打开100ms，然后关闭
 */
void buzzer_turn(void)
{
	buzzer_on();
	Delay_ms(100);
	buzzer_off();
}



