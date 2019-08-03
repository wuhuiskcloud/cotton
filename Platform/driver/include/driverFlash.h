/*********************************************
* @�ļ�: driverFlash.h
* @����: Li Min
* @�汾: v1.0.1
* @ʱ��: 2017-12-4
* @��Ҫ: �ṩMCU�ڲ�flash�����ӿ� 
*********************************************/

#ifndef __DRIVER_FLASH__
#define __DRIVER_FLASH__

#ifdef __cplusplus
extern "C" {
#endif

#include "baseType.h"
/* Flash������Ϣ---------------------------------------
	0x8000000��ʼ��128K�ֽ� ��ÿ��page��СΪ2K 
---------------------------------------------------------*/
#define	FLASH_SATRT_ADDR	0x8000000
#define	FLASH_END_ADDR		0x801FFFF
#define	FLASH_PAGE_SIZE		1024//2048

/*Flash���廮��----------------------------------------
	����������ݿ����ʼ��ַ�ͽ�����ַ 
---------------------------------------------------------*/
/* ===IAP ����12K (6page)=== */
#define FLASH_IAP_START			0x8000000
#define FLASH_IAP_END			0x8002fff

/* ===������ 54k(27page)=== */
//#define FLASH_DB_START			0x8003000
//#define FLASH_DB_END			0x80107ff

/* ===������ 2k(1page)=== */
#define FLASH_DB_START			0x800efff  //60page
#define FLASH_DB_END			0x8011023//0x800ffff   //61page

/*===Ԥ��2K(1page)===*/
#define	FLASH_RESAVE_START		0x8010800
#define	FLASH_RESAVE_END		0x8010fff


/* ===Ӧ�ó��� 60k(30page)=== */
#define FLASH_APP_START			0x8011000
#define FLASH_APP_END			0x801ffff		
/*---------------------------------*/
typedef	enum 
{
	FLASH_CMD_READ,		//��ָ����ַָ�����ȵ���������
	FLASH_CMD_WRITE,	//дָ����ַָ�����ȵ���������
	FLASH_CMD_ERASH,	//����ָ�������ݿ�����

	FLASH_CMD_CNT,		//falsh�����������
} T_FlashCmd;

typedef struct
{
	T_U32 u32OffsetAddr;	//ƫ�Ƶ�ַ(ƫ�Ƶ��ֽ���)
	T_U8* pu8DataBuf;		//���ݻ�����
	T_U32 u32Len;			//���ݳ���
} T_FlashCtlData;


/************************************************
 *���ܣ���flash����ģ����ӵ�
 			�����б��С�
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*************************************************/
T_S32 DRIVER_FLASH_Init(T_VOID);

#ifdef __cplusplus
}
#endif

#endif

