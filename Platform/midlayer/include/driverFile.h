/*********************************************
* @文件: driverFile.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2017-11-14
* @概要:  抽象驱动文件的操作接口。
			每一个驱动模块都是一个
			文件模块，在每一个驱动
			模块里面实现文件调用接
			口。在上层应用可以独立
			调用这些接口实现对外设
			的操作。
*********************************************/
#ifndef DRIVER_FILE_H
#define DRIVER_FILE_H
	
#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus

#include "baseType.h"

#define MODEL_NAME_LEN 12

typedef struct tagFileOperation
{
	T_S32 (*FileOpen)(T_VOID);
	T_S32 (*FileRead)(T_S8 *ps8Data, T_S32 s32InLen);
	T_S32 (*FileWrite)(T_S8 *ps8Data, T_S32 s32WriteLen);
	T_S32 (*FileClose)(T_VOID);
	T_S32 (*FileIoctl)(T_S32 s32Cmd, T_VOID *pvData);
	T_S32 (*FilePop)(T_VOID);
	T_VOID (*FileRelease)(T_VOID);

	T_S8 s8ModelName[MODEL_NAME_LEN];
}FileOperation;

/********************************************
 *功能：初始化驱动文件
 *输入：无
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
*********************************************/
T_S32 MDL_FILE_Init(T_VOID);
/********************************************
 *功能：在文件管理表中加入一个
                   驱动模块
 *输入：驱动文件模块
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
*********************************************/
T_S32 MDL_FILE_AddModel(FileOperation *pstFileModel);
/********************************************
 *功能：获取文件驱动模块
 *输入：驱动文件模块ID
 *输出：无.
 *条件：无
 *返回：驱动文件模块
 注意:无。
*********************************************/
FileOperation * MDL_FILE_GetModel(T_S32 s32ModelId);
/********************************************
 *功能：获取文件驱动模块
 *输入：驱动文件模块ID
 *输出：无.
 *条件：无
 *返回：驱动文件模块
 注意:无。
*********************************************/
T_S32 MDL_FILE_GetModelId(T_S8 *ps8Name);
/********************************************
 *功能：去初始化驱动文件
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_FILE_DeInit(T_VOID);


#ifdef __cplusplus
}
#endif //end __cplusplus

#endif // end FILE_H
