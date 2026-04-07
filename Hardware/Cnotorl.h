/**
 * @file Cnotorl.h
 * @brief 灰度传感和电机控制模块头文件
 * @details
 *   - 功能：计算灰度线偏差、PID控制、电机速度获取
 *   - 5路灰度传感器状态识别，31种可能状态
 *   - 位置PID控制（Kp=2.5, Kd=3.5）
 *   - 角度PID控制（Kap=0.5, Kad=3.5）
 */

#ifndef __ENCODER_H
#define __ENCODER_H

/* 灰度线跟踪函数 */
float Track_err(void);  /*!< 计算灰度线偏差值 */

/* PID控制函数 */
int PID_out(float error, int Target);   /*!< 位置PID输出 */
int pid_angle2(int target, int yaw);    /*!< 角度PID输出 */

/* 电机控制函数 */
void Final_Speed(int pid_out, int base_speed);  /*!< 计算最终电机速度 */

/* 编码器相关函数 */
int Get_Motor_Speed_Left(void);   /*!< 获取左电机转速 */
int Get_Motor_Speed_Right(void);  /*!< 获取右电机转速 */
void Encoder_Init(void);          /*!< 编码器初始化 */

#endif







