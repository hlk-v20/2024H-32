/**
 * @file Motor.h
 * @brief 电机驱动模块头文件
 * @details
 *   - 功能：电机初始化、方向控制、速度设置
 *   - 电机驱动方式：GPIO方向控制 + PWM占空比调速
 *   - 左电机：PA8(PWM), PB0/PB1(方向)
 *   - 右电机：PA9(PWM), PB10/PB11(方向)
 */

#ifndef __MOTOR_H
#define __MOTOR_H

/* 函数声明 */
void Motor_Init(void);                           /*!< 电机初始化 */
int myabs(int a);                               /*!< 绝对值函数 */
void Limit(int *motor_left, int *motor_right);  /*!< 限速函数 */
void Set_Speed(int motor_l, int motor_r);       /*!< 设置电机速度 */

#endif



