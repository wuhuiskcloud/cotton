/*********************************************
* @文件: driverSPIFlash.h
* @作者: Li Min
* @版本: v1.0.1
* @时间: 2017-12-27
* @概要: 提供外部SPI Flash的驱动接口 
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

/* 升级标志存储的地址 4K*/
#define UPDATA_FLAG_ADDR		FLASH_START_ADDR
#define UPDATA_FLAG_ADDR_END	0x00000FFF

/* 升级时代码存储的地址 64K*/
#define UPDATA_START_ADDR		0x00001000
#define UPDATA_END_ADDR			0x00020FFF

/* ===数据区 4095K=== */
#define FLASH_DB_START			0x00021000
#define FLASH_DB_END			0x00420FFF

/*===缓冲区128K===*/
#define	FLASH_CACHE_START		0x00421000
#define FALSH_CACHE_END			0x00440FFF

/*===预留8060K===*/
#define	FLASH_RESAVE_START		0x00441000
#define	FLASH_RESAVE_END		0x007FEFFF

/*===spiFlash初始化标志存储区4K===*/
#define	FLASH_INIT_START		0x007FF000
#define	FLASH_INIT_END			FLASH_END_ADDR

typedef	enum 
{
	FLASH_CMD_READ,		//读指定地址指定长度的数据命令
	FLASH_CMD_WRITE,	//写指定地址指定长度的数据命令
	FLASH_CMD_ERASH,	//擦除指定的数据块命令

	FLASH_CMD_CNT,		//falsh操作命令个数
} T_FlashCmd;

typedef struct
{
	T_U32 u32OffsetAddr;	//偏移地址(偏移的字节数)
	T_U8* pu8DataBuf;		//数据缓冲区
	T_U32 u32Len;			//数据长度
} T_FlashCtlData;

/************************************************
 *功能：将SPI Flash驱动模块添加到
 			驱动列表中。
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
T_S32 DRIVER_SpiFlash_Init(T_VOID);

#endif
#ifdef __cplusplus
}
#endif

#endif




