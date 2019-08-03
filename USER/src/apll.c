/*********************************************
* @�ļ�: apll.c
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-6-1
* @��Ҫ: ʵ��Ӧ���߼����ҵ��
                   �ܣ���֯��ÿһ��״̬��
                   ����
*********************************************/
#include "driverManager.h"
#include "apll.h"
#include "global.h"
#include "ioctrl.h"
#include "dataBase.h"
#include "allBleMesh.h"
#include "cJSON.h"
#include "endian_swap.h"
#include "allGprs.h"

#define DEVICE_TMP_NAME "TMP"
#define DEVICE_GPIO_NAME "Gpio" 
#define DEVICE_WD_NAME "WD"
#define DEVICE_PRESSURE_NAME "PRESSURE"
#define DEVICE_NET_NAME "NET"
#define DEVICE_MCU_FLASH_NAME "FLASH"
#define DEVICE_GPRS_NAME		"GPS"
#define DEVICE_BLEMESH_NAME  "BLE_MESH"


//����Ĭ��ϵͳ����
static T_VOID ALL_DefaultSysConfig(T_VOID)
{
	ConfigInfo stConf;
	
	/* ���ϵͳ���������û�о�д��Ĭ�ϲ��� */
	if(RET_FAILED == ALL_DB_GetSysConfig(&stConf))
	{
		
		if(RET_FAILED == ALL_DB_SetConfigDefault())
		{
			print_err("Set Default Sysconfig fail.\r\n");
		}
	}
}

/*
static T_VOID ALL_GpioHandle(AllHandle *pstAllHandle, T_S8 *ps8Data)
{
	GpioCrtl *pstGpioCrt = (GpioCrtl *)ps8Data;

	if(GPIO_RECVY & pstGpioCrt->eType)//�ظ�����
	{
		//ALL_DB_ClearDataTable();
        //ALL_DB_SetConfigDefault();
	}
}*/

static T_VOID ALL_IsResetAllTable(T_U8 u8NewVersion)
{
    ConfigInfo stConf;

    ALL_DB_GetSysConfig(&stConf);
    
    if(stConf.u8SheetVer != u8NewVersion)
    {
        //ALL_DB_DestoryAllTable();

        ALL_DB_Init();
        
        stConf.u8SheetVer = u8NewVersion;
        
        ALL_DB_SetSysConfig(&stConf);

    }
}

static T_VOID ALL_Task(AllHandle *pstAllHandle)
{
	T_U32 u32CurTime = 0;
	T_S8 s8ReadBuf[256] = {0};
	T_S32 s32ReadLen = 0;
    GpioCrtl stGpioCtl;
    T_U8 u8GprsWorkMode = 0;

    memset(&stGpioCtl, 0 ,sizeof(GpioCrtl));
    
	(T_VOID)MDL_DRIVERMGR_GetTimeTick(&u32CurTime);
	(T_VOID)MDL_TIMER_Active(&pstAllHandle->stTimerHead, u32CurTime);

	//����׷�ͨ����û�����ݶ�
	if(RET_SUCCESS == MDL_DRIVERMGR_Pop(pstAllHandle->s32NetworkModuleId))
	{
		memset(s8ReadBuf, 0, sizeof(s8ReadBuf));
		
		s32ReadLen = MDL_DRIVERMGR_Read(pstAllHandle->s32NetworkModuleId, s8ReadBuf, sizeof(s8ReadBuf));
		
		if(s32ReadLen > 0)
		{
			//IPCҵ����
			//ALL_NET_MainHandler((T_VOID *)pstAllHandle, s8ReadBuf);
		}
	}

		//�������ݴ���
	if(RET_SUCCESS == MDL_DRIVERMGR_Pop(pstAllHandle->s32GprsModuleId))
	{
		memset(s8ReadBuf, 0, sizeof(s8ReadBuf));
		s32ReadLen = MDL_DRIVERMGR_Read(pstAllHandle->s32GprsModuleId, s8ReadBuf, sizeof(s8ReadBuf));
		if(s32ReadLen > 0)
		{
			MDL_DRIVERMGR_Ioctl(pstAllHandle->s32GprsModuleId, GPS_GET_WORK_MODE, (T_VOID *)&u8GprsWorkMode);
			if(GPS_DATA_MODE == u8GprsWorkMode)
			{
				ALL_GprsServiceHandle(pstAllHandle, s8ReadBuf);
				
			}else if(GPS_USER_MODE == u8GprsWorkMode)
			{
				memcpy(pstAllHandle->s8GatewayLocation, s8ReadBuf, s32ReadLen);
				MDL_DRIVERMGR_Ioctl(pstAllHandle->s32GprsModuleId, GPS_SET_DATA_MODE, T_NULL);
			}
		}
	}

	//����mesh���ݴ���
	if(RET_SUCCESS == MDL_DRIVERMGR_Pop(pstAllHandle->s32BleMeshModuleId))
	{
		memset(s8ReadBuf, 0, sizeof(s8ReadBuf));
		s32ReadLen = MDL_DRIVERMGR_Read(pstAllHandle->s32BleMeshModuleId, s8ReadBuf, sizeof(s8ReadBuf));
		if(s32ReadLen > 0)
		{
			ALL_BleMeshHandle(pstAllHandle, s8ReadBuf, s32ReadLen);
		}

	}

#if WD_EN
	//ι��
	MDL_DRIVERMGR_Write(pstAllHandle->s32WdModuleId, T_NULL, 0);
#endif	
}

T_VOID _TestTimer(T_VOID *pvData)
{

	printf("Time is over\n");
}

T_VOID _GatewayHeartbeat(T_VOID *pvData)
{
	AllHandle *pstAllHandle = (AllHandle *)pvData;
	T_S8 s8GatewayNo[24] = {0};
	cJSON *pJsonRoot = T_NULL;
	T_S8 *ps8HearData = T_NULL;
	T_U8 u8GprsWorkMode = 0;
	T_S8 s8GatewayData[512] = {0};
	static T_U32 u32Battery = 0;
//	T_S8 s8Bat[8] = {0};
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
	

	ps8HearData=cJSON_PrintUnformatted(pJsonRoot);

	strcpy(s8GatewayData, ps8HearData);
	strcat(s8GatewayData,"\r\n");
	printf("json baddy %s len %d\n",s8GatewayData,strlen(s8GatewayData));
	MDL_DRIVERMGR_Write(pstAllHandle->s32GprsModuleId, s8GatewayData, strlen(s8GatewayData));

	free(ps8HearData);
	ps8HearData = T_NULL;
	
	cJSON_Delete(pJsonRoot);
	pJsonRoot = T_NULL;
}


/********************************************
 *���ܣ��ϲ�Ӧ���߼����ʼ���ӿ�
 *���룺Ӧ���߼���handle
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:��
********************************************/
T_S32 ALL_Init(T_VOID **ppvAllHandle)
{
	AllHandle *pstAllHandle = T_NULL;
	ConfigInfo stConfigInfo, stConfigInfo1;
	strcpy((char *)stConfigInfo1.u8GateWayDevSn, "12345");
	
	ASSERT_EQUAL_RETVAL(ppvAllHandle, T_NULL, RET_FAILED);
    
	ASSERT_FUN_FAILED_RETVAL(MDL_DRIVERMGR_Init(), RET_SUCCESS, RET_FAILED); //��ʼ���м�㼰����ģ��

	ASSERT_FUN_FAILED_RETVAL(ALL_DB_Init(), RET_SUCCESS, RET_FAILED);//�����ݴ洢��
	//ALL_DB_DestoryAllTable();
	ENDIAN_SWAP_Init();
    ALL_DefaultSysConfig();
	ALL_DB_SetSysConfig(&stConfigInfo1);
    ALL_DB_GetSysConfig(&stConfigInfo);
    
	ALL_IsResetAllTable(D_SHEET_VER);    //�����̼����ж����ݱ�汾���Ƿ���Ҫ�����������ݱ�
    
	pstAllHandle = (AllHandle *)malloc(sizeof(AllHandle));
	ASSERT_EQUAL_RETVAL(pstAllHandle, T_NULL, RET_FAILED);
	memset(pstAllHandle, 0, sizeof(AllHandle));

	pstAllHandle->u8BatVal = 100;
	pstAllHandle->u8ModelSets = 0;
    pstAllHandle->s32GpioModuleId = MDL_DRIVERMGR_Open(DEVICE_GPIO_NAME);   //�������ĳ�ʼ������ŵ���ǰ�棬�����������·��Ӱ�����·
	pstAllHandle->s32TemperModuleId = MDL_DRIVERMGR_Open(DEVICE_TMP_NAME);
	pstAllHandle->s32PressureModuleId = MDL_DRIVERMGR_Open(DEVICE_PRESSURE_NAME);
	pstAllHandle->s32NetworkModuleId = MDL_DRIVERMGR_Open(DEVICE_NET_NAME);
	pstAllHandle->s32GprsModuleId = MDL_DRIVERMGR_Open(DEVICE_GPRS_NAME);
	pstAllHandle->s32BleMeshModuleId = MDL_DRIVERMGR_Open(DEVICE_BLEMESH_NAME);
#if WD_EN	
	pstAllHandle->s32WdModuleId = MDL_DRIVERMGR_Open(DEVICE_WD_NAME);
#endif

	MDL_TIMER_Init(&pstAllHandle->stTimerHead);
	
	pstAllHandle->u8Quit = T_FALSE;
	pstAllHandle->u8SysTate = SYSTEM_STATE_INIT;

	
	MDL_TIMER_Add(&pstAllHandle->stTimerHead, TEST_TIMER_TOK, 0, _TestTimer, T_NULL, pstAllHandle);
	MDL_TIMER_Start(&pstAllHandle->stTimerHead, TEST_TIMER_TOK, START_4G_TIME);

	MDL_TIMER_Add(&pstAllHandle->stTimerHead, MESHHEART_TX_TIMER_TOK, 0, ALL_BleMeshTxHeartbeat, T_NULL, pstAllHandle);
	MDL_TIMER_Start(&pstAllHandle->stTimerHead, MESHHEART_TX_TIMER_TOK, MESH_TX_HEART_TIME);

	MDL_TIMER_Add(&pstAllHandle->stTimerHead, MESHHEART_RX_TIMER_TOK, 0, ALL_BleMeshHeartbeatTimeout, T_NULL, pstAllHandle);

	MDL_TIMER_Add(&pstAllHandle->stTimerHead, GATEWAY_TX_TIMER_TOK, 0, _GatewayHeartbeat, T_NULL, pstAllHandle);
	MDL_TIMER_Start(&pstAllHandle->stTimerHead, GATEWAY_TX_TIMER_TOK, GATEWAY_TX_HEART_TIME);

	*ppvAllHandle = pstAllHandle;
	print_debug("system start \n");
	return RET_SUCCESS;
}

/********************************************
 *���ܣ��ϲ�Ӧ���߼������ҵ��
                   ����ӿ�
 *���룺Ӧ���߼���handle
 *�������.
 *��������
 *���أ���
 ע��:��
********************************************/
T_VOID ALL_Run(T_VOID *pvAllHandle)
{	
	AllHandle *pstAllHandle = T_NULL;
	
	ASSERT_EQUAL_RETURN(pvAllHandle, T_NULL);

	pstAllHandle = (AllHandle *)pvAllHandle;
	printf("hello word\n");	
	while(T_FALSE == pstAllHandle->u8Quit)
	{
		MDL_DRIVERMGR_Task();
		ALL_Task(pstAllHandle);
	}
}

/********************************************
 *���ܣ��ϲ�Ӧ���߼����ȥʼ��
                   �ӿ�
 *���룺Ӧ���߼���handle
 *�������.
 *��������
 *���أ���
 ע��:��
********************************************/
T_VOID ALL_DeInit(T_VOID *pvAllHandle)
{
	AllHandle *pstAllHandle = T_NULL;
	
	ASSERT_EQUAL_RETURN(pvAllHandle, T_NULL);

	pstAllHandle = (AllHandle *)pvAllHandle;
	
	MDL_TIMER_UnInit(&pstAllHandle->stTimerHead);

	//ALL_DB_DeInit();
	MDL_DRIVERMGR_Close(pstAllHandle->s32NetworkModuleId);
	MDL_DRIVERMGR_Close(pstAllHandle->s32GpioModuleId);
	MDL_DRIVERMGR_Close(pstAllHandle->s32WdModuleId);
	MDL_DRIVERMGR_Close(pstAllHandle->s32PressureModuleId);
	MDL_DRIVERMGR_Close(pstAllHandle->s32TemperModuleId);
	MDL_DRIVERMGR_Close(pstAllHandle->s32GprsModuleId);
	MDL_DRIVERMGR_Close(pstAllHandle->s32BleMeshModuleId);

	MDL_DRIVERMGR_DeInit();
	
	free(pvAllHandle);
	pvAllHandle = T_NULL;
}

