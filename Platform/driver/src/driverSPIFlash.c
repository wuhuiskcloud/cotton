/*********************************************
* @�ļ�: driverSPIFlash.c
* @����: Li Min
* @�汾: v1.0.1
* @ʱ��: 2017-12-27
* @��Ҫ: �ⲿSPI Flash�������ӿ� ��ʵ��
*********************************************/

//#include "driverSPIFlash.h"
#include "driverFile.h"
#include "ioctrl.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
//#include "driverIWDog.h"

#if 1
/* ===���IO�ڶ���===*/
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

/*===== W25Q64 �Ĵ�������=====*/
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

#define W25Q64_PAGE_SIZE      	FLASH_PAGE_SIZE						// һ��page 256 (Byte) 
#define W25Q64_SECTOR_SIZE      (W25Q64_PAGE_SIZE*16)	// 16��page���һ��sector: PAGE * 16 = 4K (Byte) //��������С������Ԫ
#define W25Q64_BLOCK_SIZE       (W25Q64_SECTOR_SIZE*16)	// 16��sector���һ��block: SECTOR * 16 = 64K (Byte)
#define W25Q64_CHIP_SIZE		(W25Q64_BLOCK_SIZE*128)	// 128��block�������flash�ռ�: BLOCK * 128 = 8M

/************************************************
 *���ܣ�SPI Flash��ӦSPI�ӿڵ����ų�ʼ��
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
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
 *���ܣ�SPI Flash��ӦSPI�ӿڹ���ģʽ����
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
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
 *���ܣ�SPI Flash��ʼ��
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*************************************************/
static T_VOID _SPIFlash_init(T_VOID)
{
	_SPIFlash_IO_Init();
	_SPIFlash_SPI_init();
}

/************************************************
 *���ܣ�ͨ��SPI���ͺͽ���һ���ֽ�
 *���룺Ҫ���͵�����
 *�������
 *��������
 *���أ�����������
 ע��:
*************************************************/
static T_U8 _SPIFlash_TxRx_Byte(T_U8 data)
{
	T_U32 timeout = 0;

	#if (1)
	timeout = 3000;
    while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET)
	{
	    DRIVER_IWDog_Write(T_NULL, 0);   //��ʱι����ֹӦ�ò��д�������ݿ��Ź���ʱ  
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
	    DRIVER_IWDog_Write(T_NULL, 0);   //��ʱι����ֹӦ�ó�д���ݿ��Ź���ʱ 
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
 *���ܣ�дʹ��
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*************************************************/	
static T_VOID _SPIFlash_Write_En(T_VOID)
{
	SPI_FLASH_CS_L(); 
	_SPIFlash_TxRx_Byte(W25X_WriteEnable);          
	SPI_FLASH_CS_H();  
}

/************************************************
 *���ܣ�д��ֹ
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*************************************************/
static T_VOID _SPIFlash_Write_Dis(T_VOID)
{
	SPI_FLASH_CS_L(); 
	_SPIFlash_TxRx_Byte(W25X_WriteDisable);          
	SPI_FLASH_CS_H();  
}

/************************************************
 *���ܣ��ȴ�д���
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*************************************************/
static void _SPIFlash_WaitForWriteEnd(void)
{
	T_U8 FLASH_Status = 0;
	
	SPI_FLASH_CS_L();
	
	_SPIFlash_TxRx_Byte(W25X_ReadStatusReg); //��ȡFlash ״̬�Ĵ���1
	do
	{
		FLASH_Status = _SPIFlash_TxRx_Byte(0x00);
	}while ((FLASH_Status & 0x01) == 0x01);//��û��д����һֱ�ȴ�������CS=0

	SPI_FLASH_CS_H();
}

/************************************************
 *���ܣ���ָ���ĵ�ַ��ȡָ���������ֽ�
 *���룺databuffer �������ݵĻ�����
 			datasize��������С
 			addressҪ���ĵ�ַ
 *�������
 *��������
 *���أ���
 ע��:
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
 *���ܣ���ָ���ĵ�ַд��ָ���������ֽ�     
 *���룺databuffer �������ݵĻ�����
 			datasize��������С
 			addressҪ���ĵ�ַ     
 *�������
 *��������
 *���أ���
 ע��:
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
 *���ܣ�����һ������(4K)
 *���룺Ҫ�����������׵�ַ
 *�������
 *��������
 *���أ���
 ע��:
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
 *���ܣ�����һ����(64K)
 *���룺Ҫ�����Ŀ���׵�ַ
 *�������
 *��������
 *���أ���
 ע��:
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
 *���ܣ���Ƭ����
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
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
 *���ܣ���ȡDevice ID
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
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
 *���ܣ�д���ݵ��ⲿflash
 *���룺u32AddressҪд�ĵ�ַ,
 			pBufҪд�������,
 			u32NbrToWriteҪд���ֽ���
 *�����
 *������Ҫд������ݲ���Ϊ��
 *���أ��ɹ�:д����ֽ��� 
 			ʧ��:0 
 ע��:
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
		/* ����һЩ��ַ�͹ؼ�ֵ */
		u32SectorPos = u32Address & (~(W25Q64_SECTOR_SIZE - 1));	//Sector��ַ 
		u32PagePos = u32Address & (~(W25Q64_PAGE_SIZE - 1));		//page��ַ 
		u32PageOff = u32Address & (W25Q64_PAGE_SIZE - 1);			//��page�ڵ�ƫ��
		u32PageRemain = W25Q64_PAGE_SIZE - u32PageOff; 				//pageʣ��ռ��С 
		if(iNumByteToWrite <= u32PageRemain) 
		{
			u32PageRemain = iNumByteToWrite;				
		}
        
		/* ������Ӧ����ȫ�����ݴ浽������*/
		_SPIFlash_SectorErase((T_U32)FLASH_CACHE_START);
		for(i=0; i<(W25Q64_SECTOR_SIZE/W25Q64_PAGE_SIZE); i++)
		{
			memset(s8Buf, 0, W25Q64_PAGE_SIZE);
			_SPIFlash_ReadBuf((T_S8*)&s8Buf[0], W25Q64_PAGE_SIZE, u32SectorPos+i*W25Q64_PAGE_SIZE);
			_SPIFlash_WriteBuf((T_S8*)&s8Buf[0], W25Q64_PAGE_SIZE, FLASH_CACHE_START+i*W25Q64_PAGE_SIZE);
		}
		_SPIFlash_SectorErase(u32SectorPos);
			
		/* �ӻ��������������޸Ķ�Ӧλ�ú�д���Ӧ���� */
		for(i=0; i<(W25Q64_SECTOR_SIZE/W25Q64_PAGE_SIZE); i++)
		{
			u32PageAddr = u32SectorPos + i*W25Q64_PAGE_SIZE;
			
			memset(s8Buf, 0, W25Q64_PAGE_SIZE);
			_SPIFlash_ReadBuf((T_S8*)&s8Buf[0], W25Q64_PAGE_SIZE, FLASH_CACHE_START+i*W25Q64_PAGE_SIZE);

			if(u32PageAddr == u32PagePos)
			{
				for(j=0; j<u32PageRemain; j++) 
				{  //����
					s8Buf[j+u32PageOff] = pBuf[j];  
				}

				if(iNumByteToWrite != u32PageRemain)
				{
					u32PagePos += W25Q64_PAGE_SIZE;
					u32PageOff = 0; 							//ƫ��λ��Ϊ0 
					pBuf += u32PageRemain;						//ָ��ƫ��
					u32Address += u32PageRemain;				//д��ַƫ�� 
					iNumByteToWrite -= u32PageRemain;			//�ֽ����ݼ�
					if(iNumByteToWrite > W25Q64_PAGE_SIZE) 
					{
						u32PageRemain = W25Q64_PAGE_SIZE;		//��һ����������д����
					}else
					{
						u32PageRemain = iNumByteToWrite;		//��һ����������д����
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
 *���ܣ����ⲿSPI Flash����
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*************************************************/
static T_S32 DRIVER_SPIFlash_Open(T_VOID)
{
	T_U16 u16DevID = 0;
	T_S8 s8Flag = 0;
	static T_U8 u8IsOpen = T_FALSE;

	if(T_FALSE == u8IsOpen)
	{
		_SPIFlash_init();
		
		u16DevID = _SPIFlash_ReadDeviceID();	//��ȡFlash ID
		
		if(0 != u16DevID)
		{
			_SPIFlash_ReadBuf((T_S8*)&s8Flag, 1, W25Q64_BLOCK_SIZE*127+W25Q64_SECTOR_SIZE*15+W25Q64_PAGE_SIZE*15);  
			if(s8Flag != 0xA5)
			{
				_SPIFlash_ChipErase(); // �����������flash�����Ҫʮ����ʱ�䡣

				s8Flag = 0xA5;
				//��127��ĵ�15�������ĵ�15��page(���һ��page)����Ϊ�ж�оƬ��ʼ���ı�ʶ
				_SPIFlash_WriteBuf((T_S8*)&s8Flag, 1, W25Q64_BLOCK_SIZE*127+W25Q64_SECTOR_SIZE*15+W25Q64_PAGE_SIZE*15);
			}
		}

		u8IsOpen = T_TRUE;
	}
	
	return RET_SUCCESS;
}

/************************************************
 *���ܣ����ⲿSPI Flash����
 *���룺s32Cmd������
 			pData��������
 *�������
 *��������
 *���أ���
 ע��:n
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

	/*	�õ�Ҫд���������ݵ�ʵ�ʵ�ַ��*/
	u32DestAddr = (T_U32)FLASH_START_ADDR + pstData->u32OffsetAddr;
	u32EndAddr = u32DestAddr + pstData->u32Len;
#if 0
	if(u32EndAddr > (T_U32)FLASH_END_ADDR)	//�����о���
	{
		return RET_FAILED;
	}
#else
	u32EndAddr = u32EndAddr;
#endif

	/*	�����ÿ��õĵ�ַ�ͼ���������Ӧ����*/
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
 *���ܣ���SPI Flash����ģ����ӵ�
 			�����б��С�
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*************************************************/
T_S32 DRIVER_SpiFlash_Init(T_VOID)
{
	FileOperation stSPIFlashOperation;

	/* �������ģ�� */
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


