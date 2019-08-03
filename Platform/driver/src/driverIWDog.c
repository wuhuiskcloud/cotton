/*********************************************
* @�ļ�: driverIWDog.c
* @����: wuhuiskt 
* @�汾: v1.0.1
* @ʱ��: 2019-05-29
* @��Ҫ: �������Ź�������ʵ��
*********************************************/

//#include "driverIWDog.h"
#include "stm32f10x_iwdg.h"
#include "driverFile.h"

/********************************************
 *���ܣ��������Ź���ʼ��
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 			ʧ��:RET_FAILED 
 ע��:
*********************************************/
static T_S32 DRIVER_IWDog_Open(T_VOID)
{
  	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	//�����Ĵ�����д

	/* 128*937/40 = 3000(ms) ==>   3����� */
  	IWDG_SetPrescaler(IWDG_Prescaler_128);			//128��Ƶ
  	IWDG_SetReload(937);               				//937

	IWDG_ReloadCounter();            				//������������ι����
  	IWDG_Enable();                       			//�������Ź�
	
	return RET_SUCCESS;
}

/********************************************
 *���ܣ�
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 			ʧ��:RET_FAILED 
 ע��:
*********************************************/
T_S32 DRIVER_IWDog_Write(T_S8 *ps8Data, T_S32 s32InLen)
{
	IWDG_ReloadCounter();
	
	return RET_SUCCESS;
}


/************************************************
 *���ܣ�������ģ����ӵ�
 			�����б��С�
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*************************************************/
T_S32 DRIVER_IWDog_Init(T_VOID)
{
	FileOperation stICCardOperation;

	/* �������ģ�� */
	stICCardOperation.FileOpen = DRIVER_IWDog_Open;
	stICCardOperation.FileRead = T_NULL;
	stICCardOperation.FileWrite = DRIVER_IWDog_Write;
	stICCardOperation.FileClose = T_NULL;
	stICCardOperation.FileIoctl = T_NULL;
	stICCardOperation.FilePop = T_NULL;
	stICCardOperation.FileRelease = T_NULL;
	memset(stICCardOperation.s8ModelName, 0, MODEL_NAME_LEN);
	strcpy(stICCardOperation.s8ModelName, "WD");
	
	return MDL_FILE_AddModel((FileOperation*)&stICCardOperation);
}


