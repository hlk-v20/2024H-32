/**
 * @file OLED.h
 * @brief OLED显示屏模块头文件
 * @details
 *   - 功能：0.96寸OLED屏的初始化和显示控制
 *   - 通信接口：I2C（PB6-SCL, PB7-SDA）
 *   - 屏幕分辨率：128×64
 *   - 显示内容：文字、数字、十六进制、二进制等
 */

#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"

/* 函数声明 */
void OLED_Init(void);                                                    /*!< OLED初始化 */
void OLED_Clear(void);                                                   /*!< OLED清屏 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);             /*!< 显示单个字符 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);        /*!< 显示字符串 */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);       /*!< 显示无符号整数 */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);  /*!< 显示有符号整数 */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);    /*!< 显示十六进制数 */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);    /*!< 显示二进制数 */

#endif
