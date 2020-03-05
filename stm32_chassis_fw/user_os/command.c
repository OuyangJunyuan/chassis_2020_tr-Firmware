/* --- std head file --- */
#include <limits.h>
#include <stdbool.h>
/* --- os head file --- */
#include "FreeRTOS.h"
#include "task.h"
/* --- hal head file --- */

/* --- prj head file --- */
#include "WTRconfig.h"
#include "command.h"
#include "signal.h"
/* --- user head file --- */




TaskHandle_t CommandTaskHandle;
void CommandTask(void *argument);

void command_init(void)
{
	BaseType_t xReturn = pdPASS;
	xReturn = xTaskCreate((TaskFunction_t )CommandTask,
												(const char*    )"CommandTask",
												(uint16_t			  )wtrcfg_STACKDEPTH_COMMAND,
												(void*          )NULL,
												(UBaseType_t    )wtrcfg_PRIORITY_COMMAND,
												(TaskHandle_t*  )&CommandTaskHandle);
	if(xReturn==pdPASS){
    ;
  }/* 任务创建成功	*/
  else 
    while(1);
}

void CommandTask(void *argument)
{
	BaseType_t xReturn = pdTRUE;
	uint32_t event;
	for(;;)
	{
		/* --- block mode evet , wait command --- */
		xReturn = xTaskNotifyWait((uint32_t)0,
															(uint32_t)ULONG_MAX,
															(uint32_t*)&event,
															(TickType_t)portMAX_DELAY);
    
		/* --- process command --- */
    
	}
}
