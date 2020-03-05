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
	
#include "CanCommute.h"
#define ROTER_RANGE	8192

/** 
* @brief 将计算得到的电机新参数发送给电机
* @param None
* @retval None
* @TODO None
*/
void SendMotor()
{

		CanTransmit_1234(&hcan1, 
								 Chassis.Motor[0].output,
								 Chassis.Motor[1].output,
								 Chassis.Motor[2].output,
								 Chassis.Motor[3].output);
	
}

/**
* @brief ID为1~4的电机信号发送函数
* @param ID为1~4的各个电机的电流数值
* @retval None
*/
void CanTransmit_1234(CAN_HandleTypeDef *hcanx, int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq)
{
	CAN_TxHeaderTypeDef TxMessage;
		
	TxMessage.DLC=0x08;
	TxMessage.StdId=0x200;
	TxMessage.IDE=CAN_ID_STD;
	TxMessage.RTR=CAN_RTR_DATA;

	uint8_t TxData[8];
	TxData[0] = (uint8_t)(cm1_iq >> 8);
	TxData[1] = (uint8_t)cm1_iq;
	TxData[2] = (uint8_t)(cm2_iq >> 8);
	TxData[3] = (uint8_t)cm2_iq;
	TxData[4] = (uint8_t)(cm3_iq >> 8);
	TxData[5] = (uint8_t)cm3_iq;
	TxData[6] = (uint8_t)(cm4_iq >> 8);
	TxData[7] = (uint8_t)cm4_iq; 

	if(HAL_CAN_AddTxMessage(hcanx,&TxMessage,TxData,(uint32_t*)CAN_TX_MAILBOX0)!=HAL_OK)
	{
		 Error_Handler();       //如果CAN信息发送失败则进入死循环
	}
}

	
	/**
* @brief 根据电机信息的ID号进行对应的数据解析
* @param 电机ID号
* @retval None
*/
void CanDataReceive(int motor_index)
{
	
	switch(motor_index)
	{
		case CAN_CHASSIS_MOTOR1_ID:{
			if(Chassis.Motor[0].msg_cnt++ < 50 ){
				get_rotor_offset(&Chassis.Motor[0]);
			}
			else{
			CanDataDecoder(&Chassis.Motor[0]);    //电机数据具体解析函数
			}
			break;
		}
		case CAN_CHASSIS_MOTOR2_ID:{
			if(Chassis.Motor[1].msg_cnt++ < 50 ){
				get_rotor_offset(&Chassis.Motor[1]);
			}
			else{
			CanDataDecoder(&Chassis.Motor[1]);    //电机数据具体解析函数
			}
			break;
		}
		case CAN_CHASSIS_MOTOR3_ID:{
			if(Chassis.Motor[2].msg_cnt++ < 50 ){
				get_rotor_offset(&Chassis.Motor[2]);
			}
			else{
			CanDataDecoder(&Chassis.Motor[2]);    //电机数据具体解析函数
			}
			break;
		}
		case CAN_CHASSIS_MOTOR4_ID:{
			if(Chassis.Motor[3].msg_cnt++ < 50 ){
				get_rotor_offset(&Chassis.Motor[3]);
			}
			else{
			CanDataDecoder(&Chassis.Motor[3]);    //电机数据具体解析函数
			}
			break;
		}
		default:;
	}
}

/**
* @brief CAN通信电机的反馈数据具体解析函数
* @param 电机数据结构体
* @retval None
*/
int delta_sum = 0,omega;
void CanDataDecoder(PID_t *motor)
{
	//更新位置
	motor->fdbPos[LAST] = motor->fdbPos[NOW];
	motor->fdbPos[NOW] = CanReceiveData[0]<<8|CanReceiveData[1];
	//更新速度
	motor->fdb = CanReceiveData[2]<<8|CanReceiveData[3];
	motor->fdb_Vol = CanReceiveData[2]<<8|CanReceiveData[3];
	//更新总转数
	motor->real_pos[LAST] = motor->real_pos[NOW];

	/* 将电机速度反馈值由无符号整型转变为有符号整型 */
	if(motor->fdb > 32768)
	{
		motor->fdb -= 65536;
	}
	 /* 电机圈数更新 */
	if(motor->fdbPos[NOW] - motor->fdbPos[LAST] > ROTER_RANGE / 2)
	{
		motor->round --;
	}
	else if(motor -> fdbPos[NOW] - motor->fdbPos[LAST] < -(ROTER_RANGE/2))
	{
		motor->round ++;
	}
	motor->round_wheel = (float)(motor->round * 187.0f) / 3591;
	motor->real_pos[NOW] = motor->fdbPos[NOW] + ROTER_RANGE * motor->round - motor->offsetPos;
	
	motor ->real_angle = (float)(motor->real_pos[NOW]) * 2.288f*0.001f;// 1/8192*360*187/3519
	
	int delta = motor->real_pos[NOW] - motor->real_pos[LAST];
	
	motor->DeltaPosbuf[motor->PosIndex++]=delta;
	
	if(motor->PosIndex == 30){
		delta_sum = 0;
		motor->PosIndex = 0;
		for(int i=0;i<28;i++){
			delta_sum += motor->DeltaPosbuf[i];
		}
		omega = delta_sum		* 0.081729;				// sum/28times/1ms/8192/ * 360 *187 / 3591
	}
	motor->real_w =omega;
	
}

void get_rotor_offset(PID_t *ptr)
{
	ptr->real_pos[NOW] = (uint16_t)CanReceiveData[0]<<8|CanReceiveData[1];
	ptr->offsetPos= ptr->real_pos[NOW];
}

	
	
	
/**
* @brief CAN外设过滤器初始化
* @param can结构体
* @retval None
*/
HAL_StatusTypeDef CanFilterInit(CAN_HandleTypeDef* hcan)
{
  CAN_FilterTypeDef  sFilterConfig;

  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;
  
	if(hcan == &hcan1)
	{
		sFilterConfig.FilterBank = 0;
	}

	
  if(HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_Start(hcan) != HAL_OK)
  {
    Error_Handler();
  }
	
  if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    Error_Handler();
  }

	return HAL_OK;
}

