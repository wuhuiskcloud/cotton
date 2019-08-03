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
 *功能：读取函数
 *输入：输出数据 及 数据长度
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 		失败:RET_FAILED 
 		或者长度
 注意:
*********************************************/
static T_S32 DRIVER_BleMeshRead(T_S8 *ps8Buf, T_S32 s32Len)
{
	return DRIVER_UartRead(USART1, ps8Buf, s32Len);
}

/********************************************
 *功能：写数据函数
 *输入：输出数据 及 数据长度
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 		失败:RET_FAILED 
 		或者长度
 注意:
*********************************************/
static T_S32 DRIVER_BleMeshWrite(T_S8 *ps8Buf, T_S32 s32Len)
{
	return DRIVER_UartWrite(USART1, ps8Buf, s32Len);
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

static T_S32 DRIVER_BleMeshPop(T_VOID)
{
	return DRIVER_IsUartData(USART1);
}


/************************************************
 *功能：将BleMesh驱动模块添加到
 			驱动列表中。
 *输入：无
 *输出：无
 *条件：无
 *返回：成功0失败-1
 注意:
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

