/**
 * @file main.c
 * @brief 主控制程序 - 测试版
 * @details 只保留case 17用于电机测试
 */

#include "stm32f10x.h"              
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "Key.h"
#include "TRACK.h"
#include "Cnotorl.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "buzzer.h"
#include "LED.h"
#include "uart.h"

uint8_t TRACK1;
uint8_t TRACK2;
uint8_t TRACK3;
uint8_t TRACK4;
uint8_t TRACK5;

int pid_out;
float find_err;
int pid_angle2(int target, int yaw);

float target_angle = 0;
float Pitch, Roll, Yaw;
int16_t ax, ay, az, gx, gy, gz;

u8 MPU_Get_Gyroscope(short *gx, short *gy, short *gz);
u8 MPU_Get_Accelerometer(short *ax, short *ay, short *az);
int left = 0, right = 0, aleft = 0, aright = 0;
float Yaw0_360 = 0;
int ack = 0;
uint8_t all_state = 0;
int step = 1;    
int count = 0;

int main(void)
{    
    Track_Init();
    LED_Init();
    buzzer_Init();
    Motor_Init();
    Key_Init();
    Encoder_Init();
    
    OLED_Init();
    MPU6050_Init();
    MPU6050_DMP_Init();    
    
    while (1)
    {    
        buzzer_off();
        LED1_ON();
        while(MPU6050_DMP_Get_Data(&Pitch, &Roll, &Yaw) != 0) {ack = 999;};
        ack = 1;
        Delay_ms(10);    
        OLED_ShowSignedNum(1, 1, Yaw, 5);
        OLED_ShowHexNum(2, 1, all_state, 4);
        OLED_ShowSignedNum(3, 1, step, 3);
        OLED_ShowBinNum(4, 1, Get_Infrared_State(), 8);
        
        /* 显示电机速度 */
        OLED_ShowString(5, 1, "L:");
        OLED_ShowSignedNum(5, 4, Get_Motor_Speed_Left(), 4);
        OLED_ShowString(5, 9, "R:");
        OLED_ShowSignedNum(5, 12, Get_Motor_Speed_Right(), 4);        
        
        /* 直接进入case 17测试，无需按键 */
        all_state = 0x11;

        delay_ms(10);
        
        switch(all_state)
        {
            case 17:
                delay_ms(200);
                while (Get_Infrared_State() == 0)
                {
                    while (MPU6050_DMP_Get_Data(&Pitch, &Roll, &Yaw) != 0) { ack = 999; };
                    ack = 1;

                    float Yaw0_360;
                    if (Yaw < 0)
                        Yaw0_360 = Yaw + 360;
                    else
                        Yaw0_360 = Yaw;
                    int yaw_int = (int)(Yaw0_360 + 0.5f);

                    /* 计算角度PID输出 */
                    int pid_out = pid_angle2(0, yaw_int);

                    /* 计算左右电机速度 */
                    int aleft = 30 - pid_out;
                    int aright = 30 + pid_out;

                    /* 限制电机速度 */
                    Limit(&aleft, &aright);
                    Set_Speed(aleft, aright);
                    OLED_ShowSignedNum(1, 1, Yaw, 5);
                }                
                Set_Speed(0, 0);
                LED1_Turn();
                buzzer_turn();
                break;
                
            /* 其他case已注释 */
            /*
            case 18:
                ... (所有其他状态已注释)
            */
        }
    }
    
    return 0;
}
