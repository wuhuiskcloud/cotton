/*********************************************
* @�ļ�: driverIWDog.h
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-05-29
* @��Ҫ: �������Ź������ӿ�
*********************************************/

#ifndef __DRIVER_IWDOG__
#define __DRIVER_IWDOG__
	
#ifdef __cplusplus
	extern "C" {
#endif
	
#include "baseType.h"


/************************************************
 *���ܣ�������ģ����ӵ�
 			�����б��С�
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*************************************************/
T_S32 DRIVER_IWDog_Init(T_VOID);

T_S32 DRIVER_IWDog_Write(T_S8 *ps8Data, T_S32 s32InLen);

#ifdef __cplusplus
}
#endif

#endif


