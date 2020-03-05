/******************************
 *@file:WTRconfig.h
 *@brief:用来定义工程常量
 ******************************
 ******************************
 *
 *
 *****************************/
#ifndef WTR_CONFIG_H
#define WTR_CONFIG_H
/* -------------------- 常量 -------------------- */
#define wtrcfg_PI 3.1415926

/* -------------------- OS -------------------- */
/* --------------- OS/任务 --------------- */
/* ---------- OS/任务/优先级 ---------- */
#include "cmsis_os.h"
#define wtrcfg_PRIORITY_EMERGENCY   osPriorityRealtime
#define wtrcfg_PRIORITY_COMMAND	    osPriorityNormal
#define wtrcfg_PRIORITY_CANREAD     osPriorityNormal
#define wtrcfg_PRIORITY_PERIOD      osPriorityNormal
#define wtrcfg_PRIORITY_SUPPORT     osPriorityLow

/* ---------- OS/任务/任务栈大小 ---------- */
#define wtrcfg_STACKDEPTH_EMERGENCY 128
#define wtrcfg_STACKDEPTH_PERIOD    128
#define wtrcfg_STACKDEPTH_CANREAD   128
#define wtrcfg_STACKDEPTH_COMMAND   128
#define wtrcfg_STACKDEPTH_SUPPORT   128

/* ---------- OS/任务/任务通知事件 ---------- */
#define wtrcfg_EVENT_BIT0_EMERGENCY (0x00000000<<0)  /* - bit0:Emergency stop button - */

/* ---------- OS/任务/任务延时间隔 ---------- */
#define wtrcfg_TIMEINCREMENT_TICK 1
/* -------------------- App -------------------- */

/* ---------------- App/底盘 ---------------- */
/* -------- App/底盘/模型/阈值 ----------- */
#define wtrcfg_CHASSIS_CONTROL_THRESHOLD_X 1
#define wtrcfg_CHASSIS_CONTROL_THRESHOLD_Y 1
#define wtrcfg_CHASSIS_CONTROL_THRESHOLD_Z 1

/* -------- App/底盘/模型/尺寸 ---------- */
#define wtrcfg_CHASSISMODEL_AFA 45.0f
#define wtrcfg_CHASSISMODEL_GEAR_RATE (79.0f/18.0f)
#define wtrcfg_CHASSIS_MECHANICAL_WHEEL_ANGLE0  ( 1 * wtrcfg_PI / 4 )
#define wtrcfg_CHASSIS_MECHANICAL_WHEEL_ANGLE1  ( 7 * wtrcfg_PI / 4 )
#define wtrcfg_CHASSIS_MECHANICAL_WHEEL_ANGLE2  ( 5 * wtrcfg_PI / 4 )
#define wtrcfg_CHASSIS_MECHANICAL_WHEEL_ANGLE3  ( 3 * wtrcfg_PI / 4 )
#define wtrcfg_CHASSIS_MECHANICAL_WHEEL_CENTER_LENGTH0 375 /* - mm - */
#define wtrcfg_CHASSIS_MECHANICAL_WHEEL_CENTER_LENGTH1 375
#define wtrcfg_CHASSIS_MECHANICAL_WHEEL_CENTER_LENGTH2 375
#define wtrcfg_CHASSIS_MECHANICAL_WHEEL_CENTER_LENGTH3 375

/* ---------- App/底盘/PID参数 ---------- */
#define wtrcfg_PID_a 0
#define wtrcfg_PID_v 1
#define wtrcfg_PID_x 2
#define wtrcfg_PID_x_TURN_KP 10000.0f
#define wtrcfg_PID_x_TURN_KI 100.3f
#define wtrcfg_PID_x_TURN_KD 100.0f
#define wtrcfg_PID_x_TURN_OUTPUTMAX 10000.0f	
#define wtrcfg_PID_v_TURN_KP 15
#define wtrcfg_PID_v_TURN_KI 1
#define wtrcfg_PID_v_TURN_KD 1
#define wtrcfg_PID_v_TURN_OUTPUTMAX 10000

/* ---------- App/底盘/电机信息 ---------- */
#define wtrcfg_MOTORINFO_EXT

/* ---------- App/底盘/电机参数 ---------- */
#define wtrcfg_MOTOR_DJI2006_ENCODER 8192
#define wtrcfg_MOTOR_DJI2006_GEAR_RATE 36.0f

/* ---------- App/底盘/电调参数 ---------- */
/* ------- App/底盘/电调参数/CAN ------- */
#define wtrcfg_CAN_HANDLE hcan
#define wtrcfg_CAN_RXBUFFER_SIZE 100

/* ------- App/底盘/电调参数/DJI ------- */
#define wtrcfg_CAN_CHASSIS_DJI_ESC_ID1 0x201
#define wtrcfg_CAN_CHASSIS_DJI_ESC_ID2 0x202
#define wtrcfg_CAN_CHASSIS_DJI_ESC_ID3 0x203
#define wtrcfg_CAN_CHASSIS_DJI_ESC_ID4 0x204

/* ------ App/底盘/电调参数/VESC ------ */
#define wtrcfg_VESC_MODE_CAN 1
#define wtrcfg_VESC_MODE_UART 0
#define wtrcfg_VESC_ID_BASE 0x00
#define wtrcfg_VESC_COMMAND_DUTY_MAX 100
#define wtrcfg_VESC_COMMAND_CURRENT_MAX 10
#define wtrcfg_VESC_COMMAND_POS_MAX 360
#define wtrcfg_VESC_COMMAND_ERPM_MAX 20000
#define wtrcfg_VESC_UART_TIMEOUT 0xff

/* ---------------- App/遥控 ---------------- */
/* ------------ App/遥控/缓存 ------------ */
#define wtrcfg_REMOTE_ACK_BUFSIZE 32
#define wtrcfg_REMOTE_RX_BUFSIZE 7

/* ------------ App/遥控/位号 ------------ */
#define wtrcfg_REMOTE_BUF_JOYSTICK_RX_MASK 3
#define wtrcfg_REMOTE_BUF_JOYSTICK_RY_MASK 4
#define wtrcfg_REMOTE_BUF_JOYSTICK_LX_MASK 1
#define wtrcfg_REMOTE_BUF_JOYSTICK_LY_MASK 2
#define wtrcfg_REMOTE_BUF_SUM_ENBALE 1

#endif




