/* --- std head file --- */

/* --- os head file --- */
#include "FreeRTOS.h"
#include "task.h"
/* --- hal head file --- */
#include "gpio.h"
#include "can.h"
/* --- prj head file --- */
#include "WTRconfig.h"
#include "signal.h"
#include "emergency.h"
#include "chassis.h"
/* --- user head file --- */
#include "vesc.h"
#include "nrf24l01.h"


/****************************
 * @PrevateVariable 
 ****************************/
 
/* --- support variable --- */ 
static struct{
	int32_t msg_cnt;

	int encoder;
	int encoder_last;
	int encoder_round;
	int encoder_offset;
	
	float rpm_rotor;
	float erpm_rotor;
}support_data[4];

TaskHandle_t CanReadTaskHandle;

/* --- for can data receiving --- */ 
#ifndef wtrcfg_CAN_RXBUFFER_SIZE 
#define wtrcfg_CAN_RXBUFFER_SIZE 100
#endif
static void decode_rx_can(void);
static CANRxFrame rx_can[wtrcfg_CAN_RXBUFFER_SIZE];
static int rx_can_write,rx_can_read;

/* --- for nrf24l01 remote data --- */ 
static void decode_rx_remote(void);
static unsigned char remote_ack_buf[wtrcfg_REMOTE_ACK_BUFSIZE]; 
unsigned char remote_rx_buf[wtrcfg_REMOTE_RX_BUFSIZE];
bool is_ack_payload = true;

/****************************
 * @Prevatefunction
 ****************************/
static void decode_dji_esc(CANRxFrame *rxframe);
void  CanReadTask(void *argument);





/****************************
 ****************************
 * @Implementation
 ****************************
 ****************************/
void canread_init(void){
	BaseType_t xReturn = pdPASS;
	xReturn = xTaskCreate((TaskFunction_t ) CanReadTask,
												(const char*    )"CanReadTask",
												(uint16_t			  )wtrcfg_STACKDEPTH_CANREAD,
												(void*          )NULL,
												(UBaseType_t    )wtrcfg_PRIORITY_CANREAD,
												(TaskHandle_t*  )&CanReadTaskHandle);
                        
	if(xReturn==pdPASS){
    ;
  }/* 任务创建成功	*/
  else{
    while(1);
  }

  hnrf24l01.pAckBuffPtr = remote_ack_buf;
  hnrf24l01.AckBuffsize = wtrcfg_REMOTE_ACK_BUFSIZE;

}

/******************************
 *	         @Task
 ******************************/
void CanReadTask(void *argument){
	for(;;){
    /* --- decode data --- */
		/* --- consider to set event or notify to save time --- */
		decode_rx_can();
    decode_rx_remote();
    
    /* --- remote receive interrupt mode --- */
    NRF24L01_Recieve_IT(&hnrf24l01,remote_rx_buf,is_ack_payload);/* continue receive_it mode should be modified to */
   
    /* --- delay --- */
		vTaskDelay(1);
	}
}


/******************************
 *			  @Interrupt
 ******************************/
/* --- emergency task notify (as event) --- */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	
	if(GPIO_Pin==GPIO_PIN_2)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		
		xTaskNotifyFromISR((TaskHandle_t)EmergencyTaskHandle,
											 (uint32_t)wtrcfg_EVENT_BIT0_EMERGENCY,/* BIT0 */
											 (eNotifyAction)eSetBits,
											 (BaseType_t *)&xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}

}


void NRF_RxCpltCallback(NRF24L01_HandleTypeDef *hnrf,uint8_t pipex)
{
  /* --- ack payload buf update --- */
    /* can update chassis pose or status to remote user */
}

uint32_t counttt;

void NRF_RxPayloadAckCpltCallback(NRF24L01_HandleTypeDef *hnrf,uint8_t pipex)
{
  counttt++;
}


/* --- can receive --- */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){/* -- todo list :vesc ->fifo0，c610->fifo1 -- */
	
	CAN_RxHeaderTypeDef   RxHeader;
	if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, rx_can[rx_can_write].data8) != HAL_OK){
    Error_Handler();            
  }
	
	rx_can[rx_can_write].IDE=RxHeader.IDE;
	
	if(RxHeader.IDE==CAN_ID_STD)
		rx_can[rx_can_write].StdId=RxHeader.StdId;
	else 
		rx_can[rx_can_write].ExtId=RxHeader.ExtId;
	
	++rx_can_write;
	if(rx_can_write==wtrcfg_CAN_RXBUFFER_SIZE)
		rx_can_write=0;
}

/******************************
 *    @RXdataDecodeFunction
 ******************************/
/* --- decode can data --- */
static void decode_rx_can(void){
	while (rx_can_read != rx_can_write){
		CANRxFrame rxmsg = rx_can[rx_can_read++];
		/* --- do some processing here--- */
		/* --- such as refeash the chassis motor data --- */
		if(rxmsg.IDE==CAN_ID_STD){
			/* -- stdid frame  -- */
			decode_dji_esc(&rxmsg);
		}else{
			/* -- extid frame  -- */
		}
		
		/* --- overfloat dispose --- */
		if (rx_can_read == wtrcfg_CAN_RXBUFFER_SIZE) {
			rx_can_read = 0;
		}
	}
}

static void decode_dji_esc(CANRxFrame *rxframe){

	int id=rxframe->StdId,temp;
	
	if( (id > wtrcfg_CAN_CHASSIS_DJI_ESC_ID4) || (id < wtrcfg_CAN_CHASSIS_DJI_ESC_ID1) )
		return;
	
	id-=wtrcfg_CAN_CHASSIS_DJI_ESC_ID1;
	
	/* --- process --- */
	support_data[id].msg_cnt++;
	
	if(support_data[id].msg_cnt!=0){
		/* --- update value / encoder --- */		
		support_data[id].encoder_last = support_data[id].encoder;
		support_data[id].encoder = (int16_t)((rxframe->data8[0] << 8) |  rxframe->data8[1] );
		
		if((support_data[id].encoder - support_data[id].encoder_last) > (wtrcfg_MOTOR_DJI2006_ENCODER/2) )
			--(support_data[id].encoder_round);
		if((support_data[id].encoder - support_data[id].encoder_last) < -(wtrcfg_MOTOR_DJI2006_ENCODER/2) )
			++(support_data[id].encoder_round);
		
		chassis.wheel[id].turn.info.pos_axis = ( support_data[id].encoder + support_data[id].encoder_round * wtrcfg_MOTOR_DJI2006_ENCODER - support_data[id].encoder_offset )
																						 * 2 * wtrcfg_PI / ( chassis.wheel[id].turn.info.rate_gear * wtrcfg_MOTOR_DJI2006_ENCODER ) ;
		/* --- update value / rpm --- */
		temp = support_data[id].rpm_rotor = (int16_t)((rxframe->data8[2] << 8) |  rxframe->data8[3]) ;
		chassis.wheel[id].turn.info.rpm_axis = ( temp ) / chassis.wheel[id].turn.info.rate_gear;
		
		/* --- update value / torque --- */
		chassis.wheel[id].turn.info.torque = (int16_t)((rxframe->data8[4] << 8) |  rxframe->data8[5]) ;
	
	}
	else{
		/* --- get encoder offset at the first time --- */
		support_data[id].encoder_offset = (int16_t)((rxframe->data8[0] << 8) |  rxframe->data8[1] ); /* --- bit0 ahead --- */
		support_data[id].encoder = support_data[id].encoder_offset;
	}
}


static uint32_t txmailbox;
/* --- decode remote data --- */
uint8_t iserror;
static void decode_rx_remote(void){
  /* --- ↓↓↓ remote frame definition ↓↓↓ --- *
   *  0xaa | 4Bytes joystick value | reserve key value | sum |
   * --- ↑↑↑ remote frame definition ↑↑↑ --- */
  #if (wtrcfg_REMOTE_BUF_SUM_ENBALE == 1)
  uint8_t sum=0;
  for(int i=0;i<wtrcfg_REMOTE_RX_BUFSIZE-1;++i)
    sum+=remote_rx_buf[i];
  
  if(sum !=remote_rx_buf[wtrcfg_REMOTE_RX_BUFSIZE-1])
  { 
    iserror=0;
    return;
  }
  #endif
  
  /* --- joystick value --- */
  /* --- 需要在option中设置使用有符号的char --- */
  chassis.target.linear.x  = *(int8_t *)(remote_rx_buf+wtrcfg_REMOTE_BUF_JOYSTICK_LX_MASK);
  chassis.target.linear.y  = *(int8_t *)(remote_rx_buf+wtrcfg_REMOTE_BUF_JOYSTICK_LY_MASK);
  chassis.target.angular.z = *(int8_t *)(remote_rx_buf+wtrcfg_REMOTE_BUF_JOYSTICK_RY_MASK);
  /* --- key value --- */
  
}



/* --- can send msg --- */
void can_send_DJI2006(){
	CAN_TxHeaderTypeDef TxMessage;
		
	TxMessage.DLC = 0x08;
	TxMessage.StdId = 0x200;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.RTR = CAN_RTR_DATA;

	uint8_t TxData[8];

	for( int i = 0 ; i < 4 ; ++i ){
		TxData[ i * 2 ] = (uint8_t)( (chassis.wheel[i].turn.ctrl[wtrcfg_PID_v].output) >> 8) ;
		TxData[ i * 2  +1 ] = (uint8_t)( (chassis.wheel[i].turn.ctrl[wtrcfg_PID_v].output) ) ;
	}

	if(HAL_CAN_AddTxMessage(&wtrcfg_CAN_HANDLE,&TxMessage,TxData,&txmailbox)!=HAL_OK){
		 Error_Handler();      
	}
}

void can_send_VESC(){
//	for(int i=0;i<4;++i){
//    /* --- time span is too small,txmailbox may be filled up                   --- *
//     * --- should disable vesc-can-func error_handle,or using send-all command --- */
//    VESC_CAN_SET_DUTY(&vesc[i],(chassis.wheel[i].roll.ctrl[wtrcfg_PID_v].output));
//	}
  VESC_CAN_SET_DUTY_ALL(&vesc[0],(chassis.wheel[0].roll.ctrl[wtrcfg_PID_v].output));
}
