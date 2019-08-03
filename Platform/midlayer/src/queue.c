/*********************************************
* @文件: queue.c
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-5-28
* @概要: 队列操作接口
*********************************************/

#include "queue.h"

/********************************************
 *功能：去初始化数据队列.
 *输入：无.
 *输出：无.
 *条件：无
 *返回：成功返回0,失败返回-1
 注意:无
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
 *功能：从列中获取数据.
 *输入：无.
 *输出：无.
 *条件：无
 *返回：成功返回模块句柄,失败返回NULL
 注意:无
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
 *功能：列中添加数据到尾部.
 *输入：无.
 *输出：无.
 *条件：无
 *返回：成功返回LNT_SUCCESS,失败返回LNT_FAILED
 注意:无
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
 *功能：初始化数据队列.
 *输入：无.
 *输出：无.
 *条件：无
 *返回：成功返回0,失败返回-1
 注意:无
********************************************/
T_S32 MDL_QUEUE_Init(QueueHandle *pstQueueHandle)
{
	ASSERT_EQUAL_RETVAL(T_NULL, pstQueueHandle, RET_FAILED);

	pstQueueHandle->pstHead = T_NULL;
	pstQueueHandle->s32Count = 0;

	return RET_SUCCESS;
}

