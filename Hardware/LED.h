/**
 * @file LED.h
 * @brief LED指示灯模块头文件
 * @details
 *   LED1连接PA11引脚，用于状态指示
 */

#ifndef __LED_H
#define __LED_H

/* 函数声明 */
void LED_Init(void);   /*!< LED初始化 */
void LED1_ON(void);    /*!< LED1点亮 */
void LED1_OFF(void);   /*!< LED1熄灭 */
void LED1_Turn(void);  /*!< LED1翻转 */

#endif
