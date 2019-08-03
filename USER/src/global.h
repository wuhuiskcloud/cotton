/*********************************************
* @文件:global.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-6-1
* @概要: 实现按键增删改查的逻辑
*********************************************/
#ifndef GLOBAL_H
#define GLOBAL_H
	
#ifdef __cplusplus
	extern "C" {
#endif//end __cplusplus

#include "baseType.h"
#include "timer.h"

#define VER		" " //版本信息
#define WD_EN	0	//看门狗使能 方便调试


#define SYSTEM_STATE_INIT 0 //系统初始化
#define SYSTEM_STATE_RUNNING 1 //系统正在启动
#define SYSTEM_STATE_RUN 2 //系统已运行
#define SYSTEM_STATE_NORMAL 3 //系统完全运行

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
	SYS_MODE_SETUP = 0x01, //设置模式
}SysMode;


typedef struct tagAllHandle
{
	T_U8 u8ModelSets; 	//系统的模式标记
	T_U8 u8Quit; 		//系统退出标志
	T_U8 u8SysTate; 	//系统已经运行
	TimerHead stTimerHead; //定时器任务表
	T_S32 s32TemperModuleId; //温度传感器模块操作ID
	T_S32 s32GpioModuleId; //GPIO设备操作ID
	T_S32 s32WdModuleId; //看门狗操作ID
	T_S32 s32PressureModuleId; //压力传感器设备操作ID
	T_S32 s32NetworkModuleId;  //米峰通信模块操作ID
	T_S32 s32McuFlashModuleId; //muc flash 模块操作ID
	T_S32 s32GprsModuleId;    //Gprs 模块操作ID
	T_S32 s32BleMeshModuleId;  //蓝牙mesh模块操作ID
	T_S8 s8GatewayLocation[128];  //网关位置信息
	T_U8 u8BatVal;
	T_U8 u8TmpSaveDevData[24];
	
}AllHandle;


#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //GLOBAL_H

