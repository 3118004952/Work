
#ifndef QUEUE_H

#define QUEUE_H

#include "stdlib.h"
#include "system.h"


typedef enum
{
    FALSE=0, TRUE=1
} Status;

typedef struct node
{
    u8 *data;                   //������ָ��
	int length;					//����
    struct node *next;         //ָ��ǰ������һ���
} Node;

typedef struct Lqueue
{
    Node *front;                   //��ͷ 
    Node *rear;                    //��β
    int length;            //���г��� 
} LQueue;

/**
 *  @name        : void InitLQueue(LQueue *Q)
 *    @description : ��ʼ������
 *    @param         Q ����ָ��Q
 *  @notice      : None
 */
void InitLQueue(LQueue *Q);

/**
 *  @name        : Status EnLQueue(LQueue *Q, void *data)
 *    @description : ��Ӳ���
 *    @param         Q ����ָ��Q,�������ָ��data
 *    @return         : �������ָ��data
 *  @notice      : �����Ƿ�Ϊ��
 */
Status EnLQueue(LQueue *Q, u8 *data, int length);

/**
 *  @name        : Status DeLQueue(LQueue *Q)
 *    @description : ���Ӳ���
 *    @param         Q ����ָ��Q
 *    @return         : 成功-TRUE; 失败-FALSE
 *  @notice      : None
 */
Status DeLQueue(LQueue *Q);

/**
 *  @name        : Status GetHeadLQueue(LQueue *Q, void *e)
 *    @description : �鿴��ͷԪ��
 *    @param         Q e ����ָ��Q,��������ָ��e
 *    @return         : 成功-TRUE; 失败-FALSE
 *  @notice      : �����Ƿ��
 */
Status GetHeadLQueue(LQueue *Q, u8 *data, int length);



#endif 

