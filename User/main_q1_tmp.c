/**
 * @file main.c
 * @brief 2024 电赛 H 题第一问主程序
 * @details 小车从 A 点直行到 B 点，检测到终点黑线后停车并声光提示
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "TRACK.h"
#include "Cnotorl.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "buzzer.h"
#include "LED.h"

#define Q1_BASE_SPEED          30
#define Q1_START_IGNORE_TICKS  40

uint8_t TRACK1;
uint8_t TRACK2;
uint8_t TRACK3;
uint8_t TRACK4;
uint8_t TRACK5;

float Pitch, Roll, Yaw;
int ack = 0;

static int NormalizeYawTo360(float yaw)
{
    int yaw_360;

    if (yaw < 0)
    {
        yaw_360 = (int)(yaw + 360.5f);
    }
    else
    {
        yaw_360 = (int)(yaw + 0.5f);
    }

    if (yaw_360 >= 360)
    {
        yaw_360 -= 360;
    }

    return yaw_360;
}

static uint8_t UpdateAttitude(int *yaw_360)
{
    if (MPU6050_DMP_Get_Data(&Pitch, &Roll, &Yaw) != 0)
    {
        ack = 999;
        return 0;
    }

    ack = 1;
    *yaw_360 = NormalizeYawTo360(Yaw);
    return 1;
}

int main(void)
{
    int target_yaw;
    int current_yaw;
    int left_speed;
    int right_speed;
    int angle_pid_out;
    uint8_t infrared_state;
    uint8_t finished;
    int ignore_line_ticks;

    Track_Init();
    LED_Init();
    buzzer_Init();
    Motor_Init();
    Encoder_Init();
    OLED_Init();
    MPU6050_Init();
    MPU6050_DMP_Init();

    Set_Speed(0, 0);
    buzzer_off();
    LED1_ON();
    Delay_ms(300);

    while (UpdateAttitude(&target_yaw) == 0)
    {
        Set_Speed(0, 0);
        Delay_ms(10);
    }

    finished = 0;
    ignore_line_ticks = Q1_START_IGNORE_TICKS;

    while (1)
    {
        if (UpdateAttitude(&current_yaw) == 0)
        {
            Set_Speed(0, 0);
            Delay_ms(10);
            continue;
        }

        infrared_state = Get_Infrared_State();

        OLED_ShowString(1, 1, "Yaw:");
        OLED_ShowSignedNum(1, 5, (int)Yaw, 4);
        OLED_ShowString(2, 1, "Tar:");
        OLED_ShowNum(2, 5, target_yaw, 3);
        OLED_ShowString(3, 1, "Ack:");
        OLED_ShowSignedNum(3, 5, ack, 3);
        OLED_ShowString(4, 1, "IR:");
        OLED_ShowBinNum(4, 4, infrared_state, 8);
        OLED_ShowString(5, 1, "L:");
        OLED_ShowSignedNum(5, 3, Get_Motor_Speed_Left(), 4);
        OLED_ShowString(5, 9, "R:");
        OLED_ShowSignedNum(5, 11, Get_Motor_Speed_Right(), 4);

        if (finished)
        {
            Set_Speed(0, 0);
            Delay_ms(10);
            continue;
        }

        if (ignore_line_ticks > 0)
        {
            ignore_line_ticks--;
        }
        else if (infrared_state != 0)
        {
            Set_Speed(0, 0);
            finished = 1;
            LED1_Turn();
            buzzer_turn();
            Delay_ms(10);
            continue;
        }

        angle_pid_out = pid_angle2(target_yaw, current_yaw);
        left_speed = Q1_BASE_SPEED - angle_pid_out;
        right_speed = Q1_BASE_SPEED + angle_pid_out;
        Limit(&left_speed, &right_speed);
        Set_Speed(left_speed, right_speed);

        Delay_ms(10);
    }
}
