/*********************************************
* @文件: allBleMesh.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-07-21
* @概要: 实现蓝牙mesh数据处理接口
*********************************************/
#ifndef ALL_BLEMESH_H
#define ALL_BLEMESH_H
	
#ifdef __cplusplus
	extern "C" {
#endif//end __cplusplus

#include "baseType.h"




/*
1 串口通信约定
波特率： 	9600
数据位： 	8
奇偶校验：	无
停止位： 	1
数据流控：	无

2 帧格式说明
字段 长度（byte） 说明
帧头 		2 	固定为 0x55aa
版本 		1 	升级扩展用
命令字 	1	 具体帧类型
数据长度2 	大端
数据 xxxx
校验和 	1 	从帧头开始按字节求和得出的结果对 256 求余
说明：所有大于 1 个字节的数据均采用大端模式传输。
*/
#define HEAD_PACKECT_H 0x55
#define HEAD_PACKECT_L 0xaa
#define PROTCOLO_VER  0x00

#define NODE_OFF_LINE 1
#define NODE_ON_LINE 0
#define NODE_ALL_OFF_LINE 5

typedef enum{
	BLE_HEARTBEAT_RSP = 0x00,
	BLE_NODE_STATE_RSP = 0x07,
	BLE_DATA_THROUGH_RSP = 0x08,
	BLE_REPORT_STATE_NOTIFY = 0x09,
	BLE_GET_SYSTEM_TIME_REQ = 0x0c,
	BLE_GET_LOCAL_TIME_REQ = 0x1c,
	BLE_ID_REQ = 0x31,
	BLE_NETWORK_ACCESS_REQ = 0x32,
}GATEWAY_CMD_TYPE_e;



typedef enum{
	MODULE_GET_HEARTBEAT_CMD = 0x00,
//	MODULE_GET_BLE_STATE = 0x00,		//return  BLE_CONTINUE_RSP 、BLE_RESERT_RSP
	MODULE_GET_SINGLE_ACCELEROMERTER_STATE = 0x80,
	MODULE_GET_SINGLE_ACCELEROMERTER_VALUE = 0x81,
	MODULE_DATA_THROUGH_REQ = 0x08,
	MODULE_GET_NODE_STATE_REQ = 0x07,  //获取所有节点在线状态
	MODULE_GET_SYSTEM_TIME_RSP = 0x0c,
	MODULE_GET_LOCAL_TIME_RSP = 0x1c,
	MODULE_NETWORK_ACCESS_RSP = 0x31,
}MODULE_CMD_TYPE_e;

enum {
	MESH_HEART_REQ_CMD = 0x00,  //发送心跳
	MESH_GET_ALL_NODE_CMD = 0x07,
};

enum {
	MESH_HERAT_ACK_CMD = 0x00,
	MESH_ONLINE_ACK_CMD = 0x07,   //在线状态更新
	MESH_DATA_ACK_CMD = 0x08,
	MESH_ACK_CMD = 0x09,
};



T_VOID ALL_BleMeshHandle(AllHandle *pstAllHandle, T_S8 *ps8InData,  T_U8 u8InLen);
T_VOID ALL_BleMeshTxHeartbeat(T_VOID *pvAllHandle);
T_VOID ALL_BleMeshHeartbeatTimeout(T_VOID *pvAllHandle);
#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //ALL_BLEMESH_H


