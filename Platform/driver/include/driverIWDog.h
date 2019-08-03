/*********************************************
* @文件: driverIWDog.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-05-29
* @概要: 独立看门狗驱动接口
*********************************************/

#ifndef __DRIVER_IWDOG__
#define __DRIVER_IWDOG__
	
#ifdef __cplusplus
	extern "C" {
#endif
	
#include "baseType.h"


/************************************************
 *功能：将驱动模块添加到
 			驱动列表中。
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
T_S32 DRIVER_IWDog_Init(T_VOID);

T_S32 DRIVER_IWDog_Write(T_S8 *ps8Data, T_S32 s32InLen);

#ifdef __cplusplus
}
#endif

#endif


