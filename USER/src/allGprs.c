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
	
	ASSERT_EQUAL_RETVAL(ps8InData, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(ps8OutData, T_NULL, RET_FAILED);
	
	JsonObj = cJSON_Parse(ps8InData);

	if(T_NULL == JsonObj)
	{
		return RET_FAILED;
	}

	JsonData = cJSON_GetObjectItem(JsonObj, "evt");
	
	if(ps8OutData == T_NULL)
	{
		cJSON_Delete(JsonObj);
		JsonObj = T_NULL;
		return RET_FAILED;
	}
	
	*ps8OutData = JsonData->valueint;
	
	cJSON_Delete(JsonObj);
	JsonObj = T_NULL;
	
	return RET_SUCCESS;
}

T_VOID ALL_GprsRxHeartbeatTimeout(T_VOID *pvData)
{
	T_U8 u8GprsWorkMode = 0;
	AllHandle *pstAllHandle = (AllHandle *)pvData;
	
	MDL_DRIVERMGR_Ioctl(pstAllHandle->s32GprsModuleId, GPS_GET_WORK_MODE, (T_VOID *)&u8GprsWorkMode);
	if(u8GprsWorkMode == GPS_USER_MODE)
	{
		return;
	}
	
	MDL_TIMER_Stop(&pstAllHandle->stTimerHead, GATEWAY_RX_TIMER_TOK);
	MDL_DRIVERMGR_Ioctl(pstAllHandle->s32GprsModuleId, GPS_RECONNERT_MODE, (T_VOID *)&u8GprsWorkMode);
}

T_VOID ALL_GprsServiceHandle(AllHandle *pstAllHandle, T_S8 *ps8InData)
{
	T_S8 s8Cmd = 0;
	
	if(RET_SUCCESS == GprsJsonDataPrase(ps8InData, &s8Cmd))
	{
		switch(s8Cmd)
		{
			case CMD_SERVER_DEV_STATUS_ACK:
			case CMD_SERVER_LOCATION_ACK:
			case CMD_SERVER_HERAT_ACK:
				MDL_TIMER_Start(&pstAllHandle->stTimerHead, GATEWAY_RX_TIMER_TOK, GATEWAY_RX_HEART_TIME);
			break;

			//case CMD_SERVER_DEV_STATUS_ACK:
			//break;
			
			case CMD_SERVER_RQS_STATUS_ACK:
				BleMeshGetAllNode(pstAllHandle);
			break;

			default:break;
		}
	}
}

T_VOID ALL_GprsTxHeartbeat(T_VOID *pvData)
{
	AllHandle *pstAllHandle = (AllHandle *)pvData;
	T_S8 s8GatewayNo[24] = {0};
	cJSON *pJsonRoot = T_NULL;
	T_S8 *ps8HearData = T_NULL;
	T_U8 u8GprsWorkMode = 0;
	T_S8 s8GatewayData[256] = {0};
	static T_U32 u32Battery = 0;
	static T_U8 u8StartupFlag = 0;
	
	ASSERT_EQUAL_RETURN(pstAllHandle, T_NULL);
	
	strcpy(s8GatewayNo, GTW_SN);
	strcat(s8GatewayNo, ":");
	strcat(s8GatewayNo, "NW");

	MDL_DRIVERMGR_Ioctl(pstAllHandle->s32GprsModuleId, GPS_GET_WORK_MODE, (T_VOID *)&u8GprsWorkMode);

	if(u8GprsWorkMode != GPS_DATA_MODE)
	{
		return;
	}
	
	pJsonRoot = cJSON_CreateObject();
	
	if(pJsonRoot == T_NULL)
	{
		return;
	}
	
	cJSON_AddNumberToObject(pJsonRoot, "type", TYPE_GW);
	cJSON_AddStringToObject(pJsonRoot, "muId", s8GatewayNo);
	
	if(u8StartupFlag == 0)
	{
		BleMeshGetAllNode(pstAllHandle);
		//MDL_TIMER_Start(&pstAllHandle->stTimerHead, GATEWAY_RX_TIMER_TOK, GATEWAY_RX_HEART_TIME);

		u8StartupFlag = 1;
		if(strlen(pstAllHandle->s8GatewayLocation) > 0)
		{
			cJSON_AddStringToObject(pJsonRoot, "location", pstAllHandle->s8GatewayLocation);
		}else
		{
			cJSON_AddStringToObject(pJsonRoot, "location", "0");
		}
		
	}else
	{
		u32Battery++;
		u32Battery= u32Battery%100;
		//u32Battery = u32Battery%0x7fffff;
		//sprintf(s8Bat, "%d", u32Battery);
		cJSON_AddNumberToObject(pJsonRoot, "battery", u32Battery);
	}

	cJSON_AddNumberToObject(pJsonRoot, "status", 0);
	
	ps8HearData=cJSON_PrintUnformatted(pJsonRoot);
	if(ps8HearData == NULL)
	{
		cJSON_Delete(pJsonRoot);
		pJsonRoot = T_NULL;
		return;
	}

	strcpy(s8GatewayData, ps8HearData);
	strcat(s8GatewayData,"\r\n");
	printf("json baddy %s len %d\n",s8GatewayData,strlen(s8GatewayData));
	MDL_DRIVERMGR_Write(pstAllHandle->s32GprsModuleId, s8GatewayData, strlen(s8GatewayData));

	free(ps8HearData);
	ps8HearData = T_NULL;
	
	cJSON_Delete(pJsonRoot);
	pJsonRoot = T_NULL;
}




