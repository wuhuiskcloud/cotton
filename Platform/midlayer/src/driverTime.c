/*********************************************
* @文件: driverTime.c
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-5-26
* @概要: 定义时间处理接口，统计
                   滴答时间。由时钟中断接
                   口调用计时。
*********************************************/
#include "driverTime.h"

static volatile TimeTick g_stTimeTick;

/*********************************************
 *功能：时间计数器初始化
 *输入：无
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
**********************************************/
T_S32 MDL_TIME_Init(T_VOID)
{
	g_stTimeTick.s32MSec = 0;
	g_stTimeTick.u32Sec = 0;

	return RET_SUCCESS;
}

/*********************************************
 *功能：统计时钟滴答数
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_TIME_InterruputHandle(T_VOID)
{	
	//计数器溢出重新计数
	if((0xFFFFFFFFu - (g_stTimeTick.u32Sec * 1000)) < 1000)
	{
		g_stTimeTick.u32Sec = 0;
		g_stTimeTick.s32MSec = 0;
	}
	
	g_stTimeTick.s32MSec += 1000/HZ;
	
	if(g_stTimeTick.s32MSec >= 1000)
	{
		g_stTimeTick.u32Sec = g_stTimeTick.u32Sec + (g_stTimeTick.s32MSec / 1000);
		g_stTimeTick.s32MSec %= 1000;
	}
}

/*********************************************
 *功能：获取时钟滴答数
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_TIME_GetTimeTick(TimeTick *pstTimeTick)
{	
	ASSERT_EQUAL_RETURN(pstTimeTick, T_NULL);
	
	pstTimeTick->s32MSec = g_stTimeTick.s32MSec;
	pstTimeTick->u32Sec = g_stTimeTick.u32Sec;
}

/*********************************************
 *功能：延时
 *输入：延时时间
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_TIME_Delay(T_U32 u32Ms)
{		
	TimeTick stCurTimeTick = g_stTimeTick;
	
	while(1)
	{
		if(g_stTimeTick.s32MSec + g_stTimeTick.u32Sec*1000 >= stCurTimeTick.s32MSec + stCurTimeTick.u32Sec*1000 + u32Ms)
		{
			return;
		}
	}
}

