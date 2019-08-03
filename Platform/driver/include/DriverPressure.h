/*********************************************
* @文件: driverPressure.c
* @作者: WuHui
* @版本: v1.0.1
* @时间: 2019-6-1
* @概要: MCU内部压力传感器驱动接口的实现
*********************************************/

#ifndef DRIVER_PRESSURCE_H
#define DRIVER_PRESSURCE_H
	
#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus

#include "baseType.h"


T_S32 DRIVER_Pressure_Init(T_VOID); //加载压力传感器驱动到驱动管理层中

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif // end DRIVER_PRESSURCE_H

