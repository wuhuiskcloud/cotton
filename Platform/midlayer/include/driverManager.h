/*********************************************
* @文件: driverManager.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2017-11-14
* @概要: 抽象驱动层的调用接口，
                   为实现设备驱动和中间层
                   的联系，方便中间层管理
                   设备，包括读写等操作。
*********************************************/
#ifndef _DRIVER_MANAGER_H
#define _DRIVER_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus

#include "baseType.h"

/********************************************
 *功能：初始化驱动管理模块
 *输入：无
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_Init(T_VOID);
/********************************************
 *功能：驱动管理模块任务处理
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRIVERMGR_Task(T_VOID);
/********************************************
 *功能：打开驱动模块
 *输入：模块名
 *输出：无.
 *条件：无
 *返回：模块ID
 注意:当小于0时为失败。
*********************************************/
T_S32 MDL_DRIVERMGR_Open(T_S8 *ps8Name);
/********************************************
 *功能：读驱动模块
 *输入：模块ID、数据buffer、buffer长度
 *输出：无.
 *条件：无
 *返回：读出的数据大小
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_Read(T_S32 s32ModId, T_S8 *ps8Data, T_S32 s32InLen);
/********************************************
 *功能：写驱动模块
 *输入：模块ID、数据buffer、数据长度
 *输出：无.
 *条件：无
 *返回：写入的数据大小
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_Write(T_S32 s32ModId, T_S8 *ps8Data, T_S32 s32WriteLen);
/********************************************
 *功能：模块读写信号检测
 *输入：模块描述符
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_Pop(T_S32 s32ModId);
/********************************************
 *功能：模块读写信号检测
 *输入：模块描述符
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_Ioctl(T_S32 s32ModId, T_S32 s32Cmd, T_VOID *pvData);
/********************************************
 *功能：关闭驱动模块
 *输入：模块描述符
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_Close(T_S32 s32ModId);
/********************************************
 *功能：获取系统时钟
 *输入：时钟计数
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRIVERMGR_GetTimeTick(T_U32 *pu32Tick);
/********************************************
 *功能：获取RTC
 *输入：RTC
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_GetRtc(T_S8 *ps8RtcBuf);
/********************************************
 *功能：设置RTC
 *输入：RTC
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_SetRtc(T_S8 *ps8RtcBuf);
/********************************************
 *功能：设置RTC定时时间
 *输入：超时时间
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_SetRtcTimer(T_U32 u32Time);
/********************************************
 *功能：重启
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRIVERMGR_Reboot(T_VOID);
/********************************************
 *功能：休眠
 *输入：休眠模式
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRIVERMGR_Dormant(T_U8 u8Modle);
/********************************************
 *功能：延时
 *输入：延时时间
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRIVERMGR_Delay(T_U32 u32Ms);
/********************************************
 *功能：去初始化驱动管理模块
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRIVERMGR_DeInit(T_VOID);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end _DRIVER_MANAGER_H
