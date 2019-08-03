#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "ioctrl.h"
#include "driverUart.h"
#include "driverTime.h"
#include "driverTimer.h"
#include "driverFile.h"

static T_S32 DRIVER_BleMeshOpen(T_VOID)
{
	DRIVER_OpenUart(USART1, 9600, 1);
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
static T_S32 DRIVER_BleMeshRead(T_S8 *ps8Buf, T_S32 s32Len)
{
	return DRIVER_UartRead(USART1, ps8Buf, s32Len);
}

/********************************************
 *���ܣ�д���ݺ���
 *���룺������� �� ���ݳ���
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
 		���߳���
 ע��:
*********************************************/
static T_S32 DRIVER_BleMeshWrite(T_S8 *ps8Buf, T_S32 s32Len)
{
	return DRIVER_UartWrite(USART1, ps8Buf, s32Len);
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

static T_S32 DRIVER_BleMeshPop(T_VOID)
{
	return DRIVER_IsUartData(USART1);
}


/************************************************
 *���ܣ���BleMesh����ģ����ӵ�
 			�����б��С�
 *���룺��
 *�������
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:
*************************************************/
T_S32 DRIVER_BleMeshInit(T_VOID)
{
	FileOperation stOperation;

	stOperation.FileOpen = DRIVER_BleMeshOpen;
	stOperation.FileRead = DRIVER_BleMeshRead;
	stOperation.FileWrite = DRIVER_BleMeshWrite;
	stOperation.FileClose = T_NULL;
	stOperation.FileIoctl = T_NULL;
	stOperation.FilePop = DRIVER_BleMeshPop;
	stOperation.FileRelease = T_NULL;
	memset(stOperation.s8ModelName, 0, MODEL_NAME_LEN);
	strcpy(stOperation.s8ModelName, "BLE_MESH");
	
	return MDL_FILE_AddModel((FileOperation*)&stOperation);
}

