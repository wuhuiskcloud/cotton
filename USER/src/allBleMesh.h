/*********************************************
* @�ļ�: allBleMesh.h
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-07-21
* @��Ҫ: ʵ������mesh���ݴ���ӿ�
*********************************************/
#ifndef ALL_BLEMESH_H
#define ALL_BLEMESH_H
	
#ifdef __cplusplus
	extern "C" {
#endif//end __cplusplus

#include "baseType.h"




/*
1 ����ͨ��Լ��
�����ʣ� 	9600
����λ�� 	8
��żУ�飺	��
ֹͣλ�� 	1
�������أ�	��

2 ֡��ʽ˵��
�ֶ� ���ȣ�byte�� ˵��
֡ͷ 		2 	�̶�Ϊ 0x55aa
�汾 		1 	������չ��
������ 	1	 ����֡����
���ݳ���2 	���
���� xxxx
У��� 	1 	��֡ͷ��ʼ���ֽ���͵ó��Ľ���� 256 ����
˵�������д��� 1 ���ֽڵ����ݾ����ô��ģʽ���䡣
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
//	MODULE_GET_BLE_STATE = 0x00,		//return  BLE_CONTINUE_RSP ��BLE_RESERT_RSP
	MODULE_GET_SINGLE_ACCELEROMERTER_STATE = 0x80,
	MODULE_GET_SINGLE_ACCELEROMERTER_VALUE = 0x81,
	MODULE_DATA_THROUGH_REQ = 0x08,
	MODULE_GET_NODE_STATE_REQ = 0x07,  //��ȡ���нڵ�����״̬
	MODULE_GET_SYSTEM_TIME_RSP = 0x0c,
	MODULE_GET_LOCAL_TIME_RSP = 0x1c,
	MODULE_NETWORK_ACCESS_RSP = 0x31,
}MODULE_CMD_TYPE_e;

enum {
	MESH_HEART_REQ_CMD = 0x00,  //��������
	MESH_GET_ALL_NODE_CMD = 0x07,
};

enum {
	MESH_HERAT_ACK_CMD = 0x00,
	MESH_ONLINE_ACK_CMD = 0x07,   //����״̬����
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


