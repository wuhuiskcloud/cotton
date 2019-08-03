/*********************************************
* @�ļ�:global.h
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-6-1
* @��Ҫ: ʵ�ְ�����ɾ�Ĳ���߼�
*********************************************/
#ifndef GLOBAL_H
#define GLOBAL_H
	
#ifdef __cplusplus
	extern "C" {
#endif//end __cplusplus

#include "baseType.h"
#include "timer.h"

#define VER		" " //�汾��Ϣ
#define WD_EN	0	//���Ź�ʹ�� �������


#define SYSTEM_STATE_INIT 0 //ϵͳ��ʼ��
#define SYSTEM_STATE_RUNNING 1 //ϵͳ��������
#define SYSTEM_STATE_RUN 2 //ϵͳ������
#define SYSTEM_STATE_NORMAL 3 //ϵͳ��ȫ����

#define RESET_4G_TIME 100
#define START_4G_TIME 1000

#define MESH_TX_HEART_TIME 10000
#define MESH_RX_HEART_TIME 13000
#define GATEWAY_TX_HEART_TIME 15000

#define TEST_TIMER_TOK 1
#define MESHHEART_TX_TIMER_TOK 2
#define MESHHEART_RX_TIMER_TOK 3
#define GATEWAY_TX_TIMER_TOK 4


#define GTW_SN 		"JP010002"
#define TYPE_GW 	1
#define TYPE_DEV 	0

typedef enum
{
	SYS_MODE_NONE = 0x00,  
	SYS_MODE_SETUP = 0x01, //����ģʽ
}SysMode;


typedef struct tagAllHandle
{
	T_U8 u8ModelSets; 	//ϵͳ��ģʽ���
	T_U8 u8Quit; 		//ϵͳ�˳���־
	T_U8 u8SysTate; 	//ϵͳ�Ѿ�����
	TimerHead stTimerHead; //��ʱ�������
	T_S32 s32TemperModuleId; //�¶ȴ�����ģ�����ID
	T_S32 s32GpioModuleId; //GPIO�豸����ID
	T_S32 s32WdModuleId; //���Ź�����ID
	T_S32 s32PressureModuleId; //ѹ���������豸����ID
	T_S32 s32NetworkModuleId;  //�׷�ͨ��ģ�����ID
	T_S32 s32McuFlashModuleId; //muc flash ģ�����ID
	T_S32 s32GprsModuleId;    //Gprs ģ�����ID
	T_S32 s32BleMeshModuleId;  //����meshģ�����ID
	T_S8 s8GatewayLocation[128];  //����λ����Ϣ
	T_U8 u8BatVal;
	T_U8 u8TmpSaveDevData[24];
	
}AllHandle;


#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //GLOBAL_H

