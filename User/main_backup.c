/**
 * @file main.c
 * @brief 锟斤拷锟斤拷锟斤拷锟斤拷锟?
 * @details
 *   - 锟斤拷始锟斤拷系统锟斤拷锟借：锟揭度达拷锟叫★拷OLED锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷LED锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟組PU6050
 *   - 锟斤拷取锟斤拷锟斤拷锟斤拷锟斤拷锟捷诧拷锟斤拷示锟斤拷OLED
 *   - 通锟斤拷锟斤拷锟斤拷锟叫伙拷状态锟斤拷锟斤拷执锟斤拷路锟斤拷锟斤拷锟劫和角度匡拷锟斤拷
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
int pid_angle2(int target,int yaw);

float target_angle=0;
float Pitch,Roll,Yaw;    // 锟斤拷态锟角ｏ拷默锟斤拷值锟斤拷锟姐，锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟角硷拷锟斤拷
int16_t ax,ay,az,gx,gy,gz;    // 原始锟斤拷锟劫度和斤拷锟劫讹拷锟斤拷锟斤拷

float Pitch,Roll,Yaw;                                //??????????????????????????????
int16_t ax,ay,az,gx,gy,gz;                            //????????????????

u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz);
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az);
int left=0,right=0  ,aleft=0,aright=0;
float Yaw0_360=0;
int ack=0;
uint8_t all_state=0;
int step=1;    
int count=0;

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
        LED1_ON ();
        while(MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw)!=0){ack=999;};
        ack=1;
        Delay_ms (10);    
        OLED_ShowSignedNum(1,1,Yaw,5);
        OLED_ShowHexNum(2,1,all_state,4);
        OLED_ShowSignedNum(3,1,step,3);
        OLED_ShowBinNum(4,1,Get_Infrared_State(),8);
        
        // 锟斤拷示锟斤拷锟斤拷俣锟?
        OLED_ShowString(5, 1, "L:");
        OLED_ShowSignedNum(5, 4, Get_Motor_Speed_Left(), 4);
        OLED_ShowString(5, 9, "R:");
        OLED_ShowSignedNum(5, 12, Get_Motor_Speed_Right(), 4);        
        switch (Key_GetNum())
        {
            case 1:
                // 锟斤拷取锟斤拷锟?位状态锟斤拷锟斤拷锟斤拷实锟斤拷状态值
                if ((all_state & 0x000f) == 0x0000)
                    all_state = 0x0001;  // 状态锟斤拷锟斤拷始状态为 1
                else if ((all_state & 0x000f) == 0x0001)
                    all_state = 0x0002;  // 锟斤拷一锟斤拷转锟斤拷锟节讹拷锟斤拷
                else if ((all_state & 0x000f) == 0x0002)
                    all_state = 0x0003;  // 锟节讹拷锟斤拷转锟斤拷锟斤拷锟斤拷锟斤拷
                else if ((all_state & 0x000f) == 0x0003)
                    all_state = 0x0004;  // 锟斤拷锟斤拷锟斤拷转锟斤拷锟斤拷锟侥讹拷
                else if ((all_state & 0x000f) == 0x0004)
                    all_state = 0x0001;  // 锟斤拷锟侥段回碉拷锟斤拷一锟轿ｏ拷循锟斤拷执锟斤拷
                break;

            case 2:
                all_state = all_state | 0x0010;  // 锟斤拷锟斤拷原始模式
                break;
        }

    delay_ms (10);
    
    switch(all_state )
    {
        
//====================锟斤拷一状态=================================================
//===============================================================================        
        case 17:
            delay_ms (200);
            while (Get_Infrared_State() == 0)
            {
                while (MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw) != 0) { ack = 999; };
                ack = 1;

                float Yaw0_360;
                if (Yaw < 0)
                    Yaw0_360 = Yaw + 360;
                else
                    Yaw0_360 = Yaw;
                int yaw_int = (int)(Yaw0_360 + 0.5f);

                // 锟斤拷锟斤拷嵌锟絇ID锟斤拷锟?
                int pid_out = pid_angle2(0, yaw_int);

                // 锟斤拷锟斤拷锟斤拷锟揭碉拷锟斤拷俣锟?
                int aleft = 30 - pid_out;
                int aright = 30 + pid_out;

                        // ???????????
                        Limit(&aleft, &aright);
                        Set_Speed(aleft, aright);
                        OLED_ShowSignedNum(1,1,Yaw,5);
            }                
            Set_Speed(0,0);
            LED1_Turn();
            buzzer_turn();

        break;
//===============================================================================            
//===============================================================================



            

        all_state = 0x11;  // 直接进入case 17测试


    delay_ms(10);

    switch(all_state)
    {
        case 17:
            delay_ms(200);
            while (Get_Infrared_State() == 0)
            {
                while (MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw) != 0) { ack = 999; };
                ack = 1;

                float Yaw0_360;
                if (Yaw < 0)
                    Yaw0_360 = Yaw + 360;
                else
                    Yaw0_360 = Yaw;
                int yaw_int = (int)(Yaw0_360 + 0.5f);

                int pid_out = pid_angle2(0, yaw_int);
                int aleft = 30 - pid_out;
                int aright = 30 + pid_out;
                Limit(&aleft, &aright);
                Set_Speed(aleft, aright);
                OLED_ShowSignedNum(1,1,Yaw,5);
            }
            Set_Speed(0,0);
            LED1_Turn();
            buzzer_turn();
            break;
    }
    }
}
