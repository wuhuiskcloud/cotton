/*********************************************
* @文件: allBleMesh.c
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-7-21
* @概要: 实现蓝牙mesh数据接口
*********************************************/
#include "apll.h"
#include "global.h"
#include "ioctrl.h"
#include "allBleMesh.h"
#include "driverManager.h"
#include "cJSON.h"
#include "endian_swap.h"

//-----------------------------------------------------------------------------
static T_U8 GetCheckSum(unsigned char* buff_in, unsigned char length)
{
	unsigned char i;
	unsigned char check_sum = 0;
	for(i=0;i<length;i++)
	{
		check_sum += buff_in[i];
	}
	ENDIAN_SWAP_Init();
	return check_sum;
}

static T_VOID HeadPacket(T_U8 *pu8HeadPacket, T_U8 *pu8HeadLen)
{
	pu8HeadPacket[0] = HEAD_PACKECT_H;
	pu8HeadPacket[1] = HEAD_PACKECT_L;
	pu8HeadPacket[2] = PROTCOLO_VER;
	*pu8HeadLen = 3;
}

static T_S32 BleMeshPrasePacketHandle(T_U8 *pu8InBuf, T_U8 u8InBufLen, T_U8 *pu8Cmd, T_U8 *pu8DataBuf, T_U8 *pu8DataLen)
{
	T_U8 u8CheckCode = 0;
	//检查头
	if(pu8InBuf[0] != HEAD_PACKECT_H || pu8InBuf[1] != HEAD_PACKECT_L)
	{
		return RET_FAILED;
	}
	//检查版本号
	if(pu8InBuf[2] != PROTCOLO_VER)
	{
		return RET_FAILED;
	}
	//检查校验字
	u8CheckCode = GetCheckSum(pu8InBuf, u8InBufLen-1);
	
	
	if(u8CheckCode != pu8InBuf[u8InBufLen-1])
	{
		return RET_FAILED;
	}

	*pu8Cmd = pu8InBuf[3];
	
	*pu8DataLen = (pu8InBuf[4]<8)| pu8InBuf[5];

	memcpy(pu8DataBuf, &pu8InBuf[6], *pu8DataLen);
	
	return RET_SUCCESS;
	
}

static T_VOID BleMeshAssemblePacketHandle(T_U8 u8Cmd, T_U8 *pu8InBuf, T_U16 u16InBufLen, T_U8 *pu8OutBuf, T_U8 *pu8OutBufLen)
{
	T_U8 u8ConutLen=0;
	T_U8 u8CheckCode = 0;
	
	HeadPacket(pu8OutBuf, &u8ConutLen);  // 包头
	
	pu8OutBuf[u8ConutLen] = u8Cmd;

	u8ConutLen++;
	
	pu8OutBuf[u8ConutLen] = (u16InBufLen >> 8) & 0xff;
	
	pu8OutBuf[u8ConutLen + 1] = (u16InBufLen & 0xff);

	u8ConutLen += 2;
	
	memcpy(&pu8OutBuf[u8ConutLen], pu8InBuf, u16InBufLen);

	u8ConutLen += u16InBufLen;
	
	u8CheckCode = GetCheckSum(pu8OutBuf, u8ConutLen);

	pu8OutBuf[u8ConutLen] = u8CheckCode;

	u8ConutLen++;

	*pu8OutBufLen = u8ConutLen;

}

static T_S32 BleMeshNodeStatusReport(AllHandle *pstAllHandle, T_S8 *ps8Data, T_U8 u8Len)
{
	T_S8 s8MuId[24] = {0};
	cJSON *JasonData = T_NULL;
	T_S8 s8DevSubId[3] = {0};
	T_S8 *ps8JsonFormatData = T_NULL;
	T_S8 s8RepData[128] = {0};
	T_U8 u8GprsWorkMode = 0;	
	
	MDL_DRIVERMGR_Ioctl(pstAllHandle->s32GprsModuleId, GPS_GET_WORK_MODE, (T_VOID *)&u8GprsWorkMode);

	if(u8GprsWorkMode != GPS_DATA_MODE)
	{
		return RET_FAILED;
	}
	
	JasonData = cJSON_CreateObject();

	if(JasonData == T_NULL)
	{
		return RET_FAILED;
	}
	
	//memcpy(pstAllHandle->u8TmpSaveDevData, ps8Data, u8Len);
	
	cJSON_AddNumberToObject(JasonData, "type", TYPE_DEV);  //0:设备 1:网关
	sprintf(s8DevSubId, "%d", ps8Data[0]);
	strcpy(s8MuId, GTW_SN);
	strcat(s8MuId, ":");
	strcat(s8MuId, (const char *)s8DevSubId);
	cJSON_AddStringToObject(JasonData, "muId", s8MuId);
	
	if(ps8Data[1] != 0)
	{
		cJSON_AddNumberToObject(JasonData, "status", NODE_ON_LINE);
	}else
	{
		cJSON_AddNumberToObject(JasonData, "status", NODE_OFF_LINE);
	}
	

	cJSON_AddNumberToObject(JasonData, "moveCount", ps8Data[3]); // 0:正常，1:离线，2:在线，3:未初始化（未绑定用户），4:电池低于10%，5:被挪动',
	
	ps8JsonFormatData = cJSON_PrintUnformatted(JasonData);
	strcpy(s8RepData, ps8JsonFormatData);
	strcat(s8RepData, "\r\n");

	MDL_DRIVERMGR_Write(pstAllHandle->s32GprsModuleId, s8RepData, strlen(s8RepData));

	cJSON_Delete(JasonData);
	free(ps8JsonFormatData);

	ps8JsonFormatData = T_NULL;
	JasonData = T_NULL;

	return RET_SUCCESS;	
}

//获取所有网络节点数据
static T_S32 BleMeshGetAllNode(AllHandle *pstAllHandle)
{
	T_S8 s8TxToMeshData[12] = {0};
	T_U8 u8TxLen =0;
	//55 AA 00 07 00 00 06
	T_U8 u8DataValue[2] = {0};
	
	BleMeshAssemblePacketHandle(MESH_GET_ALL_NODE_CMD, u8DataValue, 0, (T_U8 *)s8TxToMeshData, &u8TxLen);
	
	return MDL_DRIVERMGR_Write(pstAllHandle->s32BleMeshModuleId, s8TxToMeshData, u8TxLen);
}


T_VOID ALL_BleMeshHandle(AllHandle *pstAllHandle, T_S8 *ps8InData,  T_U8 u8InLen)
{
	T_U8 u8Cmd = 0;
	T_U8 u8DataBuf[64] = {0};
	T_U8 u8DataLen =0;
	
	BleMeshPrasePacketHandle((T_U8 *)ps8InData, u8InLen, &u8Cmd, u8DataBuf, &u8DataLen);

	switch(u8Cmd)
	{
		case MESH_HERAT_ACK_CMD:
		{
			if(u8DataBuf[0] == 0x00)  //协调器重启
			{
				pstAllHandle->u8SysTate = SYSTEM_STATE_RUNNING;
			}else if(u8DataBuf[0] == 0x01)  //协调器已经开机
			{
				pstAllHandle->u8SysTate = SYSTEM_STATE_RUN; 
				MDL_TIMER_Start(&pstAllHandle->stTimerHead, MESHHEART_RX_TIMER_TOK, MESH_RX_HEART_TIME);
			}
		}	
		break;

		case MESH_ONLINE_ACK_CMD:  //在线更新状态
		{
			BleMeshNodeStatusReport(pstAllHandle, (char *)u8DataBuf, u8DataLen);
		}break;

		default:break;
	}

}

T_VOID ALL_BleMeshTxHeartbeat(T_VOID *pvAllHandle)
{
	T_S8 s8TxToMeshData[24] = {0};
	T_U8 u8TxLen =0;
	T_U8 u8DataValue[3]= {0x00, 0x00, 0x00};
	
	AllHandle *pstAllHandle =(AllHandle *)pvAllHandle;
	
	// {0x55,0xaa,0x00,	0x00,0x00,0x00,	0xff};
	BleMeshAssemblePacketHandle(MESH_HEART_REQ_CMD, u8DataValue, 0, (T_U8 *)s8TxToMeshData, &u8TxLen);

	MDL_DRIVERMGR_Write(pstAllHandle->s32BleMeshModuleId, s8TxToMeshData, u8TxLen);
}


T_VOID ALL_BleMeshHeartbeatTimeout(T_VOID *pvAllHandle)
{
	T_U8 u8TxLen =0;
	AllHandle *pstAllHandle =(AllHandle *)pvAllHandle;
	T_S8 u8DevSn[24] = {0};
	cJSON *JasonData = T_NULL;
	T_S8 *ps8JsonFormatData = T_NULL;
	T_S8 s8RepData[64] = {0};
	T_U8 u8GprsWorkMode = 0;

	MDL_DRIVERMGR_Ioctl(pstAllHandle->s32GprsModuleId, GPS_GET_WORK_MODE, (T_VOID *)&u8GprsWorkMode);

	if(u8GprsWorkMode != GPS_DATA_MODE)
	{
		return;
	}
	
	JasonData = cJSON_CreateObject();
	if(JasonData == T_NULL)
	{
		return;
	}

	cJSON_AddNumberToObject(JasonData, "type", TYPE_DEV);
	strcpy(u8DevSn, GTW_SN);
	strcat(u8DevSn, ":999");        //999代表设备未收到协调器心跳，
	cJSON_AddStringToObject(JasonData, "muId", u8DevSn);
	cJSON_AddNumberToObject(JasonData, "status", NODE_ALL_OFF_LINE);
	ps8JsonFormatData = cJSON_PrintUnformatted(JasonData);
	strcpy(s8RepData, ps8JsonFormatData);
	strcat(s8RepData, "\r\n");
    cJSON_Delete(JasonData);

    JasonData = T_NULL;
	u8TxLen = strlen(s8RepData);
	MDL_DRIVERMGR_Write(pstAllHandle->s32GprsModuleId, s8RepData, u8TxLen);
	free(ps8JsonFormatData);
	ps8JsonFormatData = T_NULL;
}







