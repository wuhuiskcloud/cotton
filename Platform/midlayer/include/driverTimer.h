/*********************************************
* @文件: driverTimer.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2017-11-14
* @概要: 定时器操作定义驱动层
                   定时操接口。在驱动管理
                   模块中初始化和任务的调
                   度，驱动模块里面添加定
                   时器操作。
*********************************************/
#ifndef DRIVER_TIMER_H
#define DRIVER_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus

#include "baseType.h"

typedef T_VOID (*TIMER_CB_HANDLE)(T_VOID *pvData);

typedef struct tagTimerHandle
{
	T_S32 s32MSec; //超时时间ms，当前系统的滴答数+ 超时时间
	T_VOID *pvData; //超时处理数据
	TIMER_CB_HANDLE fTimerHandle; //超时处理函数
}TimerHandle;

/********************************************
 *功能：定时器初始化
 *输入：无
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRVTIMER_Init(T_VOID);
/********************************************
 *功能：添加定时任务
 *输入：定时任务handle
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
********************************************/
T_S32 MDL_DRVTIMER_AddTimer(TimerHandle *pstTimerHandle);
/********************************************
 *功能：定时任务监控
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:无。
********************************************/
T_VOID MDL_DRVTIMER_Active(T_VOID);
/********************************************
 *功能：定时器去初始化
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRVTIMER_DeInit(T_VOID);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end __TIMER_H
