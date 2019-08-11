/*********************************************
* @�ļ�: allGprs.h
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-07-24
* @��Ҫ: ʵ��GprsӦ�ò��߼�����ӿ�
*********************************************/
#ifndef ALL_GPRS_H
#define ALL_GPRS_H
	
#ifdef __cplusplus
	extern "C" {
#endif//end __cplusplus

#include "baseType.h"

enum{
	CMD_SERVER_LOCATION_ACK = 0x50,
	CMD_SERVER_HERAT_ACK = 0x51,
	CMD_SERVER_DEV_STATUS_ACK = 0x52,
	CMD_SERVER_RQS_STATUS_ACK = 0x70,
};

T_VOID ALL_GprsServiceHandle(AllHandle *pstAllHandle, T_S8 *ps8InData);
T_VOID ALL_GprsTxHeartbeat(T_VOID *pvData);
T_VOID ALL_GprsRxHeartbeatTimeout(T_VOID *pvData);
T_S32 BleMeshGetAllNode(AllHandle *pstAllHandle);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //ALL_BLEMESH_H






