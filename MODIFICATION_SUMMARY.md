# 硬件配置修改总结

## 修改概述
本次修改将8路灰度传感器改为5路灰度传感器，并实现完整的电机速度显示功能。

## 详细修改清单

### 1. 灰度传感器相关修改
**文件: TRACK.h, TRACK.c**
- 灰度传感器从8路改为5路
- 新引脚配置：
  - out1: PA2
  - out2: PA3
  - out3: PA4
  - out4: PB4
  - out5: PB5

### 2. 控制算法修改
**文件: Cnotorl.c, Cnotorl.h**
- 更新灰度状态映射表（从8位改为5位）
- 添加编码器初始化函数 `Encoder_Init()`
- 添加电机速度读取函数：
  - `Get_Motor_Speed_Left()` - 获取左电机速度
  - `Get_Motor_Speed_Right()` - 获取右电机速度

### 3. 电机驱动配置
**文件: Motor.c, PWM.c, PWM.h**
- 更新PWM配置：
  - 使用TIM1代替TIM2
  - PWMA: PA8 (TIM1_CH1)
  - PWMB: PA9 (TIM1_CH2)
- 电机方向控制（通过AIN/BIN引脚）：
  - 电机A (左电机): PB0(AIN1), PB1(AIN2)
  - 电机B (右电机): PB10(BIN1), PB11(BIN2)

### 4. OLED显示配置
**文件: OLED.c**
- I2C接口引脚：
  - SCL: PB6
  - SDA: PB7
- 添加电机速度显示到OLED第5行

### 5. 编码器配置
**文件: Cnotorl.c**
- TIM3配置为编码器模式：
  - E1A: PA6 (TIM3_CH1)
  - E1B: PA7 (TIM3_CH2)

### 6. 其他外设引脚更新
**文件: LED.c, buzzer.c, Key.c, MPU6050_I2C.h**
- LED: PA11
- 蜂鸣器: PA10  
- 按键: PB2, PB3
- MPU6050 I2C (与OLED共享): SCL=PB6, SDA=PB7

### 7. Main.c 修改
**文件: main.c**
- 删除TRACK6-8的声明
- 添加 `Encoder_Init()` 初始化调用
- 在OLED显示部分添加电机速度显示：
  ```
  L: [左电机速度]
  R: [右电机速度]
  ```

## 编译注意事项
1. 确保所有STM32标准库函数都正确使用
2. 检查TIM1的主输出控制（BDTR寄存器）已正确使能
3. 验证I2C引脚冲突已解决（OLED和MPU6050共享PB6/PB7）

## 功能验证清单
- [ ] 5路灰度传感器正常读取
- [ ] PWM输出正常（PA8, PA9）
- [ ] 电机方向控制正常
- [ ] 编码器计数正确
- [ ] OLED显示电机速度
- [ ] 按键功能正常
- [ ] LED和蜂鸣器工作正常

## 已知限制
1. 由于TIM1用于PWM输出，不能同时用作编码器
2. TIM3用于电机1的编码器读取
3. 电机2的速度通过TIM1的CCR寄存器值监测（不是真正的编码器）
