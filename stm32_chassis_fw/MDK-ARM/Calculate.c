/**
  ******************************************************************************
  * 文件名          : Calculate.c
  * 文件描述        : 机器人的各个参数计算
  * 创建时间        : 2019.0710
  * 作者            : 任云帆
  ******************************************************************************
  *								文件描述     								   *
  ******************************************************************************
  *	
	* 本文件定义了底盘的电机PID
	*
  ******************************************************************************
  * 1.本代码基于STM32F427IIH6开发，编译环境为Keil 5。
  * 2.本代码只适用于WTRobot一代全向底盘，不建议用作其他用途
  * 3.本代码以UTF-8格式编码，请勿以ANSI编码形式打开
  * 4.本代码最终解释权归哈尔滨工业大学（深圳）南工问天战队所有
  *
  * Copyright (c) 2019 哈尔滨工业大学（深圳）南工问天战队 版权所有
  ******************************************************************************
  */
#include "Calculate.h"




void Calculate()
{  	
	PID_Calc(&Chassis.Motor[0]);         //进行PID速度环计算
	PID_Calc(&Chassis.Motor[1]);
	PID_Calc(&Chassis.Motor[2]);
	PID_Calc(&Chassis.Motor[3]);
}

void PID_Calc(PID_t *pid)
{
	pid->cur_error = pid->ref - pid->fdb;
	pid->output += pid->KP * (pid->cur_error - pid->error[1])  + pid->KI * pid->cur_error + pid->KD * (pid->cur_error - 2 * pid->error[1] + pid->error[0]);
	pid->error[0] = pid->error[1]; 
	pid->error[1] = pid->ref - pid->fdb;
	/*设定输出上限*/
	if(pid->output > pid->outputMax) pid->output = pid->outputMax;
	if(pid->output < -pid->outputMax) pid->output = -pid->outputMax;
}


/**
* @brief 四轮运动算法
* @param None
* @retval None
*/
void FourWheelVellControl(double x,double y,double a)
{
	double Vx,Vy,Va,vol[4];
	double len = LEN;
	Vx = x;
	Vy = y;
	Va = a*0.001;
 
		vol[0]=(double)(sin((AFA) / 180.0f * 3.1415926f) * Vx - cos((AFA) / 180.0f * 3.1415926f)*Vy+len * Va);//计算各轮子的转速
		vol[1]=(double)(-sin((AFA) / 180.0f * 3.1415926f) * Vx - cos((AFA) / 180.0f * 3.1415926f)*Vy+len * Va);
		vol[2]=(double)(-sin((AFA) / 180.0f * 3.1415926f) * Vx + cos((AFA) / 180.0f * 3.1415926f)*Vy+len * Va);
		vol[3]=(double)(sin((AFA) / 180.0f * 3.1415926f) * Vx + cos((AFA) / 180.0f * 3.1415926f)*Vy+len * Va);

/******************************double转int******************************************/
	Chassis.Motor[0].ref = (int) vol[0];
	Chassis.Motor[1].ref = (int) vol[1];
	Chassis.Motor[2].ref = (int) vol[2];
	Chassis.Motor[3].ref = (int) vol[3];
}
