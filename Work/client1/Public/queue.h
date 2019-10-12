
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
    u8 *data;                   //数据域指针
	int length;					//长度
    struct node *next;         //指向当前结点的下一结点
} Node;

typedef struct Lqueue
{
    Node *front;                   //队头 
    Node *rear;                    //队尾
    int length;            //队列长度 
} LQueue;

/**
 *  @name        : void InitLQueue(LQueue *Q)
 *    @description : 初始化队列
 *    @param         Q 队列指针Q
 *  @notice      : None
 */
void InitLQueue(LQueue *Q);

/**
 *  @name        : Status EnLQueue(LQueue *Q, void *data)
 *    @description : 入队操作
 *    @param         Q 队列指针Q,入队数据指针data
 *    @return         : 入队数据指针data
 *  @notice      : 队列是否为空
 */
Status EnLQueue(LQueue *Q, u8 *data, int length);

/**
 *  @name        : Status DeLQueue(LQueue *Q)
 *    @description : 出队操作
 *    @param         Q 队列指针Q
 *    @return         : 鎴愬姛-TRUE; 澶辫触-FALSE
 *  @notice      : None
 */
Status DeLQueue(LQueue *Q);

/**
 *  @name        : Status GetHeadLQueue(LQueue *Q, void *e)
 *    @description : 查看队头元素
 *    @param         Q e 队列指针Q,返回数据指针e
 *    @return         : 鎴愬姛-TRUE; 澶辫触-FALSE
 *  @notice      : 队列是否空
 */
Status GetHeadLQueue(LQueue *Q, u8 *data, int length);



#endif 

