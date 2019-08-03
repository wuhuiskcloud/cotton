/*********************************************
* @�ļ�: allGprs.c
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-7-21
* @��Ҫ: ʵ��GprsӦ�ò��߼�����ӿ�
*********************************************/
#include "apll.h"
#include "global.h"
#include "ioctrl.h"
#include "allGprs.h"
#include "driverManager.h"
#include "cJSON.h"

/**********************************
��̨ͨ���ֶ�
����"GwSn":"17888888888:21" 
�豸DevSN 
��ص��� 
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




