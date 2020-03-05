/* --- std head file --- */

/* --- os head file --- */
#include "FreeRTOS.h"
#include "task.h"
/* --- hal head file --- */

/* --- prj head file --- */
#include "WTRconfig.h"
#include "support.h"
/* --- user head file --- */








TaskHandle_t SupportTaskHandle;
void SupportTask(void *argument);

void support_init(void)
{
	BaseType_t xReturn = pdPASS;
	xReturn = xTaskCreate((TaskFunction_t )SupportTask,
												(const char*    )"SupportTask",
												(uint16_t			  )wtrcfg_STACKDEPTH_SUPPORT,
												(void*          )NULL,
												(UBaseType_t    )wtrcfg_PRIORITY_SUPPORT,
												(TaskHandle_t*  )&SupportTaskHandle);
	if(xReturn==pdPASS){
    ;
  }/* 任务创建成功	*/
  else
    while(1);
												
											
}

void SupportTask(void *argument)
{
	for(;;)
	{
    /* --- to display --- */
		vTaskDelay(1);
	}
}

