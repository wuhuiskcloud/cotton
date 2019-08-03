/*********************************************
* @文件: driverFlash.h
* @作者: Li Min
* @版本: v1.0.1
* @时间: 2017-12-4
* @概要: 提供MCU内部flash驱动接口 
*********************************************/

#ifndef __DRIVER_FLASH__
#define __DRIVER_FLASH__

#ifdef __cplusplus
extern "C" {
#endif

#include "baseType.h"
/* Flash基本信息---------------------------------------
	0x8000000开始，128K字节 ，每个page大小为2K 
---------------------------------------------------------*/
#define	FLASH_SATRT_ADDR	0x8000000
#define	FLASH_END_ADDR		0x801FFFF
#define	FLASH_PAGE_SIZE		1024//2048

/*Flash具体划分----------------------------------------
	定义各个数据块的起始地址和结束地址 
---------------------------------------------------------*/
/* ===IAP 程序，12K (6page)=== */
#define FLASH_IAP_START			0x8000000
#define FLASH_IAP_END			0x8002fff

/* ===数据区 54k(27page)=== */
//#define FLASH_DB_START			0x8003000
//#define FLASH_DB_END			0x80107ff

/* ===数据区 2k(1page)=== */
#define FLASH_DB_START			0x800efff  //60page
#define FLASH_DB_END			0x8011023//0x800ffff   //61page

/*===预留2K(1page)===*/
#define	FLASH_RESAVE_START		0x8010800
#define	FLASH_RESAVE_END		0x8010fff


/* ===应用程序， 60k(30page)=== */
#define FLASH_APP_START			0x8011000
#define FLASH_APP_END			0x801ffff		
/*---------------------------------*/
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
 *功能：将flash驱动模块添加到
 			驱动列表中。
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
T_S32 DRIVER_FLASH_Init(T_VOID);

#ifdef __cplusplus
}
#endif

#endif

