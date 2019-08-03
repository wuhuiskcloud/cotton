/*********************************************
* @文件: driverManager.c
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-5-25
* @概要: 抽象驱动层的调用接口，
                   为实现设备驱动和中间层
                   的联系，方便中间层管理
                   设备，包括读写等操作。
*********************************************/

#include "driverManager.h"
#include "driverTime.h"
#include "driverTimer.h"
#include "driverRtc.h"
#include "driverFile.h"
#include "driverFlash.h"
#include "module.h"

/********************************************
 *功能：初始化驱动管理模块
 *输入：无
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_Init(T_VOID)
{
	ASSERT_FUN_FAILED_RETVAL(MDL_TIME_Init(), RET_SUCCESS, RET_FAILED);
	ASSERT_FUN_FAILED_RETVAL(MDL_DRVTIMER_Init(), RET_SUCCESS, RET_FAILED);
	ASSERT_FUN_FAILED_RETVAL(MDL_FILE_Init(), RET_SUCCESS, RET_FAILED);
	ASSERT_FUN_FAILED_RETVAL(MDL_RTC_Init(), RET_SUCCESS, RET_FAILED);
	MODULES_INIT();
	return RET_SUCCESS;
}

/********************************************
 *功能：驱动管理模块任务处理
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRIVERMGR_Task(T_VOID)
{
	MDL_DRVTIMER_Active();
}

/********************************************
 *功能：打开驱动模块
 *输入：模块名
 *输出：无.
 *条件：无
 *返回：模块ID
 注意:当小于0时为失败。
*********************************************/
T_S32 MDL_DRIVERMGR_Open(T_S8 *ps8Name)
{
	T_S32 s32ModId = MDL_FILE_GetModelId(ps8Name);

	if(s32ModId >= 0)
	{
		FileOperation *pstFileModel =  MDL_FILE_GetModel(s32ModId);
	
		ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, -1);

		if(pstFileModel->FileOpen)
		{
			if(RET_SUCCESS != pstFileModel->FileOpen())
			{
				return -1;
			}
		}
	}

	return s32ModId;
}

/********************************************
 *功能：读驱动模块
 *输入：模块ID、数据buffer、buffer长度
 *输出：无.
 *条件：无
 *返回：读出的数据大小
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_Read(T_S32 s32ModId, T_S8 *ps8Data, T_S32 s32InLen)
{
	FileOperation *pstFileModel =  MDL_FILE_GetModel(s32ModId);
	
    if(s32ModId < 0)
    {
        return RET_FAILED;
    }
    
	ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pstFileModel->FileRead, T_NULL, 0);

	return pstFileModel->FileRead(ps8Data, s32InLen);
}

/********************************************
 *功能：写驱动模块
 *输入：模块ID、数据buffer、数据长度
 *输出：无.
 *条件：无
 *返回：写入的数据大小
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_Write(T_S32 s32ModId, T_S8 *ps8Data, T_S32 s32WriteLen)
{
	FileOperation *pstFileModel =  MDL_FILE_GetModel(s32ModId);
	
	if(s32ModId < 0)
    {
        return RET_FAILED;
    }
	
	ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pstFileModel->FileWrite, T_NULL, 0);

	return pstFileModel->FileWrite(ps8Data, s32WriteLen);
}


/********************************************
 *功能：模块读写信号检测
 *输入：模块描述符
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_Pop(T_S32 s32ModId)
{
	FileOperation *pstFileModel =  MDL_FILE_GetModel(s32ModId);
	
	if(s32ModId < 0)
    {
        return RET_FAILED;
    }
	
	ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pstFileModel->FilePop, T_NULL, RET_FAILED);

	return pstFileModel->FilePop();
}

/********************************************
 *功能：模块读写信号检测
 *输入：模块描述符
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_Ioctl(T_S32 s32ModId, T_S32 s32Cmd, T_VOID *pvData)
{
	FileOperation *pstFileModel =  MDL_FILE_GetModel(s32ModId);
    
	if(s32ModId < 0)
    {
        return RET_FAILED;
    }
    
	ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pstFileModel->FileIoctl, T_NULL, RET_FAILED);

	return pstFileModel->FileIoctl(s32Cmd, pvData);
}

/********************************************
 *功能：关闭驱动模块
 *输入：模块描述符
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_Close(T_S32 s32ModId)
{
	FileOperation *pstFileModel =  MDL_FILE_GetModel(s32ModId);
	
	if(s32ModId < 0)
    {
        return RET_FAILED;
    }
	
	ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pstFileModel->FileClose, T_NULL, RET_FAILED);

	return pstFileModel->FileClose();
}

/********************************************
 *功能：获取系统时钟
 *输入：时钟计数
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRIVERMGR_GetTimeTick(T_U32 *pu32Tick)
{	
	TimeTick stTimeTick;
	MDL_TIME_GetTimeTick(&stTimeTick);

	*pu32Tick = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec;
}

/********************************************
 *功能：获取RTC
 *输入：RTC
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_GetRtc(T_S8 *ps8RtcBuf)
{	
	return MDL_RTC_Read(ps8RtcBuf);
}
/********************************************
 *功能：设置RTC
 *输入：RTC
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_SetRtc(T_S8 *ps8RtcBuf)
{
	return MDL_RTC_Write(ps8RtcBuf);
}

/********************************************
 *功能：设置RTC定时时间
 *输入：超时时间
 *输出：无.
 *条件：无
 *返回：成功0，失败-1
 注意:无。
*********************************************/
T_S32 MDL_DRIVERMGR_SetRtcTimer(T_U32 u32Time)
{
    return MDL_RTC_SetTimer(u32Time);
}

/********************************************
 *功能：重启
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRIVERMGR_Reboot(T_VOID)
{
   // REBOOT();
}

/********************************************
 *功能：休眠
 *输入：休眠模式
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRIVERMGR_Dormant(T_U8 u8Modle)
{
    //DORMANT(u8Modle);
}

/********************************************
 *功能：延时
 *输入：延时时间
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRIVERMGR_Delay(T_U32 u32Ms)
{
    MDL_TIME_Delay(u32Ms);
}

/********************************************
 *功能：去初始化驱动管理模块
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID MDL_DRIVERMGR_DeInit(T_VOID)
{
	//do board deinit
	MDL_FILE_DeInit();
	MDL_DRVTIMER_DeInit();
}

