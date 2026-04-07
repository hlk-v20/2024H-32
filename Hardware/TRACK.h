/**
 * @file TRACK.h
 * @brief 灰度传感器模块头文件
 * @details
 *   - 灰度传感器：5路传感器
 *   - 引脚定义：PA2, PA3, PA4(A端)，PB4, PB5(B端)
 */

#ifndef __TRACK_H
#define __TRACK_H

#include "stm32f10x.h"

/* GPIO端口定义 */
#define TRACK_PORTA GPIOA
#define TRACK_PORTB GPIOB

/* GPIO时钟使能定义 */
#define TRACK_PORT_CLKA RCC_APB2Periph_GPIOA
#define TRACK_PORT_CLKB RCC_APB2Periph_GPIOB

/* 灰度传感器引脚定义（5路） */
#define TRACK_INFRARED_PIN_1 GPIO_Pin_2  /* out1: PA2 */
#define TRACK_INFRARED_PIN_2 GPIO_Pin_3  /* out2: PA3 */
#define TRACK_INFRARED_PIN_3 GPIO_Pin_4  /* out3: PA4 */
#define TRACK_INFRARED_PIN_4 GPIO_Pin_4  /* out4: PB4 */
#define TRACK_INFRARED_PIN_5 GPIO_Pin_5  /* out5: PB5 */

/* 函数声明 */
void Track_Init(void);           /*!< 灰度传感器初始化 */
u8 Get_Infrared_State(void);     /*!< 读取灰度传感器状态 */

#endif


