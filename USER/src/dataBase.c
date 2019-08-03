/*********************************************
* @文件: dataBase.c
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2017-12-4
* @概要:  实现flash数据增删改查的
                    接口
*********************************************/
#include "dataBase.h"
#include "sheet.h"

#define DB_NAME "FLASH"

#define DB_CONFIG               1
#define DB_USERPWD              2
#define DB_TEMPPWD              3
#define DB_USEDTEMPPWD          4
#define DB_BL_RECORD            5
#define DB_TPWDL_RECORD         6
#define DB_PWDL_RECORD          7
#define DB_CARDL_RECORD         8
#define DB_ATTR_CARD            9

//#define DB_YUN_PWD              9
//#define DB_YUN_CARD             10

static SheetHandle stDbHandle;

/********************************************
 *功能：打开数据存储表
 *输入：无
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无。
*********************************************/
T_S32 ALL_DB_Init(T_VOID)
{
	ASSERT_FUN_FAILED_RETVAL(MDL_SHEET_Open(&stDbHandle, DB_NAME), RET_SUCCESS, RET_FAILED);

	ASSERT_FUN_FAILED_RETVAL(MDL_SHEET_Create(&stDbHandle, DB_CONFIG, 1, sizeof(ConfigInfo)), RET_SUCCESS, RET_FAILED);
	
	//ASSERT_FUN_FAILED_RETVAL(MDL_SHEET_Create(&stDbHandle, DB_USERPWD, USERPWD_RECORD_MAX, sizeof(UserPwdInfo)), RET_SUCCESS, RET_FAILED);  //4k

	//ASSERT_FUN_FAILED_RETVAL(MDL_SHEET_Create(&stDbHandle, DB_TEMPPWD, TEMPPWD_RECORD_MAX, sizeof(TempPwdInfo)), RET_SUCCESS, RET_FAILED);   //4k

	//ASSERT_FUN_FAILED_RETVAL(MDL_SHEET_Create(&stDbHandle, DB_USEDTEMPPWD, UTPWD_RECORD_MAX, sizeof(UsedTempPwdInfo)), RET_SUCCESS, RET_FAILED); //4k

    //ASSERT_FUN_FAILED_RETVAL(MDL_SHEET_Create(&stDbHandle, DB_ATTR_CARD, CARD_ATTR_MAX, sizeof(CardAttrInfo)), RET_SUCCESS, RET_FAILED);  //2M

    //ASSERT_FUN_FAILED_RETVAL(MDL_SHEET_Create(&stDbHandle, DB_BL_RECORD, BTL_RECORD_MAX, sizeof(LogInfo)), RET_SUCCESS, RET_FAILED);  //1M

	//ASSERT_FUN_FAILED_RETVAL(MDL_SHEET_Create(&stDbHandle, DB_TPWDL_RECORD, TPWDL_RECORD_MAX, sizeof(LogInfo)), RET_SUCCESS, RET_FAILED); //8k

	//ASSERT_FUN_FAILED_RETVAL(MDL_SHEET_Create(&stDbHandle, DB_PWDL_RECORD, PWDL_RECORD_MAX, sizeof(LogInfo)), RET_SUCCESS, RET_FAILED); //1M

	//ASSERT_FUN_FAILED_RETVAL(MDL_SHEET_Create(&stDbHandle, DB_CARDL_RECORD, CARDL_RECORD_MAX, sizeof(LogInfo)), RET_SUCCESS, RET_FAILED); //1M

	MDL_SHEET_Initial(&stDbHandle);
	
	return RET_SUCCESS;
}

/********************************************
 *功能：获取系统配置参数
 *输入：配置信息表
 *输出：配置信息
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无。
*********************************************/
T_S32 ALL_DB_GetSysConfig(ConfigInfo *pstConfigInfo)
{
	ASSERT_EQUAL_RETVAL(pstConfigInfo, T_NULL, RET_FAILED);

	MDL_SHEET_Reset(&stDbHandle, DB_CONFIG);
	return MDL_SHEET_Query(&stDbHandle, DB_CONFIG, (T_U8 *)pstConfigInfo, sizeof(ConfigInfo));
}

/********************************************
 *功能：设置系统配置参数
 *输入：配置信息表
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:	调用之前先调用ALL_DB_ClearSysConfig
 			清空之前的数据。
*********************************************/
T_S32 ALL_DB_SetSysConfig(ConfigInfo *pstConfigInfo)
{
	ASSERT_EQUAL_RETVAL(pstConfigInfo, T_NULL, RET_FAILED);

	MDL_SHEET_Reset(&stDbHandle, DB_CONFIG);
	return MDL_SHEET_Update(&stDbHandle, DB_CONFIG, (T_U8 *)pstConfigInfo, sizeof(ConfigInfo));
}

/********************************************
 *功能：清除系统配置参数
 *输入：无
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无。
*********************************************/
T_S32 ALL_DB_SetConfigDefault(T_VOID)
{
	ConfigInfo stConfig;

	memset(&stConfig, 0, sizeof(ConfigInfo));

    stConfig.u8SheetVer = D_SHEET_VER;
    strncpy((T_S8 *)stConfig.u8GateWayDevSn, D_DEV_SN_NUM, sizeof(stConfig.u8GateWayDevSn)-1);
    
	MDL_SHEET_Reset(&stDbHandle, DB_CONFIG);
	return MDL_SHEET_Update(&stDbHandle, DB_CONFIG, (T_U8 *)&stConfig, sizeof(ConfigInfo));
}



/********************************************
 *功能：删除所有的信息表
 *输入：日志表、日志信息
 *输出：无
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无
*********************************************/
T_S32 ALL_DB_DestoryAllTable(T_VOID)
{	
    MDL_SHEET_Destory(&stDbHandle, DB_CONFIG);
	return RET_SUCCESS;
}

/********************************************
 *功能：擦除黑名单表、需要延期卡表、用户密码表、
 *临时密码表、已使用临时密码表、四张记录表的数据
 *输入：无
 *输出：无
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无
*********************************************/

T_S32 ALL_DB_ClearDataTable(T_VOID)
{
	MDL_SHEET_Clear(&stDbHandle, DB_CONFIG);
    return RET_SUCCESS;
}

/********************************************
 *功能：判断数据表是否溢出
 *输入：无
 *输出：无.
 *条件：无
 *返回：溢出返回true、否则返回false
 注意:无。
*********************************************/
T_S32 ALL_DB_IsFull(T_VOID)
{
	SheetStatus eStatus = MDL_SHEET_GetStatus(&stDbHandle);

	if(ST_NOSPACE == eStatus)
	{
		return T_TRUE;
	}

	return T_FALSE;
}

/********************************************
 *功能：判断是否查询到数据表结尾
 *输入：无
 *输出：无.
 *条件：无
 *返回：溢出返回true、否则返回false
 注意:无。
*********************************************/
T_S32 ALL_DB_IsEnd(T_VOID)
{
	SheetStatus eStatus = MDL_SHEET_GetStatus(&stDbHandle);

	if(ST_EOF == eStatus)
	{
		return T_TRUE;
	}

	return T_FALSE;
}

/********************************************
 *功能：关闭数据存储表
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID ALL_DB_DeInit(T_VOID)
{
	MDL_SHEET_Close(&stDbHandle);
}

