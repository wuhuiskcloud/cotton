/*********************************************
* @文件: allGprs.c
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-7-21
* @概要: 实现Gprs应用层逻辑处理接口
*********************************************/
#include "apll.h"
#include "global.h"
#include "ioctrl.h"
#include "allGprs.h"
#include "driverManager.h"
#include "cJSON.h"

/**********************************
后台通信字段
网关"GwSn":"17888888888:21" 
设备DevSN 
电池电量 
************************************/

T_S32 GprsJsonDataPrase(T_S8 *ps8InData, T_S8 *ps8OutData)
{
	cJSON *JsonObj = T_NULL;
	cJSON *JsonData = T_NULL;

	JsonObj = cJSON_Parse(ps8InData);

	if(T_NULL == JsonObj)
	{
		return RET_FAILED;
	}

	JsonData = cJSON_GetObjectItem(JsonObj, "evt");
	
	if(ps8OutData == T_NULL)
	{
		return RET_FAILED;
	}
	
	*ps8OutData = JsonData->valueint;

	return RET_SUCCESS;
}

T_VOID ALL_GprsServiceHandle(AllHandle *pstAllHandle, T_S8 *ps8InData)
{
	
	T_S8 s8Cmd = 0;
	
	if(RET_SUCCESS == GprsJsonDataPrase(ps8InData, &s8Cmd))
	{
		switch(s8Cmd)
		{
			case CMD_SERVER_HERAT:
				
			break;

			default:break;
		}
	}
}




