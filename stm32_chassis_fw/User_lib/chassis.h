#ifndef CHASSIS_H_20200211
#define CHASSIS_H_20200211

#include <stdint.h>

#include "vesc.h"
typedef enum 
{
	L2_Rx_Stop,	
	L1_Rx_Manual,
	L3_Rx_Serial,
	L1_R2_Servo
}WorkState_e;

typedef struct {
	struct{
		float x;
		float y;
		float z;
	}							linear;
	struct{
		float x;
		float y;
		float z;
	}							angular;
}Twist_t;

typedef struct{
	uint8_t error_code;
	uint8_t controller_id;
		
	float pos_axis;
	float rpm_axis;
	float torque;
	
	float current_motor;
	float voltage_duty;
	
	float rate_gear;
#ifdef wtrcfg_MOTORINFO_EXT
	int32_t tachometer;
	int32_t tachometer_abs;
	float current_input;
	float voltage_input;
	
	float id;
	float iq;
	float temp_fet;
	float temp_motor;
	float A_hours_draw;
	float A_hours_charge;
	float Watt_hours_draw;
	float Watt_hours_charge;
#endif
	
}MotorInfo_t;


typedef struct{
	float KP;
	float KI;
	float KD;
	float ref;
	float fdb;
	float error[3];
	int outputMax;
	uint16_t output;
}PID_t;



typedef struct{
	MotorInfo_t info;
	PID_t ctrl[3];
}Motor_t;

typedef struct{
	Motor_t turn;
	Motor_t roll;
}VectorWheel_t;

typedef struct
{
	Twist_t target;
	VectorWheel_t wheel[4];
	WorkState_e workstate;
}VectorChassis_t;

extern VectorChassis_t chassis;
extern VESC_t vesc[4];

void chassis_init(void);
void VectorCommand(Twist_t twist);

#endif

