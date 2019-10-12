#include "queue.h"



/**
 *  @name        : void InitLQueue(LQueue *Q)
 *    @description : ��ʼ������
 *    @param         Q ����ָ��Q
 *  @notice      : None
 */
void InitLQueue(LQueue *Q)
{
	Q->front = NULL;//��ָ��ָ���
	Q->rear = NULL;
	Q->length = 0; 
}

/**
 *  @name        : Status EnLQueue(LQueue *Q, void *data)
 *    @description : ��Ӳ���
 *    @param         Q ����ָ��Q,�������ָ��data
 *    @return         : �������ָ��data
 *  @notice      : �����Ƿ�Ϊ��
 */
Status EnLQueue(LQueue *Q, u8 *data, int length)
{
	if(Q == NULL)
	{
		return FALSE;
	}
	Node *enter;
	enter = (Node *)malloc(sizeof(Node)); //����ռ� 
	enter->data = data;			//��ֵ 
	enter->length = length;
	enter->next = NULL;
	if(Q->front == NULL)	//�ж϶����Ƿ�Ϊ�� 
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
 *    @description : ���Ӳ���
 *    @param         Q ����ָ��Q
 *    @return         : 成功-TRUE; 失败-FALSE
 *  @notice      : None
 */
Status DeLQueue(LQueue *Q)
{
	if(Q->front == NULL)	//�ж϶����Ƿ�Ϊ�� 
	{
		return FALSE;
	}
	Node *del;
	del = Q->front;		//�õ�ɾ���Ľڵ� 
	Q->front = Q->front->next;
	free(del->data); 
	free(del);
	if(Q->front == NULL)	//�ж�ɾ��������Ƿ�Ϊ�� 
	{
		Q->rear = NULL;
	}
	Q->length -= 1;
	return TRUE;
}

/**
 *  @name        : Status GetHeadLQueue(LQueue *Q, void *e)
 *    @description : �鿴��ͷԪ��
 *    @param         Q e ����ָ��Q,��������ָ��e
 *    @return         : 成功-TRUE; 失败-FALSE
 *  @notice      : �����Ƿ��
 */
Status GetHeadLQueue(LQueue *Q, u8 *data, int length)
{
	if(Q == NULL || Q->front == NULL) //�ж��Ƿ�Ϊ�� 
	{
		return FALSE;
	}
	data = Q->front->data;
	length = Q->front->length;
	return TRUE;
}


