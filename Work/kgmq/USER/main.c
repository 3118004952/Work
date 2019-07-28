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

//UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()

extern struct Measure measure;

//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define SENSOR_TASK_PRIO		4
//�����ջ��С	
#define SENSOR_STK_SIZE 		128
//������ƿ�
OS_TCB Sensor_TaskTCB;
//�����ջ	
CPU_STK SENSOR_TASK_STK[SENSOR_STK_SIZE];
//������
void sensor_task(void *p_arg);

//�������ȼ�
#define MPU6050_TASK_PRIO	5
//�����ջ��С
#define MPU6050_STK_SIZE	256
//������ƿ�
OS_TCB	MPU6050_TaskTCB;
//�����ջ
CPU_STK MPU6050_TASK_STK[MPU6050_STK_SIZE];
//������
void mpu6050_task(void *p_arg);

//�������ȼ�
#define LOCATION_TASK_PRIO	5
//�����ջ��С
#define LOCATION_STK_SIZE	128
//������ƿ�
OS_TCB	LOCATION_TaskTCB;
//�����ջ
CPU_STK LOCATION_TASK_STK[LOCATION_STK_SIZE];
//������
void location_task(void *p_arg);

//������
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();  //ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	USART1_Init(115200);   //����1��ʼ��
	USART2_Init(115200);   //����2��ʼ��
	USART3_Init(115200);   //����2��ʼ��
	LED_Init();         //LED��ʼ��
	KEY_Init();			//������ʼ��
	GAS_Init();			//���崫����IO�ڳ�ʼ��
	TIM3_Int_Init(2000, 719);//72000000 / (720 * 2000) = 20ms
	BEEP_Init();		//��ʼ��������
	
	OSInit(&err);		    //��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���	
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);      //����UCOSIII
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
		
	OS_CRITICAL_ENTER();	//�����ٽ���
	//�������塢���⴫��������
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
				 
	 //�������ᴫ��������
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
				 
	 //������λ����	
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

	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
}

//���塢���⴫����ģ��
void sensor_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	while(1)
	{
		if(MQ135 == 1)//�����������к�������
		{
			delay_ms(10);
			if(MQ135 == 1)
			{
				OS_CRITICAL_ENTER();	//�����ٽ���
				
				OS_CRITICAL_EXIT();	//�˳��ٽ���
			}
		}
		if(MQ7 == 1)//CO���
		{
			delay_ms(10);
			if(MQ7 == 1)
			{
				OS_CRITICAL_ENTER();	//�����ٽ���
				
				OS_CRITICAL_EXIT();	//�˳��ٽ���
			}
		}
		if(MQ6 == 0)//���顢���顢Һ�������
		{
			delay_ms(10);
			if(MQ6 == 0)
			{
				OS_CRITICAL_ENTER();	//�����ٽ���
				
				OS_CRITICAL_EXIT();	//�˳��ٽ���
			}
		}
		if(MQ4 == 0)//���顢��Ȼ�����
		{
			delay_ms(10);
			if(MQ4 == 0)
			{
				OS_CRITICAL_ENTER();	//�����ٽ���
				
				OS_CRITICAL_EXIT();	//�˳��ٽ���
			}
		}
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms
	}
}

//���ᴫ����
void mpu6050_task(void *p_arg)
{
	int time = 0;
	int judge = 0;
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	while(1)
	{
		OS_CRITICAL_ENTER();	//�����ٽ���

//		printf("\r\nAcceleration:\r\nX:%f\r\nY:%f\r\nZ:%f\r\n", measure.Acceleration_X, measure.Acceleration_Y, measure.Acceleration_Z);
//		printf("\r\nAngle:\r\nX:%f\r\nY:%f\r\nZ:%f\r\n", measure.Angle_X, measure.Angle_Y, measure.Angle_Z);
		
		if(measure.Acceleration_X > 1.2 || measure.Acceleration_Y > 1.2 )
		{	//�����жϼ��ٶ��Ƿ�ﵽ��ˤ������ֵ
			judge = 1;//��Ϊ�ж�����
			time = 1;//��ʱ��ʼ
			
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
		if(time == 400)//0.04 * 400   ��16����ж�ʱ��
		{
			if(judge > 300)//�ж��Ƿ񵹵ز���
			{
				LED1 = 0;//��ʾˤ���ҵ��ز���
			}
			time = 0;
			judge = 0;
		}
		OS_CRITICAL_EXIT();	//�˳��ٽ���
		
		OSTimeDlyHMSM(0,0,0,40,OS_OPT_TIME_PERIODIC,&err);//��ʱ40ms
	}
}

//��λ
void location_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	
	while(1)
	{	
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);//��ʱ100ms
	}
}

