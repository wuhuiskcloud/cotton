/*********************************************
* @�ļ�: driverManager.c
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-5-25
* @��Ҫ: ����������ĵ��ýӿڣ�
                   Ϊʵ���豸�������м��
                   ����ϵ�������м�����
                   �豸��������д�Ȳ�����
*********************************************/

#include "driverManager.h"
#include "driverTime.h"
#include "driverTimer.h"
#include "driverRtc.h"
#include "driverFile.h"
#include "driverFlash.h"
#include "module.h"

/********************************************
 *���ܣ���ʼ����������ģ��
 *���룺��
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Init(T_VOID)
{
	ASSERT_FUN_FAILED_RETVAL(MDL_TIME_Init(), RET_SUCCESS, RET_FAILED);
	ASSERT_FUN_FAILED_RETVAL(MDL_DRVTIMER_Init(), RET_SUCCESS, RET_FAILED);
	ASSERT_FUN_FAILED_RETVAL(MDL_FILE_Init(), RET_SUCCESS, RET_FAILED);
	ASSERT_FUN_FAILED_RETVAL(MDL_RTC_Init(), RET_SUCCESS, RET_FAILED);
	MODULES_INIT();
	return RET_SUCCESS;
}

/********************************************
 *���ܣ���������ģ��������
 *���룺��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRIVERMGR_Task(T_VOID)
{
	MDL_DRVTIMER_Active();
}

/********************************************
 *���ܣ�������ģ��
 *���룺ģ����
 *�������.
 *��������
 *���أ�ģ��ID
 ע��:��С��0ʱΪʧ�ܡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Open(T_S8 *ps8Name)
{
	T_S32 s32ModId = MDL_FILE_GetModelId(ps8Name);

	if(s32ModId >= 0)
	{
		FileOperation *pstFileModel =  MDL_FILE_GetModel(s32ModId);
	
		ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, -1);

		if(pstFileModel->FileOpen)
		{
			if(RET_SUCCESS != pstFileModel->FileOpen())
			{
				return -1;
			}
		}
	}

	return s32ModId;
}

/********************************************
 *���ܣ�������ģ��
 *���룺ģ��ID������buffer��buffer����
 *�������.
 *��������
 *���أ����������ݴ�С
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Read(T_S32 s32ModId, T_S8 *ps8Data, T_S32 s32InLen)
{
	FileOperation *pstFileModel =  MDL_FILE_GetModel(s32ModId);
	
    if(s32ModId < 0)
    {
        return RET_FAILED;
    }
    
	ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pstFileModel->FileRead, T_NULL, 0);

	return pstFileModel->FileRead(ps8Data, s32InLen);
}

/********************************************
 *���ܣ�д����ģ��
 *���룺ģ��ID������buffer�����ݳ���
 *�������.
 *��������
 *���أ�д������ݴ�С
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Write(T_S32 s32ModId, T_S8 *ps8Data, T_S32 s32WriteLen)
{
	FileOperation *pstFileModel =  MDL_FILE_GetModel(s32ModId);
	
	if(s32ModId < 0)
    {
        return RET_FAILED;
    }
	
	ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pstFileModel->FileWrite, T_NULL, 0);

	return pstFileModel->FileWrite(ps8Data, s32WriteLen);
}


/********************************************
 *���ܣ�ģ���д�źż��
 *���룺ģ��������
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Pop(T_S32 s32ModId)
{
	FileOperation *pstFileModel =  MDL_FILE_GetModel(s32ModId);
	
	if(s32ModId < 0)
    {
        return RET_FAILED;
    }
	
	ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pstFileModel->FilePop, T_NULL, RET_FAILED);

	return pstFileModel->FilePop();
}

/********************************************
 *���ܣ�ģ���д�źż��
 *���룺ģ��������
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Ioctl(T_S32 s32ModId, T_S32 s32Cmd, T_VOID *pvData)
{
	FileOperation *pstFileModel =  MDL_FILE_GetModel(s32ModId);
    
	if(s32ModId < 0)
    {
        return RET_FAILED;
    }
    
	ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pstFileModel->FileIoctl, T_NULL, RET_FAILED);

	return pstFileModel->FileIoctl(s32Cmd, pvData);
}

/********************************************
 *���ܣ��ر�����ģ��
 *���룺ģ��������
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Close(T_S32 s32ModId)
{
	FileOperation *pstFileModel =  MDL_FILE_GetModel(s32ModId);
	
	if(s32ModId < 0)
    {
        return RET_FAILED;
    }
	
	ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pstFileModel->FileClose, T_NULL, RET_FAILED);

	return pstFileModel->FileClose();
}

/********************************************
 *���ܣ���ȡϵͳʱ��
 *���룺ʱ�Ӽ���
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRIVERMGR_GetTimeTick(T_U32 *pu32Tick)
{	
	TimeTick stTimeTick;
	MDL_TIME_GetTimeTick(&stTimeTick);

	*pu32Tick = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec;
}

/********************************************
 *���ܣ���ȡRTC
 *���룺RTC
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_GetRtc(T_S8 *ps8RtcBuf)
{	
	return MDL_RTC_Read(ps8RtcBuf);
}
/********************************************
 *���ܣ�����RTC
 *���룺RTC
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_SetRtc(T_S8 *ps8RtcBuf)
{
	return MDL_RTC_Write(ps8RtcBuf);
}

/********************************************
 *���ܣ�����RTC��ʱʱ��
 *���룺��ʱʱ��
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_SetRtcTimer(T_U32 u32Time)
{
    return MDL_RTC_SetTimer(u32Time);
}

/********************************************
 *���ܣ�����
 *���룺��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRIVERMGR_Reboot(T_VOID)
{
   // REBOOT();
}

/********************************************
 *���ܣ�����
 *���룺����ģʽ
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRIVERMGR_Dormant(T_U8 u8Modle)
{
    //DORMANT(u8Modle);
}

/********************************************
 *���ܣ���ʱ
 *���룺��ʱʱ��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRIVERMGR_Delay(T_U32 u32Ms)
{
    MDL_TIME_Delay(u32Ms);
}

/********************************************
 *���ܣ�ȥ��ʼ����������ģ��
 *���룺��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRIVERMGR_DeInit(T_VOID)
{
	//do board deinit
	MDL_FILE_DeInit();
	MDL_DRVTIMER_DeInit();
}

