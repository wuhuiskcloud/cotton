/*********************************************
* @文件: ioctrl.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2017-12-6
* @概要: 由于调用文件ioctrl接口时出
                  现数据类型不一致的情况，
                  故独立一个文件用于定义
                  各类ioctrl数据形式。
*********************************************/

#ifndef IOCTRL_H
#define IOCTRL_H

#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus

#include "baseType.h"

//#include "driverSPIFlash.h"

/*==============蓝牙相关数据类型定义================start blue tooth*/

//IoCtl 设置命令定义

//A9G 设置命令	
typedef enum
{
    A9G_USER_MODE = 0x00,   //用户普通模式
    A9G_UPDATE_MODE = 0x01,  //系统升级模式
    A9G_CPIT_MODE = 0x02,	//透传模式	
    A9G_BATT_WARING = 0x03, //电池电量低警告
}A9g_SysModeType;


enum{    
	GPS_USER_MODE = 0xf1,   //设置用户指令模式    
	GPS_DATA_MODE = 0xf2,   //数据透传模式
};

typedef enum {    
	GPS_GET_WORK_MODE = 0x01,   //获取Gprs用户普通模式 
	GPS_SET_DATA_MODE = 0x02,   //设置透传模式
	GPS_RECONNERT_MODE = 0x03,
}GpsCrtl;
/*=======================普通GPIO控制================End*/

#define GPIO_SET 1
#define GPIO_GET 0

typedef enum 
{
    BAUDRATE_9600 = 9600,
    BAUDRATE_19200 = 19200,
    BAUDRATE_38400 = 38400,
    BAUDRATE_57200 = 57200,
    BAUDRATE_115200 = 115200,
    
}BleBaudRate;

/*=======================普通GPIO控制================End*/

#define GPIO_SET 1
#define GPIO_GET 0

typedef enum
{
	GPIO_RESET = 0x01, 		//复位
	GPIO_RECVY = 0x02, 		//恢复出厂
	GPIO_RELAY    = 0x04, 	//开关门
	GPIO_BEEP_OPEN = 0x08, 	//蜂鸣器打开
	GPIO_LED = 	0x10,		// LED控制
	GPIO_STRONG = 0x200,	//防爆
	GPIO_UNPLUG = 0x400,	//防拆
}GpioType;

typedef struct 
{
	GpioType eType;
	T_U8 u8Val;
}GpioCrtl;


/*==============内部flash相关数据类型定义================*/

/*
用于DRIVER_Flash_Ioctl中的命令
*/
typedef enum 
{
	MCUFLASH_CMD_READ, 	//读指定地址指定长度的数据命令
	MCUFLASH_CMD_WRITE,	//写指定地址指定长度的数据命令
	MCUFLASH_CMD_ERASH,	//擦除指定的数据块命令

	MCUFLASH_CMD_CNT,		//falsh操作命令个数
} T_MCUFlashCmd;
	
/*
用于调用DRIVER_SPIFlash_Ioctl控制外部flash时的数据结构
*/
typedef struct
{
	T_U32 u32OffsetAddr;	//偏移地址(偏移的字节数)
	T_U8* pu8DataBuf;		//数据缓冲区
	T_U32 u32Len;			//数据长度
} T_MCUFlashCtlData;





/* ===================================================End */


#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end IOCTRL_H

