#include "queue.h"



/**
 *  @name        : void InitLQueue(LQueue *Q)
 *    @description : 初始化队列
 *    @param         Q 队列指针Q
 *  @notice      : None
 */
void InitLQueue(LQueue *Q)
{
	Q->front = NULL;//将指针指向空
	Q->rear = NULL;
	Q->length = 0; 
}

/**
 *  @name        : Status EnLQueue(LQueue *Q, void *data)
 *    @description : 入队操作
 *    @param         Q 队列指针Q,入队数据指针data
 *    @return         : 入队数据指针data
 *  @notice      : 队列是否为空
 */
Status EnLQueue(LQueue *Q, u8 *data, int length)
{
	if(Q == NULL)
	{
		return FALSE;
	}
	Node *enter;
	enter = (Node *)malloc(sizeof(Node)); //分配空间 
	enter->data = data;			//赋值 
	enter->length = length;
	enter->next = NULL;
	if(Q->front == NULL)	//判断队列是否为空 
	{
		Q->front = enter;
		Q->rear = enter;
	}
	else
	{
		Q->rear->next = enter;
		Q->rear = enter;
	}
	Q->length += 1;
	return TRUE;
}

/**
 *  @name        : Status DeLQueue(LQueue *Q)
 *    @description : 出队操作
 *    @param         Q 队列指针Q
 *    @return         : 鎴愬姛-TRUE; 澶辫触-FALSE
 *  @notice      : None
 */
Status DeLQueue(LQueue *Q)
{
	if(Q->front == NULL)	//判断队列是否为空 
	{
		return FALSE;
	}
	Node *del;
	del = Q->front;		//得到删除的节点 
	Q->front = Q->front->next;
	free(del->data); 
	free(del);
	if(Q->front == NULL)	//判断删除后队列是否为空 
	{
		Q->rear = NULL;
	}
	Q->length -= 1;
	return TRUE;
}

/**
 *  @name        : Status GetHeadLQueue(LQueue *Q, void *e)
 *    @description : 查看队头元素
 *    @param         Q e 队列指针Q,返回数据指针e
 *    @return         : 鎴愬姛-TRUE; 澶辫触-FALSE
 *  @notice      : 队列是否空
 */
Status GetHeadLQueue(LQueue *Q, u8 *data, int length)
{
	if(Q == NULL || Q->front == NULL) //判断是否为空 
	{
		return FALSE;
	}
	data = Q->front->data;
	length = Q->front->length;
	return TRUE;
}


