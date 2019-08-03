/*********************************************
* @文件: driverPressure.c
* @作者: WuHui
* @版本: v1.0.1
* @时间: 2019-6-1
* @概要: MCU内部压力传感器驱动接口的实现
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
 *功能：压力传感器模块上电自检
 *输入：无
 *输出：无
 *条件：无
 *返回：0 正确 -1 错误
 注意:
*********************************************/
/*static T_S32 Pressure_SelfChk(T_VOID)
{

	return RET_FAILED;
}*/


/********************************************
 *功能：打开函数
 *输入：无
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 		失败:RET_FAILED 
 注意:
*********************************************/
static T_S32 Pressure_Open(T_VOID)
{

	return RET_FAILED;
}


/********************************************
 *功能：读取函数
 *输入：输出数据 及 数据长度
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 		失败:RET_FAILED 
 		或者长度
 注意:
*********************************************/
static T_S32 Pressure_Read(T_S8 *ps8DataBuf, T_S32 s32BufLen)
{
	ASSERT_EQUAL_RETVAL(ps8DataBuf, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(s32BufLen, 0, RET_FAILED);
	
	return RET_FAILED;
}

/********************************************
 *功能：事件标志
 *输入：无
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 		失败:RET_FAILED 
 注意:  
*********************************************/
static T_S32 Pressure_Pop(T_VOID)
{
	return RET_FAILED;
}

/********************************************
 *功能：控制接口
 *输入：s32Cmd :命令类型
 		pvData:根据命令不同数据类型不同
 *条件：无
 *返回：成功:RET_SUCCESS 
 		失败:RET_FAILED 
*********************************************/
static T_S32 Pressure_Ioctl(T_S32 s32Cmd, T_VOID *pvData)
{

	return RET_FAILED;
}

/********************************************
 *功能：关闭函数
 *输入：无
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 			失败:RET_FAILED 
 注意:
*********************************************/
static T_S32 Pressure_Close(T_VOID)
{
	return RET_FAILED;
}

/************************************************
 *功能：将蓝牙驱动模块添加到
 			驱动列表中。
 *输入：无
 *输出：无
 *条件：无
 *返回：成功0失败-1
 注意:
*************************************************/
T_S32 DRIVER_Pressure_Init(T_VOID)
{
	FileOperation stOperation;
	/* 添加驱动模块 */
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

