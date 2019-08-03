/*********************************************
* @文件: driverGpio.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-6-1
* @概要: GPIO驱动管理，包括按键、
                  状态灯
*********************************************/

#ifndef DRIVERGPIO_H
#define DRIVERGPIO_H
	
#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus

#include "baseType.h"

T_S32 DRIVER_GPIO_Init(T_VOID); //加载GPIO驱动到驱动管理层中

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif // end DRIVERGPIO_H

