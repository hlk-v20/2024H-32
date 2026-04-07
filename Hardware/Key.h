/**
 * @file Key.h
 * @brief 按键模块头文件
 * @details
 *   - 功能：按键初始化和检测
 *   - KEY1: PB2，KEY2: PB3
 */

#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

/* 函数声明 */
void Key_Init(void);              /*!< 按键初始化 */
uint8_t Key_GetNum(void);         /*!< 获取按键编号 */

#endif
