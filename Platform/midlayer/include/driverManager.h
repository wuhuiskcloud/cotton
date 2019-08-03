/*********************************************
* @�ļ�: driverManager.h
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2017-11-14
* @��Ҫ: ����������ĵ��ýӿڣ�
                   Ϊʵ���豸�������м��
                   ����ϵ�������м�����
                   �豸��������д�Ȳ�����
*********************************************/
#ifndef _DRIVER_MANAGER_H
#define _DRIVER_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus

#include "baseType.h"

/********************************************
 *���ܣ���ʼ����������ģ��
 *���룺��
 *�������.
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Init(T_VOID);
/********************************************
 *���ܣ���������ģ��������
 *���룺��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRIVERMGR_Task(T_VOID);
/********************************************
 *���ܣ�������ģ��
 *���룺ģ����
 *�������.
 *��������
 *���أ�ģ��ID
 ע��:��С��0ʱΪʧ�ܡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Open(T_S8 *ps8Name);
/********************************************
 *���ܣ�������ģ��
 *���룺ģ��ID������buffer��buffer����
 *�������.
 *��������
 *���أ����������ݴ�С
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Read(T_S32 s32ModId, T_S8 *ps8Data, T_S32 s32InLen);
/********************************************
 *���ܣ�д����ģ��
 *���룺ģ��ID������buffer�����ݳ���
 *�������.
 *��������
 *���أ�д������ݴ�С
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Write(T_S32 s32ModId, T_S8 *ps8Data, T_S32 s32WriteLen);
/********************************************
 *���ܣ�ģ���д�źż��
 *���룺ģ��������
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Pop(T_S32 s32ModId);
/********************************************
 *���ܣ�ģ���д�źż��
 *���룺ģ��������
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Ioctl(T_S32 s32ModId, T_S32 s32Cmd, T_VOID *pvData);
/********************************************
 *���ܣ��ر�����ģ��
 *���룺ģ��������
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_Close(T_S32 s32ModId);
/********************************************
 *���ܣ���ȡϵͳʱ��
 *���룺ʱ�Ӽ���
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRIVERMGR_GetTimeTick(T_U32 *pu32Tick);
/********************************************
 *���ܣ���ȡRTC
 *���룺RTC
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_GetRtc(T_S8 *ps8RtcBuf);
/********************************************
 *���ܣ�����RTC
 *���룺RTC
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_SetRtc(T_S8 *ps8RtcBuf);
/********************************************
 *���ܣ�����RTC��ʱʱ��
 *���룺��ʱʱ��
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_DRIVERMGR_SetRtcTimer(T_U32 u32Time);
/********************************************
 *���ܣ�����
 *���룺��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRIVERMGR_Reboot(T_VOID);
/********************************************
 *���ܣ�����
 *���룺����ģʽ
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRIVERMGR_Dormant(T_U8 u8Modle);
/********************************************
 *���ܣ���ʱ
 *���룺��ʱʱ��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRIVERMGR_Delay(T_U32 u32Ms);
/********************************************
 *���ܣ�ȥ��ʼ����������ģ��
 *���룺��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_DRIVERMGR_DeInit(T_VOID);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end _DRIVER_MANAGER_H