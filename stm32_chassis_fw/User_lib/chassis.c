/* --- std head file --- */
#include <stddef.h>
#include <math.h>
/* --- os head file --- */

/* --- hal head file --- */

/* --- prj head file --- */
#include "WTRConfig.h"
#include "chassis.h"
#include "signal.h"
/* --- user head file --- */

VectorChassis_t chassis;

VESC_t vesc[4];


void PID_Calc(PID_t *pid){
	pid->error[2] = pid->ref - pid->fdb ;
	pid->output += pid->KP * (pid->error[2] - pid->error[1])  + pid->KI *pid->error[2] + pid->KD * (pid->error[2] - 2 * pid->error[1] + pid->error[0]) ;
	pid->error[0] = pid->error[1] ; 
	pid->error[1] = pid->error[2] ;
	/*设定输出上限*/
	if(pid->output > pid->outputMax) pid->output = pid->outputMax ;
	if(pid->output < -pid->outputMax) pid->output = -pid->outputMax ;
}





/*--------------------------------------------------------
 *					Chassis Model 															
 *				
 * 		  								   angular.z  anti-clockwise  rad / s
 *	  ↙ head ↖						 
 *    3      0             linear.x   →  						  mm  / s
 *		   ..     
 *		2			 1	           linear.y   ↑  						  mm  / s      
 *    ↘      ↗     
 *-------------------------------------------------------*/		
/* --- some const value definition (depend on mechanical layout) --- */

/* -- wheel orientation when turn around the center -- */
static const float woac[4] = { wtrcfg_PI / 2  + wtrcfg_CHASSIS_MECHANICAL_WHEEL_ANGLE0 , 
															 wtrcfg_PI / 2  + wtrcfg_CHASSIS_MECHANICAL_WHEEL_ANGLE1 ,
															 wtrcfg_PI / 2  + wtrcfg_CHASSIS_MECHANICAL_WHEEL_ANGLE2 ,
															 wtrcfg_PI / 2  + wtrcfg_CHASSIS_MECHANICAL_WHEEL_ANGLE3 };

/* -- wheel to center length -- */
static const float wtcl[4] = { wtrcfg_CHASSIS_MECHANICAL_WHEEL_CENTER_LENGTH0 ,
														   wtrcfg_CHASSIS_MECHANICAL_WHEEL_CENTER_LENGTH1 ,
															 wtrcfg_CHASSIS_MECHANICAL_WHEEL_CENTER_LENGTH2 ,
															 wtrcfg_CHASSIS_MECHANICAL_WHEEL_CENTER_LENGTH3 };

static float command_theta[4], command_theta_now[4], command_theta_last[4], command_theta_round[4] ;
static void VectorWheelSetRef(Twist_t twist){
	float vx , vy ,
				temp ;
	
	/* --- threshold --- */
	if( fabsf(twist.linear.x) <= wtrcfg_CHASSIS_CONTROL_THRESHOLD_X )
		twist.linear.x = 0;
	if( fabsf(twist.linear.y) <= wtrcfg_CHASSIS_CONTROL_THRESHOLD_Y )
		twist.linear.y = 0; 
	if( fabsf(twist.angular.z) <= wtrcfg_CHASSIS_CONTROL_THRESHOLD_Z )
		twist.angular.z = 0;
	
	if( (twist.linear.x == 0) && (twist.linear.y ==0) ){
		/* --- only velocity == 0 , access to turn --- */
		if( twist.angular.z != 0 ){
			for(int i=0;i<4;++i){		
				vx = twist.angular.z * wtcl[i] * cosf(woac[i]);
				vy = twist.angular.z * wtcl[i] * sinf(woac[i]);
				/* -- update last value -- */
				command_theta_last[i] = command_theta_now[i] ;
				/* -- calculate angle to turn -- */
				command_theta_now[i] = atan2f( vy,vx ) - wtrcfg_PI/2 ;
				if( (command_theta_now[i] - command_theta_last[i]) > wtrcfg_PI/2)
					--command_theta_round[i];
				if( (command_theta_now[i] - command_theta_last[i]) < - wtrcfg_PI/2)
					++command_theta_round[i];
				command_theta[i] = command_theta_now[i] + command_theta_round[i] * 2 * wtrcfg_PI ;
				/* - update motor.turn pid ref - */
	/**/  chassis.wheel[i].turn.ctrl[wtrcfg_PID_x].ref = command_theta[i] * wtrcfg_CHASSISMODEL_GEAR_RATE;
				
				/* -- calculate velocity to roll -- */
				temp = sqrtf( vx*vx + vy*vy );
				/* - update motor.roll pid ref - */
	/**/  chassis.wheel[i].roll.ctrl[wtrcfg_PID_v].ref = temp;
			}
		}
		else{
			/* --- all zero --- */
			for(int i=0;i<4;++i){		
				/* -- update last value -- */
				command_theta[i] = command_theta_last[i] = command_theta_now[i] = command_theta_round[i] = 0 ;
				/* - update motor.turn pid ref - */
	/**/  chassis.wheel[i].turn.ctrl[wtrcfg_PID_x].ref = 0;
				/* - update motor.roll pid ref - */
	/**/  chassis.wheel[i].roll.ctrl[wtrcfg_PID_v].ref = 0;
			}
		}
	}
	else{
		/* --- translation  --- */	
		vx = twist.linear.x;
		vy = twist.linear.y;
		/* -- calculate angle to turn -- */
		/* - atan2f(y,x)~(-Π,Π] - */
		temp = atan2f( vy,vx ) - wtrcfg_PI/2 ;
	
		for(int i=0;i<4;++i){
			/* -- update last value -- */
			command_theta_last[i] = command_theta_now[i] ;
			command_theta_now[i] = temp ;
			if( (command_theta_now[i] - command_theta_last[i]) > wtrcfg_PI/2)
				--command_theta_round[i];
			if( (command_theta_now[i] - command_theta_last[i]) < - wtrcfg_PI/2)
				++command_theta_round[i];
			command_theta[i] = command_theta_now[i] + command_theta_round[i] * 2 * wtrcfg_PI ;
			/* - update motor.turn pid ref - */
/**/  chassis.wheel[i].turn.ctrl[wtrcfg_PID_x].ref = command_theta[i] * wtrcfg_CHASSISMODEL_GEAR_RATE;
			
			/* -- calculate velocity to roll -- */
			/* - update motor.roll pid ref - */
/**/  chassis.wheel[i].roll.ctrl[wtrcfg_PID_v].ref = sqrtf( vx*vx + vy*vy ) ;
		}
  }
}

/* ---- input twist data-> 3pid calculate-> hardware interface ---- */
void VectorCommand(Twist_t twist){
	
	/* --- using kinematics model set pid ref--- */
	VectorWheelSetRef(twist);
	
	/* --- run pid --- */
	for( int i = 0; i < 4; ++i ){
		/* -- run turn x pid -- */
		chassis.wheel[i].turn.ctrl[wtrcfg_PID_x].fdb = chassis.wheel[i].turn.info.pos_axis ;
		PID_Calc( &( chassis.wheel[i].turn.ctrl[wtrcfg_PID_x] ) ) ;
		/* -- run turn v pid -- */
		chassis.wheel[i].turn.ctrl[wtrcfg_PID_v].ref = chassis.wheel[i].turn.ctrl[wtrcfg_PID_x].output ;
		chassis.wheel[i].turn.ctrl[wtrcfg_PID_v].fdb = chassis.wheel[i].turn.info.rpm_axis * wtrcfg_MOTOR_DJI2006_GEAR_RATE;
		PID_Calc( &( chassis.wheel[i].turn.ctrl[wtrcfg_PID_v] ) ) ;
		/* -- run roll v pid -- */
		/* - using esc so set output directly - */
		chassis.wheel[i].roll.ctrl[wtrcfg_PID_v].output = chassis.wheel[i].roll.ctrl[wtrcfg_PID_v].ref ;
	}
	/* --- hardware signal --- */
  /* -- todolist :creat a command buf/poll , inject to command -- */
	can_send_DJI2006();
	can_send_VESC();
}


void chassis_init(void){
	for(int i=0;i<4;i++){
		chassis.wheel[i].turn.info.rate_gear = wtrcfg_MOTOR_DJI2006_GEAR_RATE;
		chassis.wheel[i].turn.ctrl[wtrcfg_PID_x].KP = wtrcfg_PID_x_TURN_KP ;
		chassis.wheel[i].turn.ctrl[wtrcfg_PID_x].KI = wtrcfg_PID_x_TURN_KI ;
		chassis.wheel[i].turn.ctrl[wtrcfg_PID_x].KD = wtrcfg_PID_x_TURN_KD ;
		chassis.wheel[i].turn.ctrl[wtrcfg_PID_x].outputMax = wtrcfg_PID_x_TURN_OUTPUTMAX ;
		chassis.wheel[i].turn.ctrl[wtrcfg_PID_v].KP = wtrcfg_PID_v_TURN_KP ;
		chassis.wheel[i].turn.ctrl[wtrcfg_PID_v].KI = wtrcfg_PID_v_TURN_KP ;
		chassis.wheel[i].turn.ctrl[wtrcfg_PID_v].KD = wtrcfg_PID_v_TURN_KP ;
		chassis.wheel[i].turn.ctrl[wtrcfg_PID_v].outputMax = wtrcfg_PID_v_TURN_OUTPUTMAX ;
		
		chassis.wheel[i].roll.info.controller_id = wtrcfg_VESC_ID_BASE + i ;
		
		vesc[i].hcann = &hcan;
		vesc[i].controller_id = chassis.wheel[i].roll.info.controller_id;
	}
}

