/*********************************************
* @文件: driverIWDog.c
* @作者: wuhuiskt 
* @版本: v1.0.1
* @时间: 2019-05-29
* @概要: 独立看门狗驱动的实现
*********************************************/

//#include "driverIWDog.h"
#include "stm32f10x_iwdg.h"
#include "driverFile.h"

/********************************************
 *功能：独立看门狗初始化
 *输入：无
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 			失败:RET_FAILED 
 注意:
*********************************************/
static T_S32 DRIVER_IWDog_Open(T_VOID)
{
  	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	//启动寄存器读写

	/* 128*937/40 = 3000(ms) ==>   3秒溢出 */
  	IWDG_SetPrescaler(IWDG_Prescaler_128);			//128分频
  	IWDG_SetReload(937);               				//937

	IWDG_ReloadCounter();            				//重启计数器（喂狗）
  	IWDG_Enable();                       			//启动看门狗
	
	return RET_SUCCESS;
}

/********************************************
 *功能：
 *输入：无
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 			失败:RET_FAILED 
 注意:
*********************************************/
T_S32 DRIVER_IWDog_Write(T_S8 *ps8Data, T_S32 s32InLen)
{
	IWDG_ReloadCounter();
	
	return RET_SUCCESS;
}


/************************************************
 *功能：将驱动模块添加到
 			驱动列表中。
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
T_S32 DRIVER_IWDog_Init(T_VOID)
{
	FileOperation stICCardOperation;

	/* 添加驱动模块 */
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


