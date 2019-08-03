/*********************************************
* @�ļ�: driverTimer.c
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-5-27
* @��Ҫ: ��ʱ����������������
                   ��ʱ�ٽӿڡ�����������
                   ģ���г�ʼ��������ĵ�
                   �ȣ�����ģ��������Ӷ�
                   ʱ��������
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
 *���ܣ���ʱ����ʼ��
 *���룺��
 *�������.
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRVTIMER_Init(T_VOID)
{
	return MDL_QUEUE_Init(&g_stTimerQueue);
}

/********************************************
 *���ܣ���Ӷ�ʱ����
 *���룺��ʱ����handle
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
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
 *���ܣ���ʱ������
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:�ޡ�
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
 *���ܣ���ʱ��ȥ��ʼ��
 *���룺��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRVTIMER_DeInit(T_VOID)
{
	(T_VOID)MDL_QUEUE_DeInit(&g_stTimerQueue);
}

