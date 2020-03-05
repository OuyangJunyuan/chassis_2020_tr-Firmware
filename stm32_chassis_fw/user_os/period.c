/* --- std head file --- */

/* --- os head file --- */
#include "FreeRTOS.h"
#include "task.h"
/* --- hal head file --- */

/* --- prj head file --- */
#include "WTRconfig.h"
#include "period.h"
#include "chassis.h"
/* --- user head file --- */



TaskHandle_t PeriodTaskHandle;
void PeriodTask(void *argument);

void period_init(void)
{
	BaseType_t xReturn = pdPASS;
	xReturn = xTaskCreate((TaskFunction_t )PeriodTask,
												(const char*    )"PeriodTask",
												(uint16_t			  )wtrcfg_STACKDEPTH_PERIOD,
												(void*          )NULL,
												(UBaseType_t    )wtrcfg_PRIORITY_PERIOD,
												(TaskHandle_t*  )&PeriodTaskHandle);
	if(xReturn==pdPASS){;}/* 任务创建成功	*/else while(1);
												
												
	
}

void PeriodTask(void *argument){
	static portTickType PreviousWakeTime;
	PreviousWakeTime = xTaskGetTickCount();
	for(;;)
	{
		/* --- ↓↓↓ exacty delay until 1ms later (1000Hz) ↓↓↓ --- */
		vTaskDelayUntil( &PreviousWakeTime,wtrcfg_TIMEINCREMENT_TICK );
    /* --- ↑↑↑ exacty delay until 1ms later (1000Hz) ↑↑↑ --- */
    
		VectorCommand(chassis.target);
	}
}

