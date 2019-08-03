/*********************************************
* @文件: baseType.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2017-11-14
* @概要: 定义进本数据类型
*********************************************/
#ifndef __BASE_TYPE_H
#define __BASE_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif//end __cplusplus

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "assert.h"

#define T_VOID          void
#define T_NULL          (void *)0
#define T_TRUE          1
#define T_FALSE         0

typedef char               T_S8;
typedef unsigned char      T_U8;
typedef short              T_S16;
typedef unsigned short     T_U16;
typedef int                T_S32;
typedef unsigned int       T_U32;
typedef long               T_L32;
typedef unsigned long      T_UL32;
typedef long long          T_S64;
typedef unsigned long long T_U64;
typedef float              T_F32;
typedef double             T_F64;
typedef unsigned char      T_BOOL;

#define RET_SUCCESS 0
#define RET_FAILED -1

#ifdef __cplusplus
}
#endif//end __cplusplus

#endif //end __BASE_TYPE_H
