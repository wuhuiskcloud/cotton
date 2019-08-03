/*********************************************
* @�ļ�:dataBase.h
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2017-12-4
* @��Ҫ:  ʵ��flash������ɾ�Ĳ��
                    �ӿ�
*********************************************/
#ifndef DATABASE_H
#define DATABASE_H
	
#ifdef __cplusplus
	extern "C" {
#endif//end __cplusplus

#include "baseType.h"

#define USERPWD_RECORD_MAX 450 //�û�����洢����� 4K   450*
#define TEMPPWD_RECORD_MAX 63 //��ʱ����洢����� 4K
#define UTPWD_RECORD_MAX 300 //��ʹ����ʱ����洢����� 4K
#define TPWDL_RECORD_MAX 700 //��ʱ���뿪����¼�洢�����
#define BTL_RECORD_MAX 100000 //����������¼�洢�����
#define PWDL_RECORD_MAX 100000 //���뿪����¼�洢�����
#define CARDL_RECORD_MAX 100000 //��������¼�洢�����

#define CARD_ATTR_MAX 90000   //1180   //2M�ռ�
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

//ϵͳ�������ñ����ݶ���
typedef struct tagConfigInfo
{
	T_U8 u8GateWayDevSn[D_DEV_SN_LEN];  //����SN��
    T_U8 u8SheetVer;            //���ݱ�汾��
    
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
	TAB_BT_LOCK,   //����app������¼��
	TAB_PWD_LOCK,  //�������뿪����¼��
	TAB_TPWD_LOCK, //��ʱ���뿪����¼��
	TAB_CARD_LOCK, //ˢ��������¼��
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
    T_U8 u8CardSerial[D_SERIAL_LEN];  //�����к�
    T_U8 u8Status;                     //��ʧ ���� ��֤
    T_U8 u8Freeze;                     //����ⶳ
    T_U8 u8Validity[D_VALIDITY_LEN];  //��Ч��
    T_U8 u8ValidNum[D_VALIDNO_LEN];  //��Ч����
    T_U8 u8Timestamp[D_TIMESTM_LEN];  //����ʱ���
    
}CardAttrInfo;


/********************************************
 *���ܣ������ݴ洢��
 *���룺��
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:�ޡ�
*********************************************/
T_S32 ALL_DB_Init(T_VOID);
/********************************************
 *���ܣ���ȡϵͳ���ò���
 *���룺������Ϣ��
 *�����������Ϣ
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:�ޡ�
*********************************************/
T_S32 ALL_DB_GetSysConfig(ConfigInfo *pstConfigInfo);
/********************************************
 *���ܣ�����ϵͳ���ò���
 *���룺������Ϣ��
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:�ޡ�
*********************************************/
T_S32 ALL_DB_SetSysConfig(ConfigInfo *pstConfigInfo);
/********************************************
 *���ܣ����ϵͳ���ò���
 *���룺��
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:�ޡ�
*********************************************/
T_S32 ALL_DB_SetConfigDefault(T_VOID);
/********************************************
 *���ܣ�ɾ�����е���Ϣ��
 *���룺��־����־��Ϣ
 *�������
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:��
*********************************************/
T_S32 ALL_DB_DestoryAllTable(T_VOID);

/********************************************
 *���ܣ��ж����ݱ��Ƿ����
 *���룺��
 *�������.
 *��������
 *���أ��������true�����򷵻�false
 ע��:�ޡ�
*********************************************/
T_S32 ALL_DB_IsFull(T_VOID);
/********************************************
 *���ܣ��ж��Ƿ��ѯ�����ݱ��β
 *���룺��
 *�������.
 *��������
 *���أ��������true�����򷵻�false
 ע��:�ޡ�
*********************************************/
T_S32 ALL_DB_IsEnd(T_VOID);
/********************************************
 *���ܣ��ر����ݴ洢��
 *���룺��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID ALL_DB_DeInit(T_VOID);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //DATABASE_H

