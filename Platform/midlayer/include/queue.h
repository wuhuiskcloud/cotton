/*********************************************
* @文件: queue.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2017-11-28
* @概要: 队列操作接口
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
 *功能：去初始化数据队列.
 *输入：无.
 *输出：无.
 *条件：无
 *返回：成功返回0,失败返回-1
 注意:无
********************************************/
T_S32 MDL_QUEUE_DeInit(QueueHandle *pstQueueHandle);

/********************************************
 *功能：从列中获取数据.
 *输入：无.
 *输出：无.
 *条件：无
 *返回：成功返回模块句柄,失败返回NULL
 注意:无
********************************************/
T_VOID *MDL_QUEUE_Get(QueueHandle *pstQueueHandle);

/********************************************
 *功能：列中添加数据到尾部.
 *输入：无.
 *输出：无.
 *条件：无
 *返回：成功返回LNT_SUCCESS,失败返回LNT_FAILED
 注意:无
********************************************/
T_S32 MDL_QUEUE_Add(QueueHandle *pstQueueHandle, T_VOID *vData);

/********************************************
 *功能：初始化数据队列.
 *输入：无.
 *输出：无.
 *条件：无
 *返回：成功返回0,失败返回-1
 注意:必须先初始化协议站才能启动.
********************************************/
T_S32 MDL_QUEUE_Init(QueueHandle *pstQueueHandle);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end _QUEUE_H

