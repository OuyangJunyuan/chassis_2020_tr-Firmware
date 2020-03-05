#include "led.h"
#define CREAT_ONE_LED(__HANDLE,__LOGIC,__PORT,__PIN,__COUNT)  do{\
																												 ((LED_HandleTypeDef*)__HANDLE)->led_set[__COUNT].port=__PORT;	  \
																												 ((LED_HandleTypeDef*)__HANDLE)->led_set[__COUNT].pin=__PIN;     \
																												 ((LED_HandleTypeDef*)__HANDLE)->led_set[__COUNT].state=LED_STATE_READY;   \
																												 ((LED_HandleTypeDef*)__HANDLE)->led_set[__COUNT].logic=__LOGIC;   \
																												}while(0)		

#define HAL_LED_ON(__LED) HAL_GPIO_WritePin(__LED.port,__LED.pin,__LED.logic==LED_LOGIC_P?GPIO_PIN_SET:GPIO_PIN_RESET)
#define HAL_LED_OFF(__LED) HAL_GPIO_WritePin(__LED.port,__LED.pin,__LED.logic==LED_LOGIC_P?GPIO_PIN_RESET:GPIO_PIN_SET)																									
#define HAL_LED_TOGGLE(__LED)	HAL_GPIO_TogglePin(__LED.port,__LED.pin)

																												
#define newTASK(__WHICH,__WHAT) {.which2do=__WHICH,.what2do=__WHAT}
#define LED_STATE_DELAY(__n) ((LED_STATE)__n)							
#define SIZEOFTASKS(__TASK)	(sizeof(__TASK)/sizeof(LED_TASK))																			
#define ALL_LED INT8_MAX				



void LED_INIT(void);
void LED_DOING(void);
LED_HandleTypeDef hled={.Init=LED_INIT,.Doing=LED_DOING};




//在这里学到了enum在switch不default的时候会wanring。
void LED_DOING()
{ 
	if(hled.tick==HAL_GetTick())
		return;
	else 
		hled.tick=HAL_GetTick();
	
	
	uint8_t flag1=0,flag2=0;
	for(int i=0;i<hled.numofleds;++i)
	{
		if(hled.led_set[i].state>0)
		{
			--(hled.led_set[i].state);
			++flag1;
		}
	}
	if(flag1==hled.numofleds)
		return;


	LED_TASK task	=	hled.task_queue[hled.tasknow_point2];
	if(task.which2do==ALL_LED)
		flag1=hled.numofleds-1;
	else 
	{
		flag1=flag2=task.which2do;
	}
		
	switch(task.what2do)
	{
	case LED_STATE_READY:
		break;
	case LED_STATE_ON:
		for(int i=flag2;i<=flag1;++i)
				HAL_LED_ON(hled.led_set[i]);
		break;
	case LED_STATE_OFF:
		for(int i=flag2;i<=flag1;++i)
				HAL_LED_OFF(hled.led_set[i]);
		break;
	case LED_STATE_TOGGLE:
		for(int i=flag2;i<=flag1;++i)
				HAL_LED_TOGGLE(hled.led_set[i]);
		break;
	default:
		for(int i=flag2;i<=flag1;++i)
				hled.led_set[i].state=task.what2do;
	}
	
	if(hled.tasknow_point2!=hled.numoftasks)
		++(hled.tasknow_point2);
	else
		hled.tasknow_point2=0;
}



LED_TASK taskqueue[]=
{
	newTASK(0,LED_STATE_TOGGLE),newTASK(ALL_LED,LED_STATE_DELAY(50)),
	newTASK(1,LED_STATE_TOGGLE),newTASK(ALL_LED,LED_STATE_DELAY(50)),
	newTASK(2,LED_STATE_TOGGLE),newTASK(ALL_LED,LED_STATE_DELAY(50)),
	newTASK(3,LED_STATE_TOGGLE),newTASK(ALL_LED,LED_STATE_DELAY(50)),
	newTASK(4,LED_STATE_TOGGLE),newTASK(ALL_LED,LED_STATE_DELAY(50)),
	newTASK(5,LED_STATE_TOGGLE),newTASK(ALL_LED,LED_STATE_DELAY(50)),
	newTASK(6,LED_STATE_TOGGLE),newTASK(ALL_LED,LED_STATE_DELAY(50)),
	newTASK(7,LED_STATE_TOGGLE),newTASK(ALL_LED,LED_STATE_DELAY(50)),
};

void LED_INIT(void)
{
	hled.numofleds=8;
	hled.led_set =(LED_ONE*)malloc(hled.numofleds * sizeof(LED_ONE));
	
	
	CREAT_ONE_LED(&hled,LED_LOGIC_P,GPIOE,GPIO_PIN_2,0);
	CREAT_ONE_LED(&hled,LED_LOGIC_P,GPIOE,GPIO_PIN_3,1);
	CREAT_ONE_LED(&hled,LED_LOGIC_P,GPIOE,GPIO_PIN_4,2);
	CREAT_ONE_LED(&hled,LED_LOGIC_P,GPIOE,GPIO_PIN_5,3);
	CREAT_ONE_LED(&hled,LED_LOGIC_P,GPIOG,GPIO_PIN_4,4);
	CREAT_ONE_LED(&hled,LED_LOGIC_P,GPIOG,GPIO_PIN_5,5);
	CREAT_ONE_LED(&hled,LED_LOGIC_P,GPIOG,GPIO_PIN_6,6);
	CREAT_ONE_LED(&hled,LED_LOGIC_P,GPIOG,GPIO_PIN_7,7);
	
	hled.task_queue=taskqueue;
	hled.numoftasks=SIZEOFTASKS(taskqueue);
	hled.tasknow_point2=0;
	hled.tick=HAL_GetTick();
}
