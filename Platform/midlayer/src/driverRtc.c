/*********************************************
* @�ļ�: driverRtc.c
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-5-31
* @��Ҫ: �ṩRTCʱ�ӽӿڣ�����ģ
                  �������ؽӿ�ʵ��RTCʱ
                  ���ȡ���ܡ�
*********************************************/

#include "driverRtc.h"

static RtcModule g_stRtcModule;

/********************************************
 *���ܣ���ʼ��RTCģ��
 *���룺RTCģ��
 *�������.
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_RTC_Init(T_VOID)
{
	g_stRtcModule.RtcRead = T_NULL;
	g_stRtcModule.RtcWrite = T_NULL;
	
	return RET_SUCCESS;
}

/********************************************
 *���ܣ����RTCģ��
 *���룺RTCģ��
 *�������.
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_RTC_AddModel(RtcModule *pstRtcModel)
{
	ASSERT_EQUAL_RETVAL(pstRtcModel, T_NULL, RET_FAILED);
	
	memcpy(&g_stRtcModule, pstRtcModel, sizeof(RtcModule));
	
	return RET_SUCCESS;
}

/********************************************
 *���ܣ���ȡRTCʱ��
 *���룺������ʱ����
 *�������.
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_RTC_Read(T_S8 *ps8RtcBuf)
{
	if(T_NULL == ps8RtcBuf )
		return RET_FAILED;

	if(g_stRtcModule.RtcRead != T_NULL)
	{
		return g_stRtcModule.RtcRead(ps8RtcBuf);
	}else
	{
		return RET_FAILED;
	}
}

/********************************************
 *���ܣ�����RTCʱ��
 *���룺������ʱ����
 *�������.
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_RTC_Write(T_S8 *ps8RtcBuf)
{
	if(T_NULL == ps8RtcBuf )
	{
		return RET_FAILED;
	}

	if(g_stRtcModule.RtcWrite != T_NULL)
	{
		return g_stRtcModule.RtcWrite(ps8RtcBuf);
	}else
	{
		return RET_FAILED;
	}
}

/********************************************
 *���ܣ�����RTC��ʱʱ��
 *���룺��ʱʱ��
 *�������.
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_RTC_SetTimer(T_U32 u32Time)
{
	if(g_stRtcModule.RtcSetTimer != T_NULL)
	{
		return g_stRtcModule.RtcSetTimer(u32Time);
	}else
	{
		return RET_FAILED;
	}
}


