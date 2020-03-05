#include "gpio.h"
#include <stdlib.h>
														


//目的是为了不使用hal_delay占用cpu。
//keil的enum类型问题在这里注释一下,标准C是以int处理的，但是keil中按
//你最大值来节省空间，如果里面没有超过255的，他以1个1个字节处理，如果有0x7fffff的他以4个字节处理以此类推
typedef enum
{
	LED_STATE_END=-4,
	LED_STATE_TOGGLE=-3,
	LED_STATE_OFF=-2,
	LED_STATE_ON=-1,
	LED_STATE_READY=0,

	LED_ENUM_MAX=0X7FFFFFFF
}LED_STATE;

typedef struct
{
	enum{LED_LOGIC_P=1,LED_LOGIC_N=0}logic;
	LED_STATE state;
	
	GPIO_TypeDef *port;
	uint16_t pin;
}LED_ONE;




typedef struct
{
	uint8_t which2do;
	LED_STATE what2do;
}LED_TASK;


typedef  LED_ONE*  LEDSET;
typedef  LED_TASK* LED_TASK_QUEUE ;

typedef struct
{
	LEDSET led_set;
	uint8_t numofleds;
	
	LED_TASK_QUEUE task_queue;
	uint8_t numoftasks;
	
	uint8_t tasknow_point2;
	uint32_t tick;
	
	void (*Doing)(void);
	void (*Init)(void);
}LED_HandleTypeDef;

extern LED_HandleTypeDef hled;

