/*********************************************
* @文件: timer.c
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-5-25
* @概要: 提供定时器小工具接口
*********************************************/
#include "driverManager.h"
#include "timer.h"
#include "list.h"

typedef struct tagTimerInfo
{
	T_U8 u8Tok; //定时器标识
	T_U8 u8IsActive; //定时器激活标志
	T_U8 u8Count;
	T_U32  u32StartTimeMs;//开始时间
	T_U32  u32IntervalMs;//预定执行时间
	TimeoutHandel fTimeoutHandle;
	DeleteHandel  fDeleteHandle;
	T_VOID *pvData;
}TimerInfo;

typedef struct tagTimerNode
{	
	TimerInfo stTimerInfo;
	struct list_head list;
}TimerNode;

/********************************************
 *功能：创建一个定时器链表头
 *输入：定时器链表头信息
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_TIMER_Init(TimerHead *pstTimerHead)
{
	ASSERT_EQUAL_RETURN(pstTimerHead, T_NULL);
	
	INIT_LIST_HEAD(&pstTimerHead->stTimerHead);
}

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
								DeleteHandel fDeleteHandle, T_VOID *vDate)
{
	struct list_head *pList = NULL;
	struct list_head *pN = NULL;
	
	TimerNode *pstTimerNode = T_NULL;
	
	ASSERT_EQUAL_RETVAL(pstTimerHead, T_NULL, RET_FAILED);
		
	list_for_each_safe(pList, pN, &pstTimerHead->stTimerHead)
	{
		TimerNode *node = list_entry(pList, TimerNode, list);

		if(node->stTimerInfo.u8Tok == u8Tok)
		{
			return RET_SUCCESS;     //已经存在的定时器标志
		}
	}

	pstTimerNode = (TimerNode *)malloc(sizeof(TimerNode));	
	ASSERT_EQUAL_RETVAL(pstTimerNode, T_NULL, RET_FAILED);	
	memset(pstTimerNode, 0, sizeof(TimerNode));

	pstTimerNode->stTimerInfo.u8Tok = u8Tok;
	pstTimerNode->stTimerInfo.u8Count = u8RunCount;
	pstTimerNode->stTimerInfo.u8IsActive = T_FALSE;
	pstTimerNode->stTimerInfo.fTimeoutHandle = fTimeoutHandle;
	pstTimerNode->stTimerInfo.fDeleteHandle = fDeleteHandle;
	pstTimerNode->stTimerInfo.u32StartTimeMs = 0;
	pstTimerNode->stTimerInfo.u32IntervalMs = 0;
	pstTimerNode->stTimerInfo.pvData = T_NULL;

	if(T_NULL != vDate)
	{
		pstTimerNode->stTimerInfo.pvData = vDate;
	}

	list_add(&(pstTimerNode->list), &pstTimerHead->stTimerHead);

	return RET_SUCCESS;
}

/********************************************
 *功能：添加一个定时器
 *输入：定时器链表头、定时器标
 *输出：无.
 *条件：无
 *返回：附带私有数据
 注意:无。
*********************************************/
T_VOID *MDL_TIMER_GetPrivateData(TimerHead *pstTimerHead, T_U8 u8Tok)
{	
	struct list_head *pList = NULL;
	struct list_head *pN = NULL;

	ASSERT_EQUAL_RETVAL(pstTimerHead, T_NULL, T_NULL);
		
	list_for_each_safe(pList, pN, &pstTimerHead->stTimerHead)
	{
		TimerNode *node = list_entry(pList, TimerNode, list);

		if(node->stTimerInfo.u8Tok == u8Tok)
		{
			return node->stTimerInfo.pvData;
		}
	}

	return T_NULL;
}

/********************************************
 *功能：根据TOK，启动一个定时器
 *输入：定时器链表头、定时器标、
                   调整间隔时间(MS)
 *输出：无.
 *条件：无
 *返回：成功返回0失败返回-1
 注意:无。
*********************************************/
T_S32 MDL_TIMER_Start(TimerHead *pstTimerHead, T_U8 u8Tok, T_S32 s32InterValMs)
{	
	struct list_head *pList = NULL;
	struct list_head *pN = NULL;

	T_U32 u32CurTime = 0;
	
	ASSERT_EQUAL_RETVAL(pstTimerHead, T_NULL, RET_FAILED);
		
	list_for_each_safe(pList, pN, &pstTimerHead->stTimerHead)
	{
		TimerNode *node = list_entry(pList, TimerNode, list);

		if(node->stTimerInfo.u8Tok == u8Tok)
		{
			MDL_DRIVERMGR_GetTimeTick(&u32CurTime);
			
			node->stTimerInfo.u8IsActive = T_TRUE;
			node->stTimerInfo.u32StartTimeMs = u32CurTime;
			node->stTimerInfo.u32IntervalMs = s32InterValMs;
			return RET_SUCCESS;
		}
	}

	return RET_FAILED;
}

/********************************************
 *功能：根据TOK，停止一个定时器
 *输入：定时器链表头、定时器标
 *输出：无.
 *条件：无
 *返回：成功返回0失败返回-1
 注意:无。
*********************************************/
T_S32 MDL_TIMER_Stop(TimerHead *pstTimerHead, T_U8 u8Tok)
{	
	struct list_head *pList = NULL;
	struct list_head *pN = NULL;

	ASSERT_EQUAL_RETVAL(pstTimerHead, T_NULL, RET_FAILED);
		
	list_for_each_safe(pList, pN, &pstTimerHead->stTimerHead)
	{
		TimerNode *node = list_entry(pList, TimerNode, list);

		if(node->stTimerInfo.u8Tok == u8Tok)
		{
			node->stTimerInfo.u8IsActive = T_FALSE;

			return RET_SUCCESS;
		}
	}

	return RET_FAILED;
}

/********************************************
 *功能：创建一个定时器
 *输入：定时器链表头、当前时间
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/

T_VOID MDL_TIMER_Active(TimerHead *pstTimerHead, T_U32 u32CurTime)
{
	struct list_head *pList = NULL;
	struct list_head *pN = NULL;
	
	list_for_each_safe(pList, pN, &pstTimerHead->stTimerHead)
	{
		TimerNode *node = list_entry(pList, TimerNode, list);

		//计数溢出，重新从0开始并且等待时钟计数器归零。
		if(0xFFFFFFFFu - u32CurTime < node->stTimerInfo.u32IntervalMs)
		{
			node->stTimerInfo.u32StartTimeMs = 0;
			continue;
		}

		if(T_TRUE == node->stTimerInfo.u8IsActive)
		{	
			if(u32CurTime >= node->stTimerInfo.u32StartTimeMs + node->stTimerInfo.u32IntervalMs)
			{
				if(T_NULL != node->stTimerInfo.fTimeoutHandle)
				{
					node->stTimerInfo.fTimeoutHandle(node->stTimerInfo.pvData);
				}
				
				node->stTimerInfo.u32StartTimeMs = u32CurTime;

				if(node->stTimerInfo.u8Count > 0)
				{
					node->stTimerInfo.u8Count--;

					if(0 == node->stTimerInfo.u8Count)
					{
						if(node->stTimerInfo.fDeleteHandle)
						{
							node->stTimerInfo.fDeleteHandle(node->stTimerInfo.pvData);
						}
						
						list_del(&(node->list));
						free(node);
						node = T_NULL;
						
						continue;
					}
				}
			}
		}
	}
}

/********************************************
 *功能：删除某定时器
 *输入：定时器链表头，定时器标识
 *输出：无.
 *条件：无
 *返回：成功返回0失败返回-1
 注意:无。
*********************************************/
T_S32 MDL_TIMER_Delete(TimerHead *pstTimerHead, T_U8 u8Tok)
{
	struct list_head *pList = NULL;
	struct list_head *pN = NULL;
	
	ASSERT_EQUAL_RETVAL(pstTimerHead, T_NULL, RET_FAILED);
		
	list_for_each_safe(pList, pN, &pstTimerHead->stTimerHead)
	{
		TimerNode *node = list_entry(pList, TimerNode, list);

		if(node->stTimerInfo.u8Tok == u8Tok)
		{
			if(node->stTimerInfo.fDeleteHandle)
			{
				node->stTimerInfo.fDeleteHandle(node->stTimerInfo.pvData);
			}
			
			list_del(&(node->list));
			free(node);
			node = T_NULL;
			
			return RET_SUCCESS;
		}
	}

	return RET_FAILED;
}

/********************************************
 *功能：去初始化定时器链表
 *输入：定时器链表头
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_TIMER_UnInit(TimerHead *pstTimerHead)
{
	struct list_head *pPos = T_NULL;
	TimerNode *node = T_NULL;

	while(!list_empty(&pstTimerHead->stTimerHead))
	{
		pPos = pstTimerHead->stTimerHead.next;
		node = list_entry(pPos, TimerNode, list);

		if(node->stTimerInfo.fDeleteHandle)
		{
			node->stTimerInfo.fDeleteHandle(node->stTimerInfo.pvData);
		}
		
		list_del(pPos);
		free(node);
		node = T_NULL;
	}
}

