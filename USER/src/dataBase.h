/*********************************************
* @文件:dataBase.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2017-12-4
* @概要:  实现flash数据增删改查的
                    接口
*********************************************/
#ifndef DATABASE_H
#define DATABASE_H
	
#ifdef __cplusplus
	extern "C" {
#endif//end __cplusplus

#include "baseType.h"

#define USERPWD_RECORD_MAX 450 //用户密码存储最大数 4K   450*
#define TEMPPWD_RECORD_MAX 63 //临时密码存储最大数 4K
#define UTPWD_RECORD_MAX 300 //已使用临时密码存储最大数 4K
#define TPWDL_RECORD_MAX 700 //临时密码开锁记录存储最大数
#define BTL_RECORD_MAX 100000 //蓝牙开锁记录存储最大数
#define PWDL_RECORD_MAX 100000 //密码开锁记录存储最大数
#define CARDL_RECORD_MAX 100000 //卡开锁记录存储最大数

#define CARD_ATTR_MAX 90000   //1180   //2M空间
//#define YUN_PWD_MAX 300
//#define YUN_CARD_MAX 300

#define D_PASSWD_LEN 9
#define D_CARD_LEN 8
#define D_BLACKLIST_LEN 5
#define D_USED_TEMP_PWD_LEN 5
#define D_LOG_LEN 9
#define D_SHEET_VER 1
#define D_MAC_LEN 6
#define D_SERIAL_LEN 4
#define D_VALIDITY_LEN 4
#define D_VALIDNO_LEN 5
#define D_TIMESTM_LEN 7

#define D_DEV_SN_LEN 8

#define D_DEV_SN_NUM "01020001"

//系统参数配置表数据定义
typedef struct tagConfigInfo
{
	T_U8 u8GateWayDevSn[D_DEV_SN_LEN];  //网关SN号
    T_U8 u8SheetVer;            //数据表版本号
    
}ConfigInfo;

typedef struct tagTempPwdInfo
{
	T_S8 s8Passwd[D_PASSWD_LEN];
}TempPwdInfo;

typedef struct tagUserPwdInfo
{
	T_S8 s8Flag;
	T_S8 s8RoomNum[2];
	T_S8 s8Passwd[3];//3
}UserPwdInfo;

typedef struct tagUsedTempPwdInfo
{
	T_S8 s8UsedTempPwd[D_USED_TEMP_PWD_LEN];
}UsedTempPwdInfo;

typedef enum tagLogTab
{
	TAB_BT_LOCK,   //蓝牙app开锁记录表
	TAB_PWD_LOCK,  //按键密码开锁记录表
	TAB_TPWD_LOCK, //临时密码开锁记录表
	TAB_CARD_LOCK, //刷卡开锁记录表
}LogTab;

typedef struct tagLogInfo
{
	T_S8 s8Log[D_LOG_LEN];
}LogInfo;

typedef struct tagYunCardInfo
{
	T_S8 s8CardId[D_CARD_LEN];
}YunCardInfo;

typedef struct tagYunPwdInfo
{
	T_S8 s8Pwd[D_PASSWD_LEN];
}YunPwdInfo;

typedef struct tagCardAttrInfo
{
    T_U8 u8CardSerial[D_SERIAL_LEN];  //卡序列号
    T_U8 u8Status;                     //挂失 激活 认证
    T_U8 u8Freeze;                     //冻结解冻
    T_U8 u8Validity[D_VALIDITY_LEN];  //有效期
    T_U8 u8ValidNum[D_VALIDNO_LEN];  //有效次数
    T_U8 u8Timestamp[D_TIMESTM_LEN];  //更新时间戳
    
}CardAttrInfo;


/********************************************
 *功能：打开数据存储表
 *输入：无
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无。
*********************************************/
T_S32 ALL_DB_Init(T_VOID);
/********************************************
 *功能：获取系统配置参数
 *输入：配置信息表
 *输出：配置信息
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无。
*********************************************/
T_S32 ALL_DB_GetSysConfig(ConfigInfo *pstConfigInfo);
/********************************************
 *功能：设置系统配置参数
 *输入：配置信息表
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无。
*********************************************/
T_S32 ALL_DB_SetSysConfig(ConfigInfo *pstConfigInfo);
/********************************************
 *功能：清除系统配置参数
 *输入：无
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无。
*********************************************/
T_S32 ALL_DB_SetConfigDefault(T_VOID);
/********************************************
 *功能：删除所有的信息表
 *输入：日志表、日志信息
 *输出：无
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无
*********************************************/
T_S32 ALL_DB_DestoryAllTable(T_VOID);

/********************************************
 *功能：判断数据表是否溢出
 *输入：无
 *输出：无.
 *条件：无
 *返回：溢出返回true、否则返回false
 注意:无。
*********************************************/
T_S32 ALL_DB_IsFull(T_VOID);
/********************************************
 *功能：判断是否查询到数据表结尾
 *输入：无
 *输出：无.
 *条件：无
 *返回：溢出返回true、否则返回false
 注意:无。
*********************************************/
T_S32 ALL_DB_IsEnd(T_VOID);
/********************************************
 *功能：关闭数据存储表
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
*********************************************/
T_VOID ALL_DB_DeInit(T_VOID);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //DATABASE_H

