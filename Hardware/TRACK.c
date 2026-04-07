/**
 * @file TRACK.c
 * @brief 灰度传感器模块
 * @details
 *   - 功能：5路灰度传感器初始化和状态读取
 *   - 灰度传感器：out1~out5分别接PA2, PA3, PA4, PB4, PB5
 *   - 工作方式：GPIO上拉输入，读取传感器输出信号
 */

#include "stm32f10x.h"
#include "TRACK.h"
#include "Motor.h"
#include "stdio.h"
#include "PWM.h"

/* 外部声明：灰度传感器各路状态 */
extern uint8_t TRACK1;
extern uint8_t TRACK2;
extern uint8_t TRACK3;
extern uint8_t TRACK4;
extern uint8_t TRACK5;

/**
 * @brief 灰度传感器初始化
 * @details
 *   初始化5路灰度传感器所在的GPIO引脚为上拉输入模式
 *   - 传感器1：PA2（TRACK_INFRARED_PIN_1）
 *   - 传感器2：PA3（TRACK_INFRARED_PIN_2）
 *   - 传感器3：PA4（TRACK_INFRARED_PIN_3）
 *   - 传感器4：PB4（TRACK_INFRARED_PIN_4）
 *   - 传感器5：PB5（TRACK_INFRARED_PIN_5）
 */
void Track_Init(void)
{
    /* A端的初始化 - out1,out2,out3接PA2,PA3,PA4 */
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(TRACK_PORT_CLKA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = TRACK_INFRARED_PIN_1 | TRACK_INFRARED_PIN_2 | TRACK_INFRARED_PIN_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(TRACK_PORTA, &GPIO_InitStructure);

    /* B端的初始化 - out4,out5接PB4,PB5 */
    RCC_APB2PeriphClockCmd(TRACK_PORT_CLKB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = TRACK_INFRARED_PIN_4 | TRACK_INFRARED_PIN_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(TRACK_PORTB, &GPIO_InitStructure);
}

/**
 * @brief 读取灰度传感器状态
 * @details
 *   逐一读取5路灰度传感器的输入信号，打包成一个8位的状态值
 *   - 位4：传感器1状态（PA2）
 *   - 位3：传感器2状态（PA3）
 *   - 位2：传感器3状态（PA4）
 *   - 位1：传感器4状态（PB4）
 *   - 位0：传感器5状态（PB5）
 * @return 5路传感器状态打包值 (0x00~0x1F)
 *   - 1：扫描到灰度线
 *   - 0：没有扫描到灰度线
 */
u8 Get_Infrared_State(void)
{
    TRACK1 = GPIO_ReadInputDataBit(TRACK_PORTA, TRACK_INFRARED_PIN_1) << 4;
    TRACK2 = GPIO_ReadInputDataBit(TRACK_PORTA, TRACK_INFRARED_PIN_2) << 3;
    TRACK3 = GPIO_ReadInputDataBit(TRACK_PORTA, TRACK_INFRARED_PIN_3) << 2;
    TRACK4 = GPIO_ReadInputDataBit(TRACK_PORTB, TRACK_INFRARED_PIN_4) << 1;
    TRACK5 = GPIO_ReadInputDataBit(TRACK_PORTB, TRACK_INFRARED_PIN_5) << 0;

    u8 state = 0;
    state = (u8)(TRACK1 | TRACK2 | TRACK3 | TRACK4 | TRACK5);
    return state;
}
