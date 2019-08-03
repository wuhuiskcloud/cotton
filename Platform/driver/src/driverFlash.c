/*********************************************
* @文件: driverFlash.c
* @作者: Li Min
* @版本: v1.0.1
* @时间: 2017-12-4
* @概要: MCU内部flash驱动接口的实现
*********************************************/

#include "driverFlash.h"
#include "stm32f10x_flash.h"
#include "driverFile.h"


static T_U8 MCUFlashPageBuf[FLASH_PAGE_SIZE] = {0,};

/**************************************************************
 *功能：读MCU内部flash
 *输入：要读的地址,数据缓冲区,要读的字节数
 *输出：读到的数据
 *条件：数据缓冲区不能为空
 *返回：成功:读到的字节数 
 			失败:0 
 注意:
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
 *功能：写半字到MCU内部flash
 *输入：u32Address要写的地址,
 			pBuf要写入的数据,
 			u16NumByteToWrite要写的字节数
 *输出：
 *条件：要写入的数据不能为空
 *返回：成功:写入的字节数 
 			失败:0 
 注意:
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
 *功能：写数据到MCU内部flash
 *输入：u32Address要写的地址,
 			pBuf要写入的数据,
 			u32NbrToWrite要写的字节数
 *输出：
 *条件：要写入的数据不能为空
 *返回：成功:写入的字节数 
 			失败:0 
 注意:
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

    u32PagePos  =  u32Address  &  (~(FLASH_PAGE_SIZE  -  1))  ;	//page的地址  取2048的整数倍
    u32PageOff  =  u32Address  &  (FLASH_PAGE_SIZE  -  1);	//在page内的偏移  取2048的余数
    u32PageRemain  =  FLASH_PAGE_SIZE  -  u32PageOff;  //page剩余空间大小 

    if(iNumByteToWrite  <=  u32PageRemain)  
    {
        u32PageRemain  =  iNumByteToWrite;	
    }

    while(1)  
    {
        _MCUFlash_Read(u32PagePos,  MCUFlashPageBuf,  FLASH_PAGE_SIZE);  //读出整个page 
        
        for(i=0;  i<u32PageRemain;  i++)  
        {    //复制
            MCUFlashPageBuf[i+u32PageOff]  =  pBuf[i];    
        }

        u16TimeOut  =  0;
        
        while(1)
        {
            FLASH_ClearFlag(FLASH_FLAG_EOP  |  FLASH_FLAG_PGERR  |  FLASH_FLAG_WRPRTERR);  

            FLASHStatus  =  FLASH_ErasePage(u32PagePos);	//擦除扇区
            
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
        {	//写入结束了
            break;
        }else  
        {
            u32PagePos  +=  FLASH_PAGE_SIZE;
            u32PageOff  =  0;  // 偏移位移为0
            pBuf  +=  u32PageRemain;	//指针偏移
            u32Address  +=  u32PageRemain;	//写地址偏移     
            iNumByteToWrite  -=  u32PageRemain;	//字节数递减
            
            if(iNumByteToWrite  >  FLASH_PAGE_SIZE)  
            {
                u32PageRemain  =  FLASH_PAGE_SIZE;	//下一个扇区还是写不完
            }else
            {
                u32PageRemain  =  iNumByteToWrite;	//下一个扇区可以写完了
            }
        }
    }

    FLASH_Lock();
    //__ENABLE_IRQ();
	__set_PRIMASK(0);
    return  u32NbrToWrite;  
}
/**************************************************************
 *功能：写半字到MCU内部flash
 *输入：要擦除的page首地址,要擦除的page数
 *输出：
 *条件：u32Address必须是page的首地址
 *返回：成功:FLASH_COMPLETE
 			失败:flash错误信息 
 注意:
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
 *功能：解锁Flash操作
 *输入：无
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 			失败:RET_FAILED 
 注意:
*********************************************/
static T_S32 _FLASH_Open(T_VOID)
{
	//FLASH_Unlock();  

	return RET_SUCCESS;
}


/********************************************
 *功能：Flash控制
 *输入：s32Cmd指令:读/写/擦除
 			pvData指令参数
 *输出：如果是读命令数据存到pvData中
 *条件：无
 *返回：成功:RET_SUCCESS 
 			失败:RET_FAILED 
 注意: 擦除命令只关心数据块编号
 		其它参数不关心。
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

	/*	得到要写入或读出数据的实际地址。*/
	u32DestAddr = pstData->u32OffsetAddr;//FLASH_DB_START + pstData->u32OffsetAddr;
	u32EndAddr = u32DestAddr + pstData->u32Len;
	if(u32EndAddr > FLASH_DB_END)
	{
		return RET_FAILED;
	}

	/*	如果获得可用的地址就检测命令，做对应操作*/
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
 *功能：上锁Flash操作
 *输入：无
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 			失败:RET_FAILED 
 注意:
*********************************************/
static T_S32 _FLASH_Close(T_VOID)
{
	FLASH_Unlock();

	return RET_SUCCESS;
}

/************************************************
 *功能：将flash驱动模块添加到
 			驱动列表中。
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
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


