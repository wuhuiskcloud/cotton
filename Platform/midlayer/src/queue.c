/*********************************************
* @�ļ�: queue.c
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-5-28
* @��Ҫ: ���в����ӿ�
*********************************************/

#include "queue.h"

/********************************************
 *���ܣ�ȥ��ʼ�����ݶ���.
 *���룺��.
 *�������.
 *��������
 *���أ��ɹ�����0,ʧ�ܷ���-1
 ע��:��
********************************************/
T_S32 MDL_QUEUE_DeInit(QueueHandle *pstQueueHandle)
{
	QueueNode *pstHead = T_NULL;

	ASSERT_EQUAL_RETVAL(T_NULL, pstQueueHandle, RET_SUCCESS);

	if(pstQueueHandle->s32Count <= 0)
	{
		return RET_SUCCESS;
	}

	pstHead = pstQueueHandle->pstHead;
	while(pstHead && pstQueueHandle->s32Count > 0)
	{
		QueueNode *pstQueue = pstHead;

		if(pstHead->vData)
		{
			free(pstHead->vData);
			pstHead->vData = T_NULL;
		}
		
		free(pstHead);

		pstHead = pstQueue->pNext;
		pstQueueHandle->s32Count--;
	}

	pstQueueHandle->pstHead = T_NULL;
	pstQueueHandle->s32Count = 0;

	return RET_SUCCESS;
}

/********************************************
 *���ܣ������л�ȡ����.
 *���룺��.
 *�������.
 *��������
 *���أ��ɹ�����ģ����,ʧ�ܷ���NULL
 ע��:��
********************************************/
T_VOID *MDL_QUEUE_Get(QueueHandle *pstQueueHandle)
{
	QueueNode *pstCur = T_NULL;
	T_VOID *vData = T_NULL;

	ASSERT_EQUAL_RETVAL(T_NULL, pstQueueHandle, T_NULL);

	if(pstQueueHandle->s32Count <= 0)
	{
		return T_NULL;
	}

	pstCur = pstQueueHandle->pstHead;
	pstQueueHandle->pstHead = pstQueueHandle->pstHead->pNext;
	pstQueueHandle->s32Count--;

	if(pstCur)
	{
		vData = pstCur->vData;
		free((T_S8 *)pstCur);
		pstCur = T_NULL;
	}

	return vData;
}

/********************************************
 *���ܣ�����������ݵ�β��.
 *���룺��.
 *�������.
 *��������
 *���أ��ɹ�����LNT_SUCCESS,ʧ�ܷ���LNT_FAILED
 ע��:��
********************************************/
T_S32 MDL_QUEUE_Add(QueueHandle *pstQueueHandle, T_VOID *vData)
{
	QueueNode *pstNew = T_NULL;
	QueueNode *pstCur = T_NULL;

	ASSERT_EQUAL_RETVAL(T_NULL, vData, RET_FAILED);
	ASSERT_EQUAL_RETVAL(T_NULL, pstQueueHandle, RET_FAILED);
	
	pstNew = (QueueNode *)malloc(sizeof(QueueNode));
	ASSERT_EQUAL_RETVAL(T_NULL, pstNew, RET_FAILED);

	memset(pstNew, 0, sizeof(QueueNode));

	pstNew->vData = vData;
	pstNew->pNext = T_NULL;

	if(T_NULL == pstQueueHandle->pstHead)
	{
		pstQueueHandle->pstHead = pstNew;
		pstQueueHandle->s32Count = 1;

		return RET_SUCCESS;
	}
	
	pstCur = pstQueueHandle->pstHead;

	while(T_NULL != pstCur->pNext)
	{
		pstCur = pstCur->pNext;
	}

	pstCur->pNext = pstNew;
	pstQueueHandle->s32Count++;

	return RET_SUCCESS;
}

/********************************************
 *���ܣ���ʼ�����ݶ���.
 *���룺��.
 *�������.
 *��������
 *���أ��ɹ�����0,ʧ�ܷ���-1
 ע��:��
********************************************/
T_S32 MDL_QUEUE_Init(QueueHandle *pstQueueHandle)
{
	ASSERT_EQUAL_RETVAL(T_NULL, pstQueueHandle, RET_FAILED);

	pstQueueHandle->pstHead = T_NULL;
	pstQueueHandle->s32Count = 0;

	return RET_SUCCESS;
}

