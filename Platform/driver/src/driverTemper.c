/*********************************************
* @�ļ�: driverTemper.c
* @����: WuHui
* @�汾: v1.0.1
* @ʱ��: 2019-5-27
* @��Ҫ: MCU�ڲ��¶ȴ����������ӿڵ�ʵ��
*********************************************/
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "stm32f10x_rcc.h"

#include "baseType.h"
#include "ioctrl.h"
#include "driverTime.h"
#include "driverTimer.h"
#include "driverFile.h"


/********************************************
 *���ܣ��¶�ģ��IO���� ��ʼ��
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/


/********************************************
 *���ܣ��¶ȴ�����ģ���ϵ��Լ�
 *���룺��
 *�������
 *��������
 *���أ�0 ��ȷ -1 ����
 ע��:
*********************************************/
/*static T_S32 Temper_SelfChk(T_VOID)
{
	return 0;
}
*/

/********************************************
 *���ܣ��򿪺���
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
 ע��:
*********************************************/
static T_S32 _Temper_Open(T_VOID)
{

			
	return RET_SUCCESS;
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
static T_S32 _Temper_Read(T_S8 *ps8DataBuf, T_S32 s32BufLen)
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
static T_S32 _Temper_Pop(T_VOID)
{	
	return RET_SUCCESS;
}
/********************************************
 *���ܣ����ƽӿ�
 *���룺s32Cmd :��������
 		pvData:�������ͬ�������Ͳ�ͬ

 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
*********************************************/
static T_S32 _Temper_Ioctl(T_S32 s32Cmd, T_VOID *pvData)
{
	
	return RET_SUCCESS;
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
static T_S32 _Temper_Close(T_VOID)
{


	return RET_SUCCESS;
}
/************************************************
 *���ܣ����¶ȴ���������ģ����ӵ�
 			�����б��С�
 *���룺��
 *�������
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:
*************************************************/
T_S32 DRIVER_Temper_Init(T_VOID)
{
	FileOperation stOperation;

	/* �������ģ�� */
	stOperation.FileOpen = _Temper_Open;
	stOperation.FileRead = _Temper_Read;
	stOperation.FileWrite = T_NULL;
	stOperation.FileClose = _Temper_Close;
	stOperation.FileIoctl = _Temper_Ioctl;
	stOperation.FilePop = _Temper_Pop;
	stOperation.FileRelease = T_NULL;
	memset(stOperation.s8ModelName, 0, MODEL_NAME_LEN);
	memcpy(stOperation.s8ModelName, "TMP", strlen("TMP"));
	
	return MDL_FILE_AddModel((FileOperation*)&stOperation);
}



