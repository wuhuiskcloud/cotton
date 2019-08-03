/*********************************************
* @�ļ�: driverSPIFlash.h
* @����: Li Min
* @�汾: v1.0.1
* @ʱ��: 2017-12-27
* @��Ҫ: �ṩ�ⲿSPI Flash�������ӿ� 
*********************************************/

#ifndef __DRIVER_SPIFLASH__
#define __DRIVER_SPIFLASH__

#ifdef __cplusplus
extern "C" {
#endif

#include "baseType.h"
#if 0
#define FLASH_PAGE_SIZE			256

#define FLASH_START_ADDR		0x00000000	
#define FLASH_END_ADDR			0x007FFFFF

/* ������־�洢�ĵ�ַ 4K*/
#define UPDATA_FLAG_ADDR		FLASH_START_ADDR
#define UPDATA_FLAG_ADDR_END	0x00000FFF

/* ����ʱ����洢�ĵ�ַ 64K*/
#define UPDATA_START_ADDR		0x00001000
#define UPDATA_END_ADDR			0x00020FFF

/* ===������ 4095K=== */
#define FLASH_DB_START			0x00021000
#define FLASH_DB_END			0x00420FFF

/*===������128K===*/
#define	FLASH_CACHE_START		0x00421000
#define FALSH_CACHE_END			0x00440FFF

/*===Ԥ��8060K===*/
#define	FLASH_RESAVE_START		0x00441000
#define	FLASH_RESAVE_END		0x007FEFFF

/*===spiFlash��ʼ����־�洢��4K===*/
#define	FLASH_INIT_START		0x007FF000
#define	FLASH_INIT_END			FLASH_END_ADDR

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
 *���ܣ���SPI Flash����ģ����ӵ�
 			�����б��С�
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*************************************************/
T_S32 DRIVER_SpiFlash_Init(T_VOID);

#endif
#ifdef __cplusplus
}
#endif

#endif




