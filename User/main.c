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

#define Q1_BASE_SPEED           30
#define Q1_START_IGNORE_TICKS   40
#define Q1_LEFT_MOTOR_SCALE    130
#define Q1_RIGHT_MOTOR_SCALE   100
#define Q1_LEFT_MOTOR_BIAS       3
#define Q1_RIGHT_MOTOR_BIAS      0
#define Q1_SLEW_STEP             1
#define Q1_ANGLE_KP           0.18f
#define Q1_ANGLE_KI           0.00f
#define Q1_ANGLE_KD           0.30f
#define Q1_ANGLE_PID_MAX        12
#define Q1_ANGLE_DEADBAND        2
#define Q1_ANGLE_DECAY        0.70f
#define Q1_ERR_FILTER_A       0.70f

uint8_t TRACK1;
uint8_t TRACK2;
uint8_t TRACK3;
uint8_t TRACK4;
uint8_t TRACK5;

float Pitch, Roll, Yaw;
int ack = 0;
static int g_left_cmd = 0;
static int g_right_cmd = 0;
static int g_angle_err_k_1 = 0;
static int g_angle_err_k_2 = 0;
static float g_angle_pid_out = 0;
static float g_angle_err_filtered = 0;

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

static int StepToward(int current, int target, int step)
{
    if (current < target - step)
    {
        return current + step;
    }

    if (current > target + step)
    {
        return current - step;
    }

    return target;
}

static int ScaleMotorCommand(int speed, int scale_percent)
{
    return (speed * scale_percent) / 100;
}

static void ResetAnglePid(void)
{
    g_angle_err_k_1 = 0;
    g_angle_err_k_2 = 0;
    g_angle_pid_out = 0;
    g_angle_err_filtered = 0;
}

static int AnglePidIncremental(int target, int yaw)
{
    int err;
    int err_now;
    float delta_output;

    err = target - yaw;
    if (err > 180)
    {
        err -= 360;
    }
    else if (err < -180)
    {
        err += 360;
    }

    g_angle_err_filtered = g_angle_err_filtered * Q1_ERR_FILTER_A
                         + err * (1.0f - Q1_ERR_FILTER_A);
    err_now = (int)g_angle_err_filtered;

    if ((err_now < Q1_ANGLE_DEADBAND) && (err_now > -Q1_ANGLE_DEADBAND))
    {
        err_now = 0;
    }

    delta_output = Q1_ANGLE_KP * (err_now - g_angle_err_k_1)
                 + Q1_ANGLE_KI * err_now
                 + Q1_ANGLE_KD * (err_now - 2 * g_angle_err_k_1 + g_angle_err_k_2);
    g_angle_pid_out += delta_output;

    if (err_now == 0)
    {
        g_angle_pid_out *= Q1_ANGLE_DECAY;
    }

    if (g_angle_pid_out > Q1_ANGLE_PID_MAX)
    {
        g_angle_pid_out = Q1_ANGLE_PID_MAX;
    }
    else if (g_angle_pid_out < -Q1_ANGLE_PID_MAX)
    {
        g_angle_pid_out = -Q1_ANGLE_PID_MAX;
    }

    g_angle_err_k_2 = g_angle_err_k_1;
    g_angle_err_k_1 = err_now;

    return (int)g_angle_pid_out;
}

static void ApplyDrive(int left_speed, int right_speed)
{
    left_speed = ScaleMotorCommand(left_speed, Q1_LEFT_MOTOR_SCALE);
    right_speed = ScaleMotorCommand(right_speed, Q1_RIGHT_MOTOR_SCALE);

    if (left_speed > 0)
    {
        left_speed += Q1_LEFT_MOTOR_BIAS;
    }
    else if (left_speed < 0)
    {
        left_speed -= Q1_LEFT_MOTOR_BIAS;
    }

    if (right_speed > 0)
    {
        right_speed += Q1_RIGHT_MOTOR_BIAS;
    }
    else if (right_speed < 0)
    {
        right_speed -= Q1_RIGHT_MOTOR_BIAS;
    }

    Limit(&left_speed, &right_speed);

    g_left_cmd = StepToward(g_left_cmd, left_speed, Q1_SLEW_STEP);
    g_right_cmd = StepToward(g_right_cmd, right_speed, Q1_SLEW_STEP);

    Set_Speed(g_left_cmd, g_right_cmd);
}

static void StopDrive(void)
{
    g_left_cmd = 0;
    g_right_cmd = 0;
    Set_Speed(0, 0);
}

static void ParkingAlert(void)
{
    uint8_t i;

    for (i = 0; i < 3; i++)
    {
        LED1_ON();
        buzzer_on();
        Delay_ms(150);
        LED1_OFF();
        buzzer_off();
        Delay_ms(150);
    }

    LED1_ON();
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

    StopDrive();
    buzzer_off();
    LED1_OFF();
    Delay_ms(300);

    while (UpdateAttitude(&target_yaw) == 0)
    {
        Set_Speed(0, 0);
        Delay_ms(10);
    }

    ResetAnglePid();
    finished = 0;
    ignore_line_ticks = Q1_START_IGNORE_TICKS;

    while (1)
    {
        if (UpdateAttitude(&current_yaw) == 0)
        {
            Delay_ms(10);
            continue;
        }

        infrared_state = Get_Infrared_State();

        OLED_ShowString(1, 1, "Yaw");
        OLED_ShowSignedNum(1, 4, (int)Yaw, 4);
        OLED_ShowString(1, 10, "T");
        OLED_ShowNum(1, 12, target_yaw, 3);

        OLED_ShowString(2, 1, "IR");
        OLED_ShowBinNum(2, 4, infrared_state, 8);

        OLED_ShowString(3, 1, "L");
        OLED_ShowSignedNum(3, 3, g_left_cmd, 4);
        OLED_ShowString(3, 10, "R");
        OLED_ShowSignedNum(3, 12, g_right_cmd, 4);

        OLED_ShowString(4, 1, "Ack");
        OLED_ShowSignedNum(4, 5, ack, 3);

        if (finished)
        {
            ResetAnglePid();
            StopDrive();
            Delay_ms(10);
            continue;
        }

        if (ignore_line_ticks > 0)
        {
            ignore_line_ticks--;
        }
        else if (infrared_state != 0)
        {
            ResetAnglePid();
            StopDrive();
            finished = 1;
            ParkingAlert();
            Delay_ms(10);
            continue;
        }

        angle_pid_out = AnglePidIncremental(target_yaw, current_yaw);
        left_speed = Q1_BASE_SPEED - angle_pid_out;
        right_speed = Q1_BASE_SPEED + angle_pid_out;
        ApplyDrive(left_speed, right_speed);

        Delay_ms(10);
    }
}
