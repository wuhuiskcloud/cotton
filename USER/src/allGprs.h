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
	CMD_SERVER_HERAT = 0x54,
	CMD_SERVER_DEV_STATUS = 0x50,
};

T_VOID ALL_GprsServiceHandle(AllHandle *pstAllHandle, T_S8 *ps8InData);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //ALL_BLEMESH_H






