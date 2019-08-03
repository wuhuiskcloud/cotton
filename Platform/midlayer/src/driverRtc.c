/*********************************************
* @文件: driverRtc.c
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-5-31
* @概要: 提供RTC时钟接口，驱动模
                  块调用相关接口实现RTC时
                  间获取功能。
*********************************************/

#include "driverRtc.h"

static RtcModule g_stRtcModule;

/********************************************
 *功能：初始化RTC模块
 *输入：RTC模块
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
*********************************************/
T_S32 MDL_RTC_Init(T_VOID)
{
	g_stRtcModule.RtcRead = T_NULL;
	g_stRtcModule.RtcWrite = T_NULL;
	
	return RET_SUCCESS;
}

/********************************************
 *功能：添加RTC模块
 *输入：RTC模块
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
*********************************************/
T_S32 MDL_RTC_AddModel(RtcModule *pstRtcModel)
{
	ASSERT_EQUAL_RETVAL(pstRtcModel, T_NULL, RET_FAILED);
	
	memcpy(&g_stRtcModule, pstRtcModel, sizeof(RtcModule));
	
	return RET_SUCCESS;
}

/********************************************
 *功能：获取RTC时间
 *输入：年月日时分秒
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
*********************************************/
T_S32 MDL_RTC_Read(T_S8 *ps8RtcBuf)
{
	if(T_NULL == ps8RtcBuf )
		return RET_FAILED;

	if(g_stRtcModule.RtcRead != T_NULL)
	{
		return g_stRtcModule.RtcRead(ps8RtcBuf);
	}else
	{
		return RET_FAILED;
	}
}

/********************************************
 *功能：设置RTC时间
 *输入：年月日时分秒
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
*********************************************/
T_S32 MDL_RTC_Write(T_S8 *ps8RtcBuf)
{
	if(T_NULL == ps8RtcBuf )
	{
		return RET_FAILED;
	}

	if(g_stRtcModule.RtcWrite != T_NULL)
	{
		return g_stRtcModule.RtcWrite(ps8RtcBuf);
	}else
	{
		return RET_FAILED;
	}
}

/********************************************
 *功能：设置RTC定时时间
 *输入：超时时间
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
*********************************************/
T_S32 MDL_RTC_SetTimer(T_U32 u32Time)
{
	if(g_stRtcModule.RtcSetTimer != T_NULL)
	{
		return g_stRtcModule.RtcSetTimer(u32Time);
	}else
	{
		return RET_FAILED;
	}
}


