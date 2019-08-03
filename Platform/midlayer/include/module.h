/*********************************************
* @文件: module.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2017-11-14
* @概要: 驱动模块的初始
*********************************************/
#ifndef __MODULE_H
#define __MODULE_H

#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus

#include "baseType.h"
#include "stm32f10x.h"
#include "driverTemper.h"
#include "driverGpio.h"
#include "DriverPressure.h"
#include "driverIWDog.h"
#include "driverSPIFlash.h"
#include "driverNetWork.h"
#include "driverFlash.h"
#include "driverGprs.h"
#include "driverBleMesh.h"
	
	

/*static inline T_VOID REBOOT(T_VOID)
{
	__SYS_REBOOT();
}

static inline T_VOID DORMANT(T_U8 u8Modle)
{
}
*/
#define  MODULES_INIT() \
{ \
	ASSERT_FUN_FAILED_RETVAL(HardWareInit(), RET_SUCCESS, RET_FAILED);\
	ASSERT_FUN_FAILED_RETVAL(DRIVER_Pressure_Init(), RET_SUCCESS, RET_FAILED);\
	ASSERT_FUN_FAILED_RETVAL(DRIVER_NetWork_Init(), RET_SUCCESS, RET_FAILED); \
	ASSERT_FUN_FAILED_RETVAL(DRIVER_Temper_Init(), RET_SUCCESS, RET_FAILED);\
	ASSERT_FUN_FAILED_RETVAL(DRIVER_FLASH_Init(), RET_SUCCESS, RET_FAILED);\
	ASSERT_FUN_FAILED_RETVAL(DRIVER_Gps_Init(), RET_SUCCESS, RET_FAILED);\
	ASSERT_FUN_FAILED_RETVAL(DRIVER_BleMeshInit(), RET_SUCCESS, RET_FAILED);\
}

//	

/*
#define  MODULES_INIT() \
{ \
	ASSERT_FUN_FAILED_RETVAL(HardWareInit(), RET_SUCCESS, RET_FAILED);\
	//ASSERT_FUN_FAILED_RETVAL(DRIVER_GPIO_Init(), RET_SUCCESS, RET_FAILED); \
	//ASSERT_FUN_FAILED_RETVAL(DRIVER_KEY_Init(), RET_SUCCESS, RET_FAILED);\
	//ASSERT_FUN_FAILED_RETVAL(DRIVER_BLE_Init(), RET_SUCCESS, RET_FAILED); \
	//ASSERT_FUN_FAILED_RETVAL(DRIVER_ICCARD_Init(), RET_SUCCESS, RET_FAILED); \
	ASSERT_FUN_FAILED_RETVAL(DRIVER_SpiFlash_Init(), RET_SUCCESS, RET_FAILED);\
	//ASSERT_FUN_FAILED_RETVAL(DRIVER_DS1302_Init(), RET_SUCCESS, RET_FAILED);\
	//ASSERT_FUN_FAILED_RETVAL(DRIVER_LCD_Init(), RET_SUCCESS, RET_FAILED);\
	//ASSERT_FUN_FAILED_RETVAL(DRIVER_IPCUart_Init(), RET_SUCCESS, RET_FAILED);\
	//ASSERT_FUN_FAILED_RETVAL(DRIVER_IWDog_Init(), RET_SUCCESS, RET_FAILED);\
}

*/

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end __MODULE_H
