/*********************************************
* @�ļ�: driverPressure.c
* @����: WuHui
* @�汾: v1.0.1
* @ʱ��: 2019-6-1
* @��Ҫ: MCU�ڲ�ѹ�������������ӿڵ�ʵ��
*********************************************/

#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "misc.h"
#include "stm32f10x_rcc.h"

#include "baseType.h"
#include "ioctrl.h"
#include "driverTime.h"
#include "driverTimer.h"
#include "driverFile.h"

/********************************************
 *���ܣ�ѹ��������ģ���ϵ��Լ�
 *���룺��
 *�������
 *��������
 *���أ�0 ��ȷ -1 ����
 ע��:
*********************************************/
/*static T_S32 Pressure_SelfChk(T_VOID)
{

	return RET_FAILED;
}*/


/********************************************
 *���ܣ��򿪺���
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
 ע��:
*********************************************/
static T_S32 Pressure_Open(T_VOID)
{

	return RET_FAILED;
}


/********************************************
 *���ܣ���ȡ����
 *���룺������� �� ���ݳ���
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
 		���߳���
 ע��:
*********************************************/
static T_S32 Pressure_Read(T_S8 *ps8DataBuf, T_S32 s32BufLen)
{
	ASSERT_EQUAL_RETVAL(ps8DataBuf, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(s32BufLen, 0, RET_FAILED);
	
	return RET_FAILED;
}

/********************************************
 *���ܣ��¼���־
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
 ע��:  
*********************************************/
static T_S32 Pressure_Pop(T_VOID)
{
	return RET_FAILED;
}

/********************************************
 *���ܣ����ƽӿ�
 *���룺s32Cmd :��������
 		pvData:�������ͬ�������Ͳ�ͬ
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
*********************************************/
static T_S32 Pressure_Ioctl(T_S32 s32Cmd, T_VOID *pvData)
{

	return RET_FAILED;
}

/********************************************
 *���ܣ��رպ���
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 			ʧ��:RET_FAILED 
 ע��:
*********************************************/
static T_S32 Pressure_Close(T_VOID)
{
	return RET_FAILED;
}

/************************************************
 *���ܣ�����������ģ����ӵ�
 			�����б��С�
 *���룺��
 *�������
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:
*************************************************/
T_S32 DRIVER_Pressure_Init(T_VOID)
{
	FileOperation stOperation;
	/* �������ģ�� */
	stOperation.FileOpen = Pressure_Open;
	stOperation.FileRead = Pressure_Read;
	stOperation.FileWrite = T_NULL;
	stOperation.FileClose = Pressure_Close;
	stOperation.FileIoctl = Pressure_Ioctl;
	stOperation.FilePop = Pressure_Pop;
	stOperation.FileRelease = T_NULL;
	memset(stOperation.s8ModelName, 0, MODEL_NAME_LEN);
	memcpy(stOperation.s8ModelName, "PRESSURE", strlen("PRESSURE"));

	return MDL_FILE_AddModel((FileOperation*)&stOperation);
}

