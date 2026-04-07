/**
 * @file buzzer.h
 * @brief 蜂鸣器模块头文件
 * @details
 *   蜂鸣器连接PA10引脚，用于声音提示
 */

#ifndef __BUZZER_H
#define __BUZZER_H

/* 函数声明 */
void buzzer_Init(void);   /*!< 蜂鸣器初始化 */
void buzzer_on(void);     /*!< 蜂鸣器打开 */
void buzzer_off(void);    /*!< 蜂鸣器关闭 */
void buzzer_turn(void);   /*!< 蜂鸣器翻转（鸣一声） */

#endif 
