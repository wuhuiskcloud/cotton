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
	 *����:���ֽ�����;
	 *����:
	 *���:��;
	 *����:�ɹ�����LNT_SUCCESS,ʧ�ܷ���LNT_FAILED
	 *ע��:2�ֽڳ���+����
	 ********************************************/
	T_S32 ENDIAN_EncordStream(T_S8 *pInBuf, T_U16 *pu16InLen, T_S8 *pOutBuf);

	/********************************************
	 *����:�����ֽ�����;
	 *����:
	 *���:��;
	 *����:�ɹ��������ݳ���,ʧ�ܷ���0
	 *ע��:2�ֽڳ���+����
	 ********************************************/
	T_S32 ENDIAN_DecordStream(T_S8 *pInBuf, T_S8 *pOutBuf, T_S32 s32OutLen);


#ifdef __cplusplus
}
#endif

#endif//_ENDIAN_SWAP__H__

