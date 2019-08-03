/*********************************************
* @文件: timer.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-5-25
* @概要: 提供定时器小工具接口
*********************************************/

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus

#include "baseType.h"
#include "list.h"

typedef T_VOID (*TimeoutHandel)(T_VOID *pvArg);
typedef T_VOID (*DeleteHandel)(T_VOID *pvArg);

typedef struct tagTimerHead
{	
	struct list_head stTimerHead;
}TimerHead;

/********************************************
 *功能：创建一个定时器链表头
 *输入：定时器链表头信息
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_TIMER_Init(TimerHead *pstTimerHead);
/********************************************
 *功能：添加一个定时器
 *输入：定时器链表头、定时器标
                   识、定时器运行次数，间
                   隔时间、超时handle、参数
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:运行次数为0表示永久运行，
              >0表示运行N次。
*********************************************/
T_S32 MDL_TIMER_Add(TimerHead *pstTimerHead, T_U8 u8Tok, T_U8 u8RunCount, TimeoutHandel fTimeoutHandle, 
								DeleteHandel fDeleteHandle, T_VOID *vDate);
/********************************************
 *功能：添加一个定时器
 *输入：定时器链表头、定时器标
 *输出：无.
 *条件：无
 *返回：附带私有数据
 注意:无。
*********************************************/
T_VOID *MDL_TIMER_GetPrivateData(TimerHead *pstTimerHead, T_U8 u8Tok);
/********************************************
 *功能：根据TOK，启动一个定时器
 *输入：定时器链表头、定时器标
 *输出：无.
 *条件：无
 *返回：成功返回0失败返回-1
 注意:无。
*********************************************/
T_S32 MDL_TIMER_Start(TimerHead *pstTimerHead, T_U8 u8Tok, T_S32 s32InterValMs);
/********************************************
 *功能：根据TOK，停止一个定时器
 *输入：定时器链表头、定时器标
 *输出：无.
 *条件：无
 *返回：成功返回0失败返回-1
 注意:无。
*********************************************/
T_S32 MDL_TIMER_Stop(TimerHead *pstTimerHead, T_U8 u8Tok);
/********************************************
 *功能：创建一个定时器
 *输入：定时器链表头、当前时间
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_TIMER_Active(TimerHead *pstTimerHead, T_U32 u32CurTime);
/********************************************
 *功能：删除某定时器
 *输入：定时器链表头，定时器标识
 *输出：无.
 *条件：无
 *返回：成功返回0失败返回-1
 注意:无。
*********************************************/
T_S32 MDL_TIMER_Delete(TimerHead *pstTimerHead, T_U8 u8Tok);
/********************************************
 *功能：去初始化定时器链表
 *输入：定时器链表头
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_TIMER_UnInit(TimerHead *pstTimerHead);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end TIMER_H

