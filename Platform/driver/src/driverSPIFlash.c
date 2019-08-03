/*********************************************
* @文件: driverSPIFlash.c
* @作者: Li Min
* @版本: v1.0.1
* @时间: 2017-12-27
* @概要: 外部SPI Flash的驱动接口 的实现
*********************************************/

//#include "driverSPIFlash.h"
#include "driverFile.h"
#include "ioctrl.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
//#include "driverIWDog.h"

#if 1
/* ===相关IO口定义===*/
#define sFLASH_SPI                       SPI1
#define sFLASH_SPI_CLK                   RCC_APB2Periph_SPI1

#define sFLASH_CS_PIN                    GPIO_Pin_15     			/* PA4 */     
#define sFLASH_CS_GPIO_PORT              GPIOA          			/* GPIA */     
#define sFLASH_CS_GPIO_CLK               RCC_AHBPeriph_GPIOA

#define sFLASH_SPI_SCK_PIN               GPIO_Pin_3                  /* PA5 */
#define sFLASH_SPI_SCK_GPIO_PORT         GPIOB                       /* GPIOA */
#define sFLASH_SPI_SCK_GPIO_CLK          RCC_AHBPeriph_GPIOB

#define sFLASH_SPI_MISO_PIN              GPIO_Pin_4                  /* PA6 */
#define sFLASH_SPI_MISO_GPIO_PORT        GPIOB                       /* GPIOA */
#define sFLASH_SPI_MISO_GPIO_CLK         RCC_AHBPeriph_GPIOB

#define sFLASH_SPI_MOSI_PIN              GPIO_Pin_5                  /* PA7 */
#define sFLASH_SPI_MOSI_GPIO_PORT        GPIOB                       /* GPIOA */
#define sFLASH_SPI_MOSI_GPIO_CLK         RCC_AHBPeriph_GPIOB

#define	SPI_FLASH_CS_H()	GPIO_SetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)
#define	SPI_FLASH_CS_L()	GPIO_ResetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)

/*===== W25Q64 寄存器定义=====*/
#define W25X_WriteEnable        0x06   
#define W25X_WriteDisable       0x04   
#define W25X_ReadStatusReg      0x05   
#define W25X_WriteStatusReg     0x01   
#define W25X_ReadData           0x03   
#define W25X_FastReadData       0x0B   
#define W25X_FastReadDual       0x3B   
#define W25X_PageProgram        0x02   
#define W25X_BlockErase         0xD8   
#define W25X_SectorErase        0x20   
#define W25X_ChipErase          0xC7   
#define W25X_PowerDown          0xB9   
#define W25X_ReleasePowerDown   0xAB   
#define W25X_DeviceID           0xAB   
#define W25X_ManufactDeviceID   0x90   
#define W25X_JedecDeviceID      0x9F
#define W25X_UniqueID			0x4B

#define W25Q64_PAGE_SIZE      	FLASH_PAGE_SIZE						// 一个page 256 (Byte) 
#define W25Q64_SECTOR_SIZE      (W25Q64_PAGE_SIZE*16)	// 16个page组成一个sector: PAGE * 16 = 4K (Byte) //擦除的最小操作单元
#define W25Q64_BLOCK_SIZE       (W25Q64_SECTOR_SIZE*16)	// 16个sector组成一个block: SECTOR * 16 = 64K (Byte)
#define W25Q64_CHIP_SIZE		(W25Q64_BLOCK_SIZE*128)	// 128个block组成整个flash空间: BLOCK * 128 = 8M

/************************************************
 *功能：SPI Flash对应SPI接口的引脚初始化
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
static T_VOID _SPIFlash_IO_Init(T_VOID)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(sFLASH_CS_GPIO_CLK | sFLASH_SPI_MOSI_GPIO_CLK | 
    			sFLASH_SPI_MISO_GPIO_CLK | sFLASH_SPI_SCK_GPIO_CLK, ENABLE);

    /* CS */
    GPIO_InitStructure.GPIO_Pin  = sFLASH_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(sFLASH_CS_GPIO_PORT, &GPIO_InitStructure);
	SPI_FLASH_CS_H();

	GPIO_PinAFConfig(sFLASH_SPI_SCK_GPIO_PORT, sFLASH_SPI_SCK_PIN, GPIO_AF_0);
	GPIO_PinAFConfig(sFLASH_SPI_MISO_GPIO_PORT, sFLASH_SPI_MISO_PIN, GPIO_AF_0);
	GPIO_PinAFConfig(sFLASH_SPI_MOSI_GPIO_PORT, sFLASH_SPI_MOSI_PIN, GPIO_AF_0);

	/* SCK */
    GPIO_InitStructure.GPIO_Pin   = sFLASH_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

    /* MOSI */
    GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

    /* MISO */
    GPIO_InitStructure.GPIO_Pin  = sFLASH_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
}

/************************************************
 *功能：SPI Flash对应SPI接口工作模式配置
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
static T_VOID _SPIFlash_SPI_init(T_VOID)
{
    SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(sFLASH_SPI_CLK, ENABLE);
    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(sFLASH_SPI, &SPI_InitStructure);
	
	SPI_RxFIFOThresholdConfig(sFLASH_SPI, SPI_RxFIFOThreshold_QF);
	
    SPI_Cmd(sFLASH_SPI, ENABLE);
}

/************************************************
 *功能：SPI Flash初始化
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
static T_VOID _SPIFlash_init(T_VOID)
{
	_SPIFlash_IO_Init();
	_SPIFlash_SPI_init();
}

/************************************************
 *功能：通过SPI发送和接收一个字节
 *输入：要发送的数据
 *输出：无
 *条件：无
 *返回：读到的数据
 注意:
*************************************************/
static T_U8 _SPIFlash_TxRx_Byte(T_U8 data)
{
	T_U32 timeout = 0;

	#if (1)
	timeout = 3000;
    while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET)
	{
	    DRIVER_IWDog_Write(T_NULL, 0);   //定时喂狗防止应用层读写擦除数据看门狗超时  
		if(timeout<= 0)
		{
			return 'F';
		}else
		{
			timeout--;
		}
	}
	#else
	 while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);
	#endif
	
	SPI_SendData8(sFLASH_SPI, data);

	#if (1)
	timeout = 3000;
    while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET)
	{
	    DRIVER_IWDog_Write(T_NULL, 0);   //定时喂狗防止应用成写数据看门狗超时 
		if(timeout<= 0)
		{
			return 'F';
		}else
		{
			timeout--;
		}
	}
	#else
	while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	#endif

    return SPI_ReceiveData8(sFLASH_SPI);
}

/************************************************
 *功能：写使能
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/	
static T_VOID _SPIFlash_Write_En(T_VOID)
{
	SPI_FLASH_CS_L(); 
	_SPIFlash_TxRx_Byte(W25X_WriteEnable);          
	SPI_FLASH_CS_H();  
}

/************************************************
 *功能：写禁止
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
static T_VOID _SPIFlash_Write_Dis(T_VOID)
{
	SPI_FLASH_CS_L(); 
	_SPIFlash_TxRx_Byte(W25X_WriteDisable);          
	SPI_FLASH_CS_H();  
}

/************************************************
 *功能：等待写完成
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
static void _SPIFlash_WaitForWriteEnd(void)
{
	T_U8 FLASH_Status = 0;
	
	SPI_FLASH_CS_L();
	
	_SPIFlash_TxRx_Byte(W25X_ReadStatusReg); //读取Flash 状态寄存器1
	do
	{
		FLASH_Status = _SPIFlash_TxRx_Byte(0x00);
	}while ((FLASH_Status & 0x01) == 0x01);//若没有写完则一直等待，保持CS=0

	SPI_FLASH_CS_H();
}

/************************************************
 *功能：从指定的地址读取指定数量的字节
 *输入：databuffer 读到数据的缓存区
 			datasize缓存区大小
 			address要读的地址
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
static T_VOID _SPIFlash_ReadBuf(T_S8 *databuffer, T_U16 datasize, T_U32 address)
{   
    T_U16 i = 0;

    SPI_FLASH_CS_L();

    _SPIFlash_TxRx_Byte(W25X_ReadData);
    _SPIFlash_TxRx_Byte((T_U8)(address>>16));
    _SPIFlash_TxRx_Byte((T_U8)(address>>8));
    _SPIFlash_TxRx_Byte((T_U8)address);  
    for(i=0; i<datasize; i++)
    {
      databuffer[i]=_SPIFlash_TxRx_Byte(0xff);
    }  

    SPI_FLASH_CS_H();
}  

/************************************************
 *功能：向指定的地址写入指定数量的字节     
 *输入：databuffer 读到数据的缓存区
 			datasize缓存区大小
 			address要读的地址     
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
static T_VOID _SPIFlash_WriteBuf(T_S8 *databuffer, T_U16 datasize, T_U32 address)
{ 
   _SPIFlash_Write_En();
   
   SPI_FLASH_CS_L();
   
   _SPIFlash_TxRx_Byte(W25X_PageProgram);
   _SPIFlash_TxRx_Byte((T_U8)(address>>16));
   _SPIFlash_TxRx_Byte((T_U8)(address>>8));
   _SPIFlash_TxRx_Byte((T_U8)address); 
   while(datasize>0)
   {
	 _SPIFlash_TxRx_Byte(*databuffer++);
	 datasize--; 
   }
   
   SPI_FLASH_CS_H();
  
   _SPIFlash_WaitForWriteEnd();  

   _SPIFlash_Write_Dis();
}

/************************************************
 *功能：擦除一个扇区(4K)
 *输入：要擦除的扇区首地址
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
T_VOID _SPIFlash_SectorErase(T_U32 EraseAddr)
{
	_SPIFlash_Write_En();

	SPI_FLASH_CS_L();
	
	_SPIFlash_TxRx_Byte(W25X_SectorErase);
	_SPIFlash_TxRx_Byte((T_U8)((EraseAddr & 0xFF0000) >> 16));
	_SPIFlash_TxRx_Byte((T_U8)((EraseAddr & 0xFF00) >> 8));
	_SPIFlash_TxRx_Byte((T_U8)(EraseAddr & 0xFF));

	SPI_FLASH_CS_H();
	
	_SPIFlash_WaitForWriteEnd();
	
	_SPIFlash_Write_Dis();
}

/************************************************
 *功能：擦除一个块(64K)
 *输入：要擦除的块的首地址
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
T_VOID _SPIFlash_BlockErase(T_U32 EraseAddr)
{
	_SPIFlash_Write_En();

	SPI_FLASH_CS_L();
	
	_SPIFlash_TxRx_Byte(W25X_BlockErase); 
	_SPIFlash_TxRx_Byte((T_U8)((EraseAddr & 0xFF0000) >> 16));
	_SPIFlash_TxRx_Byte((T_U8)((EraseAddr & 0xFF00) >> 8));
	_SPIFlash_TxRx_Byte((T_U8)(EraseAddr & 0xFF));

	SPI_FLASH_CS_H();
	
	_SPIFlash_WaitForWriteEnd();
	
	_SPIFlash_Write_Dis();
}


/************************************************
 *功能：整片擦除
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
static void _SPIFlash_ChipErase(void)
{
	_SPIFlash_Write_En();

	SPI_FLASH_CS_L();

	_SPIFlash_TxRx_Byte(W25X_ChipErase);

	SPI_FLASH_CS_H();
	
	_SPIFlash_WaitForWriteEnd();

	_SPIFlash_Write_Dis();
}

/************************************************
 *功能：读取Device ID
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
static T_U16 _SPIFlash_ReadDeviceID(T_VOID)
{
	T_U16 DeviceID = 0;
	
	SPI_FLASH_CS_L();

	_SPIFlash_TxRx_Byte(W25X_UniqueID);

	_SPIFlash_TxRx_Byte(0x00);

	_SPIFlash_TxRx_Byte(0x00);

	_SPIFlash_TxRx_Byte(0x00);

	DeviceID = (_SPIFlash_TxRx_Byte(0xff) << 8);
	DeviceID |= _SPIFlash_TxRx_Byte(0xff);

	SPI_FLASH_CS_H();

	return DeviceID;

}

/**************************************************************
 *功能：写数据到外部flash
 *输入：u32Address要写的地址,
 			pBuf要写入的数据,
 			u32NbrToWrite要写的字节数
 *输出：
 *条件：要写入的数据不能为空
 *返回：成功:写入的字节数 
 			失败:0 
 注意:
***************************************************************/
static T_U32 _SPIFlash_Write(T_U32 u32Address, T_U8 *pBuf, T_U32 u32NbrToWrite) 
{	
	T_U32 iNumByteToWrite = u32NbrToWrite;
	T_U32 u32SectorPos = 0;
	T_U32 u32PagePos = 0;
	T_U32 u32PageOff = 0;
	T_U32 u32PageRemain = 0;
	T_U32 u32PageAddr = 0;
	T_U32 i = 0, j = 0;   

	T_S8 s8Buf[W25Q64_PAGE_SIZE] = {0};
	
	ASSERT_EQUAL_RETVAL(pBuf, T_NULL, 0);

	while(1)
	{
		/* 计算一些地址和关键值 */
		u32SectorPos = u32Address & (~(W25Q64_SECTOR_SIZE - 1));	//Sector地址 
		u32PagePos = u32Address & (~(W25Q64_PAGE_SIZE - 1));		//page地址 
		u32PageOff = u32Address & (W25Q64_PAGE_SIZE - 1);			//在page内的偏移
		u32PageRemain = W25Q64_PAGE_SIZE - u32PageOff; 				//page剩余空间大小 
		if(iNumByteToWrite <= u32PageRemain) 
		{
			u32PageRemain = iNumByteToWrite;				
		}
        
		/* 读出对应扇区全部数据存到缓冲区*/
		_SPIFlash_SectorErase((T_U32)FLASH_CACHE_START);
		for(i=0; i<(W25Q64_SECTOR_SIZE/W25Q64_PAGE_SIZE); i++)
		{
			memset(s8Buf, 0, W25Q64_PAGE_SIZE);
			_SPIFlash_ReadBuf((T_S8*)&s8Buf[0], W25Q64_PAGE_SIZE, u32SectorPos+i*W25Q64_PAGE_SIZE);
			_SPIFlash_WriteBuf((T_S8*)&s8Buf[0], W25Q64_PAGE_SIZE, FLASH_CACHE_START+i*W25Q64_PAGE_SIZE);
		}
		_SPIFlash_SectorErase(u32SectorPos);
			
		/* 从缓冲区读出数据修改对应位置后写入对应扇区 */
		for(i=0; i<(W25Q64_SECTOR_SIZE/W25Q64_PAGE_SIZE); i++)
		{
			u32PageAddr = u32SectorPos + i*W25Q64_PAGE_SIZE;
			
			memset(s8Buf, 0, W25Q64_PAGE_SIZE);
			_SPIFlash_ReadBuf((T_S8*)&s8Buf[0], W25Q64_PAGE_SIZE, FLASH_CACHE_START+i*W25Q64_PAGE_SIZE);

			if(u32PageAddr == u32PagePos)
			{
				for(j=0; j<u32PageRemain; j++) 
				{  //复制
					s8Buf[j+u32PageOff] = pBuf[j];  
				}

				if(iNumByteToWrite != u32PageRemain)
				{
					u32PagePos += W25Q64_PAGE_SIZE;
					u32PageOff = 0; 							//偏移位置为0 
					pBuf += u32PageRemain;						//指针偏移
					u32Address += u32PageRemain;				//写地址偏移 
					iNumByteToWrite -= u32PageRemain;			//字节数递减
					if(iNumByteToWrite > W25Q64_PAGE_SIZE) 
					{
						u32PageRemain = W25Q64_PAGE_SIZE;		//下一个扇区还是写不完
					}else
					{
						u32PageRemain = iNumByteToWrite;		//下一个扇区可以写完了
					}
				}
			}
			_SPIFlash_WriteBuf((T_S8*)&s8Buf[0], W25Q64_PAGE_SIZE, u32SectorPos+i*W25Q64_PAGE_SIZE);
		}
		
		if(iNumByteToWrite == u32PageRemain)
		{
			break;	  
		}
	}

	return u32NbrToWrite; 
}


/************************************************
 *功能：打开外部SPI Flash驱动
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
static T_S32 DRIVER_SPIFlash_Open(T_VOID)
{
	T_U16 u16DevID = 0;
	T_S8 s8Flag = 0;
	static T_U8 u8IsOpen = T_FALSE;

	if(T_FALSE == u8IsOpen)
	{
		_SPIFlash_init();
		
		u16DevID = _SPIFlash_ReadDeviceID();	//读取Flash ID
		
		if(0 != u16DevID)
		{
			_SPIFlash_ReadBuf((T_S8*)&s8Flag, 1, W25Q64_BLOCK_SIZE*127+W25Q64_SECTOR_SIZE*15+W25Q64_PAGE_SIZE*15);  
			if(s8Flag != 0xA5)
			{
				_SPIFlash_ChipErase(); // 这里擦除整个flash，大概要十几秒时间。

				s8Flag = 0xA5;
				//第127块的第15个扇区的第15个page(最后一个page)，作为判断芯片初始化的标识
				_SPIFlash_WriteBuf((T_S8*)&s8Flag, 1, W25Q64_BLOCK_SIZE*127+W25Q64_SECTOR_SIZE*15+W25Q64_PAGE_SIZE*15);
			}
		}

		u8IsOpen = T_TRUE;
	}
	
	return RET_SUCCESS;
}

/************************************************
 *功能：打开外部SPI Flash驱动
 *输入：s32Cmd命令字
 			pData控制数据
 *输出：无
 *条件：无
 *返回：无
 注意:n
*************************************************/
static T_S32 DRIVER_SpiFlash_Ioctl(T_S32 s32Cmd, T_VOID* pData)
{
	T_FlashCmd enCmd = (T_FlashCmd)s32Cmd;
	T_FlashCtlData *pstData = (T_FlashCtlData*)pData;
		
	T_S32 s32Ret = RET_FAILED;
	T_U32 u32DestAddr = 0x00000000;
	T_U32 u32EndAddr = 0x00000000;
	T_U32 u8ErashPageCnt = 0;
	
	ASSERT_GE_RETVAL(enCmd, FLASH_CMD_CNT, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pstData, T_NULL, RET_FAILED);

	/*	得到要写入或读出数据的实际地址。*/
	u32DestAddr = (T_U32)FLASH_START_ADDR + pstData->u32OffsetAddr;
	u32EndAddr = u32DestAddr + pstData->u32Len;
#if 0
	if(u32EndAddr > (T_U32)FLASH_END_ADDR)	//这里有警告
	{
		return RET_FAILED;
	}
#else
	u32EndAddr = u32EndAddr;
#endif

	/*	如果获得可用的地址就检测命令，做对应操作*/
	switch(enCmd) 
	{
		case FLASH_CMD_READ:
		{
			ASSERT_EQUAL_RETVAL(pstData->pu8DataBuf, T_NULL, RET_FAILED);
			_SPIFlash_ReadBuf((T_S8*)pstData->pu8DataBuf, pstData->u32Len, u32DestAddr);
			s32Ret = RET_SUCCESS;
		}
		break;

		case FLASH_CMD_WRITE:
		{
			ASSERT_EQUAL_RETVAL(pstData->pu8DataBuf, T_NULL, RET_FAILED);
			if(_SPIFlash_Write(u32DestAddr, pstData->pu8DataBuf, pstData->u32Len) == pstData->u32Len);
			{
				s32Ret = RET_SUCCESS;
			}
		}
		break;

		case FLASH_CMD_ERASH:
		{	
			u8ErashPageCnt = pstData->u32Len/W25Q64_SECTOR_SIZE;
			if((pstData->u32Len%W25Q64_SECTOR_SIZE) != 0)
			{
				u8ErashPageCnt += 1;
			}

			while(u8ErashPageCnt--)
			{
				_SPIFlash_SectorErase(u32DestAddr);
				u32DestAddr += W25Q64_SECTOR_SIZE;
			}
			s32Ret = RET_SUCCESS;
		}
		break;

		default:
			break;
	}
	
	return s32Ret;
}

/************************************************
 *功能：将SPI Flash驱动模块添加到
 			驱动列表中。
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*************************************************/
T_S32 DRIVER_SpiFlash_Init(T_VOID)
{
	FileOperation stSPIFlashOperation;

	/* 添加驱动模块 */
	stSPIFlashOperation.FileOpen = DRIVER_SPIFlash_Open;
	stSPIFlashOperation.FileRead = T_NULL;
	stSPIFlashOperation.FileWrite = T_NULL;
	stSPIFlashOperation.FileClose = T_NULL;
	stSPIFlashOperation.FileIoctl = DRIVER_SpiFlash_Ioctl;
	stSPIFlashOperation.FilePop = T_NULL;
	stSPIFlashOperation.FileRelease = T_NULL;
	memset(stSPIFlashOperation.s8ModelName, 0, MODEL_NAME_LEN);
	strcpy(stSPIFlashOperation.s8ModelName, "exFLASH");
	
	return MDL_FILE_AddModel((FileOperation*)&stSPIFlashOperation);
}


#endif


