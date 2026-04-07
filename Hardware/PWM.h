/**
 * @file PWM.h
 * @brief PWM（脉宽调制）模块头文件
 * @details
 *   - 功能：电机驱动的PWM信号生成
 *   - 定时器：TIM1（中频定时器，72MHz频率）
 *   - PWM输出：PA8(TIM1_CH1-左电机), PA9(TIM1_CH2-右电机)
 *   - 频率：72MHz / (1+71) / 20000 = 50Hz（周期20ms）
 */

#ifndef __PWM_H
#define __PWM_H

/* PWM速度范围定义 */
#define PWM_MAX  50          /*!< PWM最大占空比 */
#define PWM_MIN -50          /*!< PWM最小占空比 */

/* PWM引脚定义 */
#define PWM_PIN_A               GPIO_Pin_8  /*!< PWMA - PA8 (TIM1_CH1-左电机) */
#define PWM_PIN_B               GPIO_Pin_9  /*!< PWMB - PA9 (TIM1_CH2-右电机) */
#define PWM_GPIO                GPIOA       /*!< PWM对应的GPIO端口 */

/* PWM定时器定义 */
#define PWM_TIMX                TIM1        /*!< PWM使用TIM1定时器 */
#define PWM_PORT_CLK            RCC_APB2Periph_GPIOA      /*!< GPIO时钟 */
#define PWM_TIM_CLK             RCC_APB2Periph_TIM1       /*!< 定时器时钟 */

/* 函数声明 */
void PWM_Init(void);  /*!< PWM初始化 */

#endif
