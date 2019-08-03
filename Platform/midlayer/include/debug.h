/*********************************************
* @文件: debug.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2017-11-14
* @概要: 调试信息输出
*********************************************/
#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus

#include "baseType.h"

#define PRINT //printf

#define IS_DEBUG 0

#if IS_DEBUG
#define print_debug(...) \
do{ \
	PRINT("[DEBUG] %s %s %d:", __FILE__, __FUNCTION__, __LINE__); \
	PRINT(__VA_ARGS__); \
}while(0)
#else //IS_DEBUG
#define print_debug(...)
#endif //end IS_DEBUG

#if IS_DEBUG
#define print_err(...) \
do{ \
	PRINT("[ERROR] %s %s %d:", __FILE__, __FUNCTION__, __LINE__); \
	PRINT(__VA_ARGS__); \
}while(0)
#else
#define print_err(...)
#endif

#if IS_DEBUG
#define print_msg(...)	PRINT(__VA_ARGS__)
#else //IS_DEBUG
#define print_msg(...)
#endif //end IS_DEBUG


#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end __DEBUG_H

