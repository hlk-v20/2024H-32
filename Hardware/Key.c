/**
 * @file Key.c
 * @brief 按键模块
 * @details
 *   - 功能：按键初始化和状态检测
 *   - KEY1: PB2，KEY2: PB3
 *   - 按键模式：上拉输入，低有效
 */

#include "stm32f10x.h"
#include "Delay.h"

/**
 * @brief 按键初始化
 * @details
 *   初始化PB2和PB3为上拉输入模式
 *   禁用JTAG调试接口，释放PB2和PB3用于按键
 */
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;  /* PB2和PB3 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
 * @brief 获取按键编号
 * @details
 *   检测按键按下，附带20ms防抖延迟
 *   - 返回1：KEY1(PB2)被按下
 *   - 返回2：KEY2(PB3)被按下
 *   - 返回0：无按键按下
 * @return 按键编号(0/1/2)
 */
uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0)  /* 按键1: PB2 */
	{
		Delay_ms(20);  /* 防抖 */
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0);  /* 等待释放 */
		Delay_ms(20);  /* 防抖 */
		KeyNum = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0)  /* 按键2: PB3 */
	{
		Delay_ms(20);  /* 防抖 */
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0);  /* 等待释放 */
		Delay_ms(20);  /* 防抖 */
		KeyNum = 2;
	}
	
	return KeyNum;
}
