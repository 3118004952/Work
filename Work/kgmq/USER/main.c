#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "sram.h"
#include "beep.h"
#include "UART2.h"
#include "JY901.h"
#include "wkup.h"
#include "includes.h"
#include "timer.h"
#include "gas.h"

#define Pi 3.141593

//UCOSIII中以下优先级用户程序不能使用，ALIENTEK
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()

extern struct Measure measure;

//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//任务优先级
#define SENSOR_TASK_PRIO		4
//任务堆栈大小	
#define SENSOR_STK_SIZE 		128
//任务控制块
OS_TCB Sensor_TaskTCB;
//任务堆栈	
CPU_STK SENSOR_TASK_STK[SENSOR_STK_SIZE];
//任务函数
void sensor_task(void *p_arg);

//任务优先级
#define MPU6050_TASK_PRIO	5
//任务堆栈大小
#define MPU6050_STK_SIZE	256
//任务控制块
OS_TCB	MPU6050_TaskTCB;
//任务堆栈
CPU_STK MPU6050_TASK_STK[MPU6050_STK_SIZE];
//任务函数
void mpu6050_task(void *p_arg);

//任务优先级
#define LOCATION_TASK_PRIO	5
//任务堆栈大小
#define LOCATION_STK_SIZE	128
//任务控制块
OS_TCB	LOCATION_TaskTCB;
//任务堆栈
CPU_STK LOCATION_TASK_STK[LOCATION_STK_SIZE];
//任务函数
void location_task(void *p_arg);

//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();  //时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	USART1_Init(115200);   //串口1初始化
	USART2_Init(115200);   //串口2初始化
	USART3_Init(115200);   //串口2初始化
	LED_Init();         //LED初始化
	KEY_Init();			//按键初始化
	GAS_Init();			//气体传感器IO口初始化
	TIM3_Int_Init(2000, 719);//72000000 / (720 * 2000) = 20ms
	BEEP_Init();		//初始化蜂鸣器
	
	OSInit(&err);		    //初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区	
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);      //开启UCOSIII
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
		
	OS_CRITICAL_ENTER();	//进入临界区
	//创建气体、红外传感器任务
	OSTaskCreate((OS_TCB*     )&Sensor_TaskTCB,		
				 (CPU_CHAR*   )"Sensor task", 		
                 (OS_TASK_PTR )sensor_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )SENSOR_TASK_PRIO,     
                 (CPU_STK*    )&SENSOR_TASK_STK[0],	
                 (CPU_STK_SIZE)SENSOR_STK_SIZE/10,	
                 (CPU_STK_SIZE)SENSOR_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
				 
	 //创建六轴传感器任务
	OSTaskCreate((OS_TCB*     )&MPU6050_TaskTCB,		
				 (CPU_CHAR*   )"MPU6050 task", 		
                 (OS_TASK_PTR )mpu6050_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )MPU6050_TASK_PRIO,     
                 (CPU_STK*    )&MPU6050_TASK_STK[0],	
                 (CPU_STK_SIZE)MPU6050_STK_SIZE/10,	
                 (CPU_STK_SIZE)MPU6050_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
				 
	 //创建定位任务	
	OSTaskCreate((OS_TCB*     )&LOCATION_TaskTCB,		
				 (CPU_CHAR*   )"Location task", 		
                 (OS_TASK_PTR )location_task, 								
                 (void*       )0,
                 (OS_PRIO	  )LOCATION_TASK_PRIO,     
                 (CPU_STK*    )&LOCATION_TASK_STK[0],	
                 (CPU_STK_SIZE)LOCATION_STK_SIZE/10,	
                 (CPU_STK_SIZE)LOCATION_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);

	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}

//气体、红外传感器模块
void sensor_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	while(1)
	{
		if(MQ135 == 1)//空气质量、有害气体检测
		{
			delay_ms(10);
			if(MQ135 == 1)
			{
				OS_CRITICAL_ENTER();	//进入临界区
				
				OS_CRITICAL_EXIT();	//退出临界区
			}
		}
		if(MQ7 == 1)//CO检测
		{
			delay_ms(10);
			if(MQ7 == 1)
			{
				OS_CRITICAL_ENTER();	//进入临界区
				
				OS_CRITICAL_EXIT();	//退出临界区
			}
		}
		if(MQ6 == 0)//丙烷、丁烷、液化气检测
		{
			delay_ms(10);
			if(MQ6 == 0)
			{
				OS_CRITICAL_ENTER();	//进入临界区
				
				OS_CRITICAL_EXIT();	//退出临界区
			}
		}
		if(MQ4 == 0)//甲烷、天然气检测
		{
			delay_ms(10);
			if(MQ4 == 0)
			{
				OS_CRITICAL_ENTER();	//进入临界区
				
				OS_CRITICAL_EXIT();	//退出临界区
			}
		}
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);   //延时10ms
	}
}

//六轴传感器
void mpu6050_task(void *p_arg)
{
	int time = 0;
	int judge = 0;
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	while(1)
	{
		OS_CRITICAL_ENTER();	//进入临界区

//		printf("\r\nAcceleration:\r\nX:%f\r\nY:%f\r\nZ:%f\r\n", measure.Acceleration_X, measure.Acceleration_Y, measure.Acceleration_Z);
//		printf("\r\nAngle:\r\nX:%f\r\nY:%f\r\nZ:%f\r\n", measure.Angle_X, measure.Angle_Y, measure.Angle_Z);
		
		if(measure.Acceleration_X > 1.2 || measure.Acceleration_Y > 1.2 )
		{	//用于判断加速度是否达到了摔倒的阈值
			judge = 1;//作为判断条件
			time = 1;//计时开始
			
//			printf("\r\nAcceleration:\r\nX:%f\r\nY:%f\r\nZ:%f\r\n", measure.Acceleration_X, measure.Acceleration_Y, measure.Acceleration_Z);
		}
		if(judge != 0)
		{
			if((fabs(measure.Acceleration_X) + fabs(measure.Acceleration_Y)) > 1.2)
			{
				judge++;
				printf("%d\r\n", judge);
			}
		}
		if(time !=0)
		{
			time++;
			
//			printf("\r\nAcceleration:\r\nX:%f\r\nY:%f\r\nZ:%f\r\n", measure.Acceleration_X, measure.Acceleration_Y, measure.Acceleration_Z);
		}
		if(time == 400)//0.04 * 400   即16秒的判断时间
		{
			if(judge > 300)//判断是否倒地不起
			{
				LED1 = 0;//表示摔倒且倒地不起
			}
			time = 0;
			judge = 0;
		}
		OS_CRITICAL_EXIT();	//退出临界区
		
		OSTimeDlyHMSM(0,0,0,40,OS_OPT_TIME_PERIODIC,&err);//延时40ms
	}
}

//定位
void location_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	
	while(1)
	{	
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);//延时100ms
	}
}

