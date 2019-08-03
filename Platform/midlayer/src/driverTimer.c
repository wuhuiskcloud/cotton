/*********************************************
* @文件: driverTimer.c
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-5-27
* @概要: 定时器操作定义驱动层
                   定时操接口。在驱动管理
                   模块中初始化和任务的调
                   度，驱动模块里面添加定
                   时器操作。
*********************************************/
#include "driverTimer.h"
#include "driverTime.h"
#include "queue.h"

static  QueueHandle g_stTimerQueue;

static T_S32 _DRVTIMER_AddTimer(TimerHandle *pstTimerHandle)
{
	return MDL_QUEUE_Add(&g_stTimerQueue, (T_VOID *)pstTimerHandle);
}

/********************************************
 *功能：定时器初始化
 *输入：无
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRVTIMER_Init(T_VOID)
{
	return MDL_QUEUE_Init(&g_stTimerQueue);
}

/********************************************
 *功能：添加定时任务
 *输入：定时任务handle
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
********************************************/
T_S32 MDL_DRVTIMER_AddTimer(TimerHandle *pstTimerHandle)
{
	TimerHandle *pstTimer = (TimerHandle *)malloc(sizeof(TimerHandle));
	ASSERT_EQUAL_RETVAL(pstTimerHandle, T_NULL, RET_FAILED);

	ASSERT_EQUAL_RETVAL(pstTimer, T_NULL, RET_FAILED);

	memcpy(pstTimer, pstTimerHandle, sizeof(TimerHandle));
	
	return _DRVTIMER_AddTimer(pstTimer);
}

/********************************************
 *功能：定时任务监控
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:无。
********************************************/
T_VOID MDL_DRVTIMER_Active(T_VOID)
{	
	TimeTick stTimeTick;
	TimerHandle *pstTimerHandle = NULL;
	
	MDL_TIME_GetTimeTick(&stTimeTick);

	pstTimerHandle = (TimerHandle*)MDL_QUEUE_Get(&g_stTimerQueue);

	if(pstTimerHandle == T_NULL)
	{
		return;
	}
	
	if(pstTimerHandle->s32MSec <= stTimeTick.u32Sec*1000 + stTimeTick.s32MSec)
	{
		if(T_NULL != pstTimerHandle->fTimerHandle)
		{
			pstTimerHandle->fTimerHandle(pstTimerHandle->pvData);

			free(pstTimerHandle);
			pstTimerHandle = T_NULL;
		}
	}else
	{
		_DRVTIMER_AddTimer(pstTimerHandle);
	}
}

/********************************************
 *功能：定时器去初始化
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRVTIMER_DeInit(T_VOID)
{
	(T_VOID)MDL_QUEUE_DeInit(&g_stTimerQueue);
}

