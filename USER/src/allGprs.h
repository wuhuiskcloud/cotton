/*********************************************
* @文件: allGprs.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-07-24
* @概要: 实现Gprs应用层逻辑处理接口
*********************************************/
#ifndef ALL_GPRS_H
#define ALL_GPRS_H
	
#ifdef __cplusplus
	extern "C" {
#endif//end __cplusplus

#include "baseType.h"

enum{
	CMD_SERVER_HERAT = 0x54,
	CMD_SERVER_DEV_STATUS = 0x50,
};

T_VOID ALL_GprsServiceHandle(AllHandle *pstAllHandle, T_S8 *ps8InData);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //ALL_BLEMESH_H






