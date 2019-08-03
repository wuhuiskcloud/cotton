#include "endian_swap.h"

/*******************************************************************************
Server manager system thread debugging and request response control.
Copyright (C) 2014.12.08  Shenzhen Lorent Network Information Technology co., LTD
Author: Mr.fang <fangjunheng@cn.lorentnetworks.com>
*******************************************************************************/

typedef T_U16 uint16;
typedef T_L32 uint32;

static T_U8 s_bIsBig = T_FALSE;

// 短整型大小端互换
#define BigLittleSwap16(A)  ((((uint16)(A) & 0xff00) >> 8) | \
                            (((uint16)(A) & 0x00ff) << 8))
// 长整型大小端互换
#define BigLittleSwap32(A)  ((((uint32)(A) & 0xff000000) >> 24) | \
                            (((uint32)(A) & 0x00ff0000) >> 8) | \
                            (((uint32)(A) & 0x0000ff00) << 8) | \
                            (((uint32)(A) & 0x000000ff) << 24))

typedef union intWithFloat
{
	T_S32 m_i32;
	T_F32 m_f32;
} intWithFloat;

/********************************************
 *fun:no;
 *in :no
 *out:Big=1, Little=0;
 *ret:module string
 *notice:
 ********************************************/
T_BOOL ENDIAN_SWAP_Init(T_VOID)
{
	union
	{
		T_U32 i;
		T_U8 s[4];
	} c;

	c.i = 0x12345678;

	if(0x12 == c.s[0])
	{
		s_bIsBig = T_TRUE;
	}
	else
	{
		s_bIsBig = T_FALSE;
	}

	//LVD_INFO("Net %s\n", s_bIsBig ? "Endian Big" : "Endian Little");

	return s_bIsBig;
}

/********************************************
 *fun:Big Little Swap;
 *in :16bit
 *out:Big Little Swap 16bit;
 *ret:module string
 *notice:
 ********************************************/
T_U16 ENDIAN_SWAP16_BigLittle(T_U16 u16H)
{
	if(s_bIsBig)
	{
		return u16H;
	}

	return BigLittleSwap16(u16H);
}

/********************************************
 *fun:Big Little Swap;
 *in :32bit
 *out:Big Little Swap 32bit;
 *ret:module string
 *notice:
 ********************************************/
T_U32 ENDIAN_SWAP32_BigLittle(T_U32 u32H)
{
	if(s_bIsBig)
	{
		return u32H;
	}

	return BigLittleSwap32(u32H);
}

