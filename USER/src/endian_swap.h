/*******************************************************************************
Server manager system thread debugging and request response control.
Copyright (C) 2014.12.08  Shenzhen Lorent Network Information Technology co., LTD
Author: Mr.fang <fangjunheng@cn.lorentnetworks.com>
*******************************************************************************/

#ifndef _ENDIAN_SWAP__H__
#define _ENDIAN_SWAP__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "baseType.h"

	/********************************************
	 *fun:no;
	 *in :no
	 *out:Big=1, Little=0;
	 *ret:module string
	 *notice:
	 ********************************************/
	T_BOOL ENDIAN_SWAP_Init(T_VOID);

	/********************************************
	 *fun:Big Little Swap;
	 *in :16bit
	 *out:Big Little Swap 16bit;
	 *ret:module string
	 *notice:
	 ********************************************/
	T_U16 ENDIAN_SWAP16_BigLittle(T_U16 u16H);

	/********************************************
	 *fun:Big Little Swap;
	 *in :32bit
	 *out:Big Little Swap 32bit;
	 *ret:module string
	 *notice:
	 ********************************************/
	T_U32 ENDIAN_SWAP32_BigLittle(T_U32 u32H);

	/********************************************
	 *fun:Big Little Swap;
	 *in :64bit
	 *out:Big Little Swap 64bit;
	 *ret:module string
	 *notice:
	 ********************************************/
	T_U64 ENDIAN_SWAP64_BigLittle(T_U64 u64H);

	/********************************************
	 *fun:Big Little Swap;
	 *in :32bit float
	 *out:Big Little Swap 32bit;
	 *ret:module string
	 *notice:
	 ********************************************/
	T_F32 ENDIAN_FSWAP32_BigLittle(T_F32 f32H);

	/********************************************
	 *功能:组字节流包;
	 *输入:
	 *输出:无;
	 *返回:成功返回LNT_SUCCESS,失败返回LNT_FAILED
	 *注意:2字节长度+内容
	 ********************************************/
	T_S32 ENDIAN_EncordStream(T_S8 *pInBuf, T_U16 *pu16InLen, T_S8 *pOutBuf);

	/********************************************
	 *功能:解析字节流包;
	 *输入:
	 *输出:无;
	 *返回:成功返回内容长度,失败返回0
	 *注意:2字节长度+内容
	 ********************************************/
	T_S32 ENDIAN_DecordStream(T_S8 *pInBuf, T_S8 *pOutBuf, T_S32 s32OutLen);


#ifdef __cplusplus
}
#endif

#endif//_ENDIAN_SWAP__H__

