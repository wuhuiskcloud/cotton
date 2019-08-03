/*********************************************
* @�ļ�: queue.h
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2017-11-28
* @��Ҫ: ���в����ӿ�
*********************************************/

#ifndef _QUEUE_H
#define _QUEUE_H

#ifdef __cplusplus
extern  "C"
{
#endif //end __cplusplus

#include "baseType.h"

typedef struct tagQueueNode
{
	struct tagQueueNode *pNext;
	T_VOID *vData;
} QueueNode;

typedef struct tagQueueHandle
{
	QueueNode *pstHead;
	T_S32 s32Count;
} QueueHandle;

/********************************************
 *���ܣ�ȥ��ʼ�����ݶ���.
 *���룺��.
 *�������.
 *��������
 *���أ��ɹ�����0,ʧ�ܷ���-1
 ע��:��
********************************************/
T_S32 MDL_QUEUE_DeInit(QueueHandle *pstQueueHandle);

/********************************************
 *���ܣ������л�ȡ����.
 *���룺��.
 *�������.
 *��������
 *���أ��ɹ�����ģ����,ʧ�ܷ���NULL
 ע��:��
********************************************/
T_VOID *MDL_QUEUE_Get(QueueHandle *pstQueueHandle);

/********************************************
 *���ܣ�����������ݵ�β��.
 *���룺��.
 *�������.
 *��������
 *���أ��ɹ�����LNT_SUCCESS,ʧ�ܷ���LNT_FAILED
 ע��:��
********************************************/
T_S32 MDL_QUEUE_Add(QueueHandle *pstQueueHandle, T_VOID *vData);

/********************************************
 *���ܣ���ʼ�����ݶ���.
 *���룺��.
 *�������.
 *��������
 *���أ��ɹ�����0,ʧ�ܷ���-1
 ע��:�����ȳ�ʼ��Э��վ��������.
********************************************/
T_S32 MDL_QUEUE_Init(QueueHandle *pstQueueHandle);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end _QUEUE_H

