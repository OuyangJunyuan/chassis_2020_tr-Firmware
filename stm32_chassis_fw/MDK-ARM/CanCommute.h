/**
  ******************************************************************************
  * 文件名          : CANCommute.c
  * 文件描述        : 机器人CAN通讯管理模块
  * 创建时间        : 2019.0710
  * 作者            : 任云帆
  ******************************************************************************
  *								文件描述     								   *
  ******************************************************************************
  *	
	* 包含：
	*				用于与底盘3508电机通讯的CAN1
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
	
#ifndef CANCOMMUTE_H
#define CANCOMMUTE_H

#include "ChassisInit.h"
#include "can.h"
#include "string.h"
#include "main.h"

/* 定义各个电机的CAN ID号 */
#define CAN_CHASSIS_MOTOR1_ID 0x201
#define CAN_CHASSIS_MOTOR2_ID 0x202
#define CAN_CHASSIS_MOTOR3_ID 0x203
#define CAN_CHASSIS_MOTOR4_ID 0x204
#define CAN_CHASSIS_MOTOR5_ID 0x205
#define CAN_BUF_SIZE 4




void SendMotor(void);
void get_rotor_offset(PID_t *ptr);
void CanTransmit_1234(CAN_HandleTypeDef *hcanx, int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq);
void CanDataReceive(int motor_index);
void CanDataDecoder(PID_t *motor);
HAL_StatusTypeDef CanFilterInit(CAN_HandleTypeDef* hcan);
void CanDataEncoderProcess5(void);

#endif // CANCOMMUTE_H


