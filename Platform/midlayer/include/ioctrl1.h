/*********************************************
* @�ļ�: ioctrl.h
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2017-12-6
* @��Ҫ: ���ڵ����ļ�ioctrl�ӿ�ʱ��
                  ���������Ͳ�һ�µ������
                  �ʶ���һ���ļ����ڶ���
                  ����ioctrl������ʽ��
*********************************************/

#ifndef IOCTRL_H
#define IOCTRL_H

#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus
#define GPS_USER_MODE
#include "baseType.h"

//#include "driverSPIFlash.h"

/*==============��������������Ͷ���================start blue tooth*/

//IoCtl ���������

typedef enum 
{
    BAUDRATE_9600 = 9600,
    BAUDRATE_19200 = 19200,
    BAUDRATE_38400 = 38400,
    BAUDRATE_57200 = 57200,
    BAUDRATE_115200 = 115200,
    
}BleBaudRate;

/*=======================��ͨGPIO����================End*/

#define GPIO_SET 1
#define GPIO_GET 0

typedef enum
{
	GPIO_RESET = 0x01, 		//��λ
	GPIO_RECVY = 0x02, 		//�ָ�����
	GPIO_RELAY    = 0x04, 	//������
	GPIO_BEEP_OPEN = 0x08, 	//��������
	GPIO_LED = 	0x10,		// LED����
	GPIO_STRONG = 0x200,	//����
	GPIO_UNPLUG = 0x400,	//����
}GpioType;

typedef struct 
{
	GpioType eType;
	T_U8 u8Val;
}GpioCrtl;


/*==============�ڲ�flash����������Ͷ���================*/

/*
����DRIVER_Flash_Ioctl�е�����
*/
typedef enum 
{
	MCUFLASH_CMD_READ, 	//��ָ����ַָ�����ȵ���������
	MCUFLASH_CMD_WRITE,	//дָ����ַָ�����ȵ���������
	MCUFLASH_CMD_ERASH,	//����ָ�������ݿ�����

	MCUFLASH_CMD_CNT,		//falsh�����������
} T_MCUFlashCmd;
	
/*
���ڵ���DRIVER_SPIFlash_Ioctl�����ⲿflashʱ�����ݽṹ
*/
typedef struct
{
	T_U32 u32OffsetAddr;	//ƫ�Ƶ�ַ(ƫ�Ƶ��ֽ���)
	T_U8* pu8DataBuf;		//���ݻ�����
	T_U32 u32Len;			//���ݳ���
} T_MCUFlashCtlData;





/* ===================================================End */


#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end IOCTRL_H
