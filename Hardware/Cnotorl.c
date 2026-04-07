#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "Timer.h"
#include "TRACK.h"

float error;

#define Kp    2.5
#define Kd    3.5
  
u8 last_infrared_state = 0;	
	
float Track_err(void)
{
	u8 state =  Get_Infrared_State();
	// �Ż�������������2��״̬һ�²Ÿ��������ȷ����ֱ�������ͺ�
	static u8 same_state_cnt = 0; // ������������ͬ״̬������
	
	if (state == last_infrared_state)
	{
	    same_state_cnt++; // ״̬һ�£���������1
	}
	else
	{
	    same_state_cnt = 0; // ״̬��һ�£�����������
	    last_infrared_state = state;
	}
	
	// ��������2��״̬һ��ʱ���Ÿ���error�����򷵻���һ�����
	if (same_state_cnt < 2)
	{
	    return error;
	}
	
	switch(state)
	{  
		case 0:     //0000 0000   
		error= 0 ;	  break;
		case 16:    //0001 0000   
		error= 1 ;  break;
		case 8:     //0000 1000   
		error= 1 ; break;	
		case 24:    //0001 1000   
		error= 0 ;	  break;		
		case 60:    //0011 1100   
		error= 0 ;	  break;		
		case 126:   //0111 1110   
		error= 0 ;	  break;
		
		case 48:     //0011 0000   //С����ƫ��errΪ��
		error= 2  ;	  break;
		case 32:     //0010 0000   
		error= 2 ;	  break;
		case 64:     //0100 0000   
		error= 4 ;	  break;
		case 96:     //0110 0000   
		error= 4 ;	  break;
		case 128:    //1000 0000   
		error= 6 ;	  break;	
		case 192:    //1100 0000   
		error= 6 ;	  break;
		case 224:   //1110 0000   
		error= 6 ;	  break;
		case 160:   //1010 0000   
		error= 6 ;	  break;
		case 254:   //1111 1110   
		error= 6 ;	  break;
		case 252:   //1111 1100   
		error= 6 ;	  break;
		case 248:   //1111 1000   
		error= 6 ;	  break;
		case 240:   //1111 0000   
		error= 6 ;	  break;
		case 124:   //0111 1100   
		error= 4 ;	  break;
		case 120:   //0111 1000   
		error= 4 ;	  break;
		case 56:    //0011 1000   
		error= 2 ;	break;
		
		
		

		case 12:   //0000 1100   //С����ƫ��errΪ��
		error= -2 ;	  break;	
    case 14:   //0000 1110   //С����ƫ��errΪ��
		error= -4 ;	  break;	
    case 30:   //0001 1110   //С����ƫ��errΪ��
		error= -4 ;	  break;
    case 62:   //0011 1110   //С����ƫ��errΪ��
		error= -4 ;	  break;
		case 31:   //0001 1111   
		error= -6 ;	  break;
		case 63:   //0011 1111   
		error= -6 ;	  break;
		case 125:  //0111 1111   
		error= -6 ;	  break;
		case 4:    //0000 0100   
		error= -2 ;	  break;	
    case 28:   //0001 1100   
		error= -2 ;	  break;		
		case 2:    //0000 0010   
		error= -4 ;	  break;
		case 6:    //0000 0110   
		error= -4 ;	  break;
		case 1:    //0000 0001   
		error= -6 ;	  break;
		case 3:    //0000 0011   
		error= -6 ;	  break;
		case 7:    //0000 0111   
		error= -6 ;	  break;
		case 15:   //0000 1111   
		error= -6 ;	  break;
			
		default: 
		// �Ż���1��Ĭ��״̬������errorΪ0�����õ�ǰֵ��������Ч����
		break;
	}
	last_infrared_state = state; 
	same_state_cnt = 0; // ���ü�������Ϊ��һ�η�����׼��
	return error;
}


int PID_out(float error, int Target)      //λ�û�
{
    static int last_err = 0;  // ����ԭ�о�̬������������ȫ�ֱ���
    
    int err = (int)error;
    int out;
    
    // �Ż���3������΢�����ʼ���䣨�ǶȻ��л����״�ѭ�����������ͻ�䣩
    int diff = err - last_err;
    if (last_err == 0 && err != 0) {
        diff = err / 2;  // ΢������룬���ٳ�ʼ���
    }
    
    // ����ԭ��PD�����߼������滻΢����Ϊ�Ż����diff
    out = Kp * err + Kd * diff;
    
    // �Ż���4�����Ӽ�������޷������ⳬ�������Ч��Χ
    if (out > 25) out = 25;    // ���޴�20������25���ͷ�����ռ�
    if (out < -25) out = -25;  // ���޶Գ�����������ת��ƽ��
    
    last_err = err;  // ����ԭ����ʷ������
    return out;
}


void Final_Speed(int pid_out ,int base_speed)
{
    int left_speed = base_speed - pid_out;
    int right_speed = base_speed + pid_out;
    
    // �����ɣ����Ƶ���ٶȱ仯���ȣ����ⱬ��
    #define MIN_SPEED 5
    #define MAX_SPEED 40
    
    // �����޷�
    left_speed = left_speed < MIN_SPEED ? MIN_SPEED : left_speed;
    left_speed = left_speed > MAX_SPEED ? MAX_SPEED : left_speed;
    // �ҵ���޷�
    right_speed = right_speed < MIN_SPEED ? MIN_SPEED : right_speed;
    right_speed = right_speed > MAX_SPEED ? MAX_SPEED : right_speed;
    
    Set_Speed(left_speed, right_speed);
}


// ����ԭ��PD�������壨�ɸ��ݵ���΢����
#define  ka2p   0.5
#define  ka2d   3.5

int pid_angle2(int target, int yaw)     //�ǶȻ�
{
    static int err_last = 0;  // ��ʼ����ʷ����̬��������¼��һ�ε���Ч��
    int pid_a_out, err;
  
    // ��һ������ԭʼyaw����180�㣩ת��Ϊ0~360�㣬ͳһ�Ƕ�����ϵ
    int yaw0_360 = (yaw < 0) ? (yaw + 360) : yaw;
    int target_360 = target;  // Ŀ��Ƕ�תΪ0~360�㣨����target��Ϊ0~360����180�㣬�˴����ݣ�

    // �ڶ������ع��������߼�
    // �����޸ģ����㡸���·����������ǿ����ʱ������
    // ��Χ�޶���[-180, 180]���������358�����೬�����Ӹ�Դ�Ϸ�ֹתȦ
    err = target_360 - yaw0_360;
    // ��������180�㣬ȡ�������߸���·����������ʱ�볬����
    if (err > 180) {
        err -= 360;
    }
    // �����С��-180�㣬ͬ��ȡ�������������·����
    else if (err < -180) {
        err += 360;
    }

    // ���������Ż�΢������㣬�����״�������ۻ����
    int diff = err - err_last;
    // �״ε���ʱ��err_last=0��err��0����΢�������
    if (err_last == 0 && err != 0) {
        diff = err / 2;
    }

    // ���Ĳ���PD���㣨�޻��������������PD�����
    // ֱ��ͨ��������+΢����õ����������޶���ջ�����
    pid_a_out = (int)(ka2p * err + ka2d * diff);  // ǿ��ת���ͣ�ƥ������������

    // ���岽������PID������ֵ�����ⳬ������������ʧ��תȦ
    // ����������£��޷��Ƿ�ֹתȦ�Ĺؼ�������������������Խ��Ŀ����޷�������
    #define PID_A_OUT_MAX 20  // �ɸ���ʵ�ʵ��Ե���������15~25��
    if (pid_a_out > PID_A_OUT_MAX) {
        pid_a_out = PID_A_OUT_MAX;
    }
    if (pid_a_out < -PID_A_OUT_MAX) {
        pid_a_out = -PID_A_OUT_MAX;
    }

    // ��������������ʷ��Ϊ��һ�μ�����׼���������ã�����Ȼ�ۻ���
    err_last = err;

    // ����PD���������������ջ�������PD�������޷�ʵ����Ȼ����
    return pid_a_out;
}

// 编码器初始化：TIM3用于电机1编码器（PA6-E1A, PA7-E1B）
void Encoder_Init(void)
{
	// GPIO初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // PA6, PA7用于TIM3
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// TIM3初始化 - 电机1编码器
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_Period = 65535;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	// 配置TIM3为编码器模式（PA6-TIM3_CH1, PA7-TIM3_CH2）
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(TIM3, ENABLE);
}

// 获取左电机（电机A）的速度
int Get_Motor_Speed_Left(void)
{
	// 电机A使用TIM1_CH1 (PA8)
	static uint16_t last_count = 0;
	uint16_t current_count;
	int speed;
	
	current_count = TIM_GetCapture1(TIM1);
	speed = (int)(current_count - last_count);
	last_count = current_count;
	
	return speed;
}

// 获取右电机（电机B）的速度
int Get_Motor_Speed_Right(void)
{
	// 电机B使用TIM1_CH2 (PA9)
	uint16_t current_count;
	static uint16_t last_count = 0;
	int speed;
	
	current_count = TIM_GetCapture2(TIM1);
	speed = (int)(current_count - last_count);
	last_count = current_count;
	
	return speed;
}


