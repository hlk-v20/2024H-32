/**
 * @file Timer.h
 * @brief 定时器模块头文件
 * @details
 *   - 功能：TIM3定时器配置，用于编码器接口
 *   - 定时器：TIM3（编码器模式）
 *   - 引脚：PA6(CH1), PA7(CH2)
 */

#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

/* 函数声明 */
void TIM3_Int_Init(u16 arr, u16 psc);  /*!< TIM3初始化 */

#endif

