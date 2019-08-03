/*********************************************
* @�ļ�: driverFlash.c
* @����: Li Min
* @�汾: v1.0.1
* @ʱ��: 2017-12-4
* @��Ҫ: MCU�ڲ�flash�����ӿڵ�ʵ��
*********************************************/

#include "driverFlash.h"
#include "stm32f10x_flash.h"
#include "driverFile.h"


static T_U8 MCUFlashPageBuf[FLASH_PAGE_SIZE] = {0,};

/**************************************************************
 *���ܣ���MCU�ڲ�flash
 *���룺Ҫ���ĵ�ַ,���ݻ�����,Ҫ�����ֽ���
 *���������������
 *���������ݻ���������Ϊ��
 *���أ��ɹ�:�������ֽ��� 
 			ʧ��:0 
 ע��:
***************************************************************/
static T_U32 _MCUFlash_Read(T_U32 u32Address, T_U8 *pBuf, T_U32 u32NbrToRead) 
{
	T_U32 i = 0;

	ASSERT_EQUAL_RETVAL(pBuf, T_NULL, 0);
	
	while(i < u32NbrToRead) 
	{
		*(pBuf + i) = *(__IO T_U8*)u32Address++;
		i++;
	}
		
	return i;
}

/**************************************************************
 *���ܣ�д���ֵ�MCU�ڲ�flash
 *���룺u32AddressҪд�ĵ�ַ,
 			pBufҪд�������,
 			u16NumByteToWriteҪд���ֽ���
 *�����
 *������Ҫд������ݲ���Ϊ��
 *���أ��ɹ�:д����ֽ��� 
 			ʧ��:0 
 ע��:
***************************************************************/
static T_U16 _Flash_Write_Without_check(T_U32 u32Address, T_U8 *pBuf, T_U16 u16NumByteToWrite) 
{
	T_U16 i = 0;
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

	ASSERT_EQUAL_RETVAL(pBuf, T_NULL, 0);
	
	while((i < u16NumByteToWrite) && (FLASHStatus == FLASH_COMPLETE))
	{
		FLASHStatus = FLASH_ProgramHalfWord(u32Address, *(T_U16*)pBuf);
		if(FLASHStatus != FLASH_COMPLETE) 
		{
			return 0;
		}
		i = i+2;
		u32Address = u32Address + 2;
		pBuf = pBuf + 2;
	}

	return i;
}

/**************************************************************
 *���ܣ�д���ݵ�MCU�ڲ�flash
 *���룺u32AddressҪд�ĵ�ַ,
 			pBufҪд�������,
 			u32NbrToWriteҪд���ֽ���
 *�����
 *������Ҫд������ݲ���Ϊ��
 *���أ��ɹ�:д����ֽ��� 
 			ʧ��:0 
 ע��:
***************************************************************/
static T_U32 _MCUFlash_Write(T_U32 u32Address, T_U8 *pBuf, T_U32 u32NbrToWrite) 
{	
    T_U32  iNumByteToWrite  =  u32NbrToWrite;
    T_U32  u32PagePos  =  0;
    T_U32  u32PageOff  =  0;
    T_U32  u32PageRemain  =  0;    
    T_U32  i  =  0;        
    volatile  FLASH_Status  FLASHStatus  =  FLASH_COMPLETE;      
    T_U16  u16TimeOut  =  0;
    T_U16  u16WriteRet  =  0;

    ASSERT_EQUAL_RETVAL(pBuf,  T_NULL,  0);

    //__DISABLE_IRQ();
	__set_PRIMASK(1);
    FLASH_Unlock();
      
    FLASH_ClearFlag(FLASH_FLAG_EOP  |  FLASH_FLAG_PGERR  |  FLASH_FLAG_WRPRTERR);  

    u32PagePos  =  u32Address  &  (~(FLASH_PAGE_SIZE  -  1))  ;	//page�ĵ�ַ  ȡ2048��������
    u32PageOff  =  u32Address  &  (FLASH_PAGE_SIZE  -  1);	//��page�ڵ�ƫ��  ȡ2048������
    u32PageRemain  =  FLASH_PAGE_SIZE  -  u32PageOff;  //pageʣ��ռ��С 

    if(iNumByteToWrite  <=  u32PageRemain)  
    {
        u32PageRemain  =  iNumByteToWrite;	
    }

    while(1)  
    {
        _MCUFlash_Read(u32PagePos,  MCUFlashPageBuf,  FLASH_PAGE_SIZE);  //��������page 
        
        for(i=0;  i<u32PageRemain;  i++)  
        {    //����
            MCUFlashPageBuf[i+u32PageOff]  =  pBuf[i];    
        }

        u16TimeOut  =  0;
        
        while(1)
        {
            FLASH_ClearFlag(FLASH_FLAG_EOP  |  FLASH_FLAG_PGERR  |  FLASH_FLAG_WRPRTERR);  

            FLASHStatus  =  FLASH_ErasePage(u32PagePos);	//��������
            
            if(FLASHStatus  !=  FLASH_COMPLETE)  
            {
                u16TimeOut++;
                if(u16TimeOut  >=  10)
                {
                    break;
                }
                
               // print_err("Erash  flash  timeout:  %d(status: %d).\r\n",  u16TimeOut, FLASHStatus);
                continue;
                
            }else
            {
                u16WriteRet  =  0;
                u16WriteRet  =  _Flash_Write_Without_check(u32PagePos,  MCUFlashPageBuf,  FLASH_PAGE_SIZE);
                
                if(u16WriteRet  <  FLASH_PAGE_SIZE)
                {
                    u16TimeOut++;
                    if(u16TimeOut  >=  10)
                    {
                        break;
                    }
                    
                   //` print_err("Write  flash  timeout:  %d(return: %d).\r\n",  u16TimeOut, u16WriteRet);
                }else
                {
                    break;
                }
            }
        }

        if(iNumByteToWrite  ==  u32PageRemain)  
        {	//д�������
            break;
        }else  
        {
            u32PagePos  +=  FLASH_PAGE_SIZE;
            u32PageOff  =  0;  // ƫ��λ��Ϊ0
            pBuf  +=  u32PageRemain;	//ָ��ƫ��
            u32Address  +=  u32PageRemain;	//д��ַƫ��     
            iNumByteToWrite  -=  u32PageRemain;	//�ֽ����ݼ�
            
            if(iNumByteToWrite  >  FLASH_PAGE_SIZE)  
            {
                u32PageRemain  =  FLASH_PAGE_SIZE;	//��һ����������д����
            }else
            {
                u32PageRemain  =  iNumByteToWrite;	//��һ����������д����
            }
        }
    }

    FLASH_Lock();
    //__ENABLE_IRQ();
	__set_PRIMASK(0);
    return  u32NbrToWrite;  
}
/**************************************************************
 *���ܣ�д���ֵ�MCU�ڲ�flash
 *���룺Ҫ������page�׵�ַ,Ҫ������page��
 *�����
 *������u32Address������page���׵�ַ
 *���أ��ɹ�:FLASH_COMPLETE
 			ʧ��:flash������Ϣ 
 ע��:
***************************************************************/
static FLASH_Status _MCUFlash_Erash(T_U32 u32Address, T_U32 u32PageToErash)
{
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
	T_U32 u32Page = u32PageToErash;
	T_U32 u32Addr = u32Address;

	if((u32Page == 0) || ((u32Addr%FLASH_PAGE_SIZE) != 0) \
		|| ((u32Addr+u32Page*FLASH_PAGE_SIZE-1)>FLASH_END_ADDR)) 
	{
		return FLASH_ERROR_PG;
	}

	while(u32Page)
	{
		FLASHStatus = FLASH_ErasePage(u32Addr);
		if(FLASHStatus == FLASH_COMPLETE)
		{
			u32Addr += FLASH_PAGE_SIZE;
			u32Page--;	
		}else
		{
			break;
		}
	}
    
     FLASH_Lock();
    //__ENABLE_IRQ();
	__set_PRIMASK(0);
	return FLASHStatus;
}


/********************************************
 *���ܣ�����Flash����
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 			ʧ��:RET_FAILED 
 ע��:
*********************************************/
static T_S32 _FLASH_Open(T_VOID)
{
	//FLASH_Unlock();  

	return RET_SUCCESS;
}


/********************************************
 *���ܣ�Flash����
 *���룺s32Cmdָ��:��/д/����
 			pvDataָ�����
 *���������Ƕ��������ݴ浽pvData��
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 			ʧ��:RET_FAILED 
 ע��: ��������ֻ�������ݿ���
 		�������������ġ�
*********************************************/
static T_S32 _FLASH_Ioctl(T_S32 s32Cmd,  T_VOID* pData)
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
	u32DestAddr = pstData->u32OffsetAddr;//FLASH_DB_START + pstData->u32OffsetAddr;
	u32EndAddr = u32DestAddr + pstData->u32Len;
	if(u32EndAddr > FLASH_DB_END)
	{
		return RET_FAILED;
	}

	/*	�����ÿ��õĵ�ַ�ͼ���������Ӧ����*/
	if(u32DestAddr != 0x00000000)
	{
		switch(enCmd) 
		{
			case FLASH_CMD_READ:
			{
				ASSERT_EQUAL_RETVAL(pstData->pu8DataBuf, T_NULL, RET_FAILED);
				if(_MCUFlash_Read(u32DestAddr, pstData->pu8DataBuf, pstData->u32Len) == pstData->u32Len)
				{
					s32Ret = RET_SUCCESS;
				}
			}
			break;

			case FLASH_CMD_WRITE:
			{
				ASSERT_EQUAL_RETVAL(pstData->pu8DataBuf, T_NULL, RET_FAILED);
				if(_MCUFlash_Write(u32DestAddr, pstData->pu8DataBuf, pstData->u32Len) == pstData->u32Len);
				{
					s32Ret = RET_SUCCESS;
				}
			}
			break;

			case FLASH_CMD_ERASH:
			{	
				u8ErashPageCnt = pstData->u32Len/FLASH_PAGE_SIZE;
                if((pstData->u32Len%FLASH_PAGE_SIZE) != 0)
                {
                    u8ErashPageCnt += 1;
                }
                
				if(_MCUFlash_Erash(u32DestAddr, u8ErashPageCnt) == FLASH_COMPLETE)
				{
					s32Ret = RET_SUCCESS;
				}
			}
			break;

			default:
				break;
		}
	}
	
	return s32Ret;
}


/********************************************
 *���ܣ�����Flash����
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 			ʧ��:RET_FAILED 
 ע��:
*********************************************/
static T_S32 _FLASH_Close(T_VOID)
{
	FLASH_Unlock();

	return RET_SUCCESS;
}

/************************************************
 *���ܣ���flash����ģ����ӵ�
 			�����б��С�
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*************************************************/
T_S32 DRIVER_FLASH_Init(T_VOID)
{
	FileOperation stFlashOperation;
	
	stFlashOperation.FileOpen = _FLASH_Open;
	stFlashOperation.FileRead = T_NULL;
	stFlashOperation.FileWrite = T_NULL;
	stFlashOperation.FileClose = _FLASH_Close;
	stFlashOperation.FileIoctl = _FLASH_Ioctl;
	stFlashOperation.FilePop = T_NULL;
	stFlashOperation.FileRelease = T_NULL;
	memset(stFlashOperation.s8ModelName, 0, MODEL_NAME_LEN);
	strcpy(stFlashOperation.s8ModelName, "FLASH");
	
	return MDL_FILE_AddModel((FileOperation*)&stFlashOperation);
}


