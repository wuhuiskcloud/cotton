/*********************************************
* @�ļ�: timer.c
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-5-25
* @��Ҫ: �ṩ��ʱ��С���߽ӿ�
*********************************************/
#include "driverManager.h"
#include "timer.h"
#include "list.h"

typedef struct tagTimerInfo
{
	T_U8 u8Tok; //��ʱ����ʶ
	T_U8 u8IsActive; //��ʱ�������־
	T_U8 u8Count;
	T_U32  u32StartTimeMs;//��ʼʱ��
	T_U32  u32IntervalMs;//Ԥ��ִ��ʱ��
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
 *���ܣ�����һ����ʱ������ͷ
 *���룺��ʱ������ͷ��Ϣ
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_TIMER_Init(TimerHead *pstTimerHead)
{
	ASSERT_EQUAL_RETURN(pstTimerHead, T_NULL);
	
	INIT_LIST_HEAD(&pstTimerHead->stTimerHead);
}

/********************************************
 *���ܣ����һ����ʱ��
 *���룺��ʱ������ͷ����ʱ����
                   ʶ����ʱ�����д�������
                   ��ʱ�䡢��ʱhandle������
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:���д���Ϊ0��ʾ�������У�
              >0��ʾ����N�Ρ�
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
			return RET_SUCCESS;     //�Ѿ����ڵĶ�ʱ����־
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
 *���ܣ����һ����ʱ��
 *���룺��ʱ������ͷ����ʱ����
 *�������.
 *��������
 *���أ�����˽������
 ע��:�ޡ�
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
 *���ܣ�����TOK������һ����ʱ��
 *���룺��ʱ������ͷ����ʱ���ꡢ
                   �������ʱ��(MS)
 *�������.
 *��������
 *���أ��ɹ�����0ʧ�ܷ���-1
 ע��:�ޡ�
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
 *���ܣ�����TOK��ֹͣһ����ʱ��
 *���룺��ʱ������ͷ����ʱ����
 *�������.
 *��������
 *���أ��ɹ�����0ʧ�ܷ���-1
 ע��:�ޡ�
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
 *���ܣ�����һ����ʱ��
 *���룺��ʱ������ͷ����ǰʱ��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/

T_VOID MDL_TIMER_Active(TimerHead *pstTimerHead, T_U32 u32CurTime)
{
	struct list_head *pList = NULL;
	struct list_head *pN = NULL;
	
	list_for_each_safe(pList, pN, &pstTimerHead->stTimerHead)
	{
		TimerNode *node = list_entry(pList, TimerNode, list);

		//������������´�0��ʼ���ҵȴ�ʱ�Ӽ��������㡣
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
 *���ܣ�ɾ��ĳ��ʱ��
 *���룺��ʱ������ͷ����ʱ����ʶ
 *�������.
 *��������
 *���أ��ɹ�����0ʧ�ܷ���-1
 ע��:�ޡ�
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
 *���ܣ�ȥ��ʼ����ʱ������
 *���룺��ʱ������ͷ
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
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

