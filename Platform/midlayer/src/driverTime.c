/*********************************************
* @�ļ�: driverTime.c
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-5-26
* @��Ҫ: ����ʱ�䴦��ӿڣ�ͳ��
                   �δ�ʱ�䡣��ʱ���жϽ�
                   �ڵ��ü�ʱ��
*********************************************/
#include "driverTime.h"

static volatile TimeTick g_stTimeTick;

/*********************************************
 *���ܣ�ʱ���������ʼ��
 *���룺��
 *�������.
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:�ޡ�
**********************************************/
T_S32 MDL_TIME_Init(T_VOID)
{
	g_stTimeTick.s32MSec = 0;
	g_stTimeTick.u32Sec = 0;

	return RET_SUCCESS;
}

/*********************************************
 *���ܣ�ͳ��ʱ�ӵδ���
 *���룺��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_TIME_InterruputHandle(T_VOID)
{	
	//������������¼���
	if((0xFFFFFFFFu - (g_stTimeTick.u32Sec * 1000)) < 1000)
	{
		g_stTimeTick.u32Sec = 0;
		g_stTimeTick.s32MSec = 0;
	}
	
	g_stTimeTick.s32MSec += 1000/HZ;
	
	if(g_stTimeTick.s32MSec >= 1000)
	{
		g_stTimeTick.u32Sec = g_stTimeTick.u32Sec + (g_stTimeTick.s32MSec / 1000);
		g_stTimeTick.s32MSec %= 1000;
	}
}

/*********************************************
 *���ܣ���ȡʱ�ӵδ���
 *���룺��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_TIME_GetTimeTick(TimeTick *pstTimeTick)
{	
	ASSERT_EQUAL_RETURN(pstTimeTick, T_NULL);
	
	pstTimeTick->s32MSec = g_stTimeTick.s32MSec;
	pstTimeTick->u32Sec = g_stTimeTick.u32Sec;
}

/*********************************************
 *���ܣ���ʱ
 *���룺��ʱʱ��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_TIME_Delay(T_U32 u32Ms)
{		
	TimeTick stCurTimeTick = g_stTimeTick;
	
	while(1)
	{
		if(g_stTimeTick.s32MSec + g_stTimeTick.u32Sec*1000 >= stCurTimeTick.s32MSec + stCurTimeTick.u32Sec*1000 + u32Ms)
		{
			return;
		}
	}
}

