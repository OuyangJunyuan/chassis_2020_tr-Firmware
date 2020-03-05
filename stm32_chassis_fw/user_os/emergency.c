/* --- std head file --- */
#include <limits.h>
/* --- os head file --- */
#include "FreeRTOS.h"
#include "task.h"
/* --- hal head file --- */
#include "gpio.h"
/* --- prj head file --- */
#include "WTRconfig.h"
#include "emergency.h"
/* --- user head file --- */


TaskHandle_t EmergencyTaskHandle;
void EmergencyTask(void *argument);

void emergency_init(void)
{
	BaseType_t xReturn = pdPASS;
	xReturn = xTaskCreate((TaskFunction_t )EmergencyTask,
												(const char*    )"EmergencyTask",
												(uint16_t			  )wtrcfg_STACKDEPTH_EMERGENCY,
												(void*          )NULL,
												(UBaseType_t    )wtrcfg_PRIORITY_EMERGENCY,
												(TaskHandle_t*  )&EmergencyTaskHandle);
	if(xReturn==pdPASS){
    ;
  }/* 任务创建成功	*/
  else 
    while(1);						
}



void EmergencyTask(void *argument)
{
	BaseType_t xReturn = pdTRUE;
	uint32_t event;
	for(;;){
    /* --- block mode to wait event occur  --- */
		xReturn = xTaskNotifyWait((uint32_t)0,
															(uint32_t)ULONG_MAX,
															(uint32_t*)&event,
															(TickType_t)portMAX_DELAY);
		
    /* --- event bit detect --- */
		if((event&wtrcfg_EVENT_BIT0_EMERGENCY)==0){
			/* --- emgergency stop button is pressed --- */
			HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
		}
	}
}

