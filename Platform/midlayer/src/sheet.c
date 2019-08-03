/*********************************************
* @文件: sheet.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-5-30
* @概要: 提供数据库操作增删改查
                   接口以及flash的管理。
*********************************************/
#include "sheet.h"
#include "driverFlash.h"
#include "driverManager.h"

#define SHEET_NAME_LEN 8

#define SHEET_FLASH_SIZE (1024*2)   //0x400000 //数据存储区的大小(4096*15)
#define SHEET_PAGE_SIZE  1024     //4096 //数据存储区最小操作大小
#define SHEET_HEAD_SIZE  1024    //4096 //数据记录信息存储区大小

#define VALID_FLAG 0xFD //新纪录设为有效
#define INVALID_FLAG 0xFE //删除后的数据记录为无效
#define END_FLAG 0xFF //到了文件末位

//数据库操作命令
typedef enum tagSheetOperater
{
	CREATE,//创建表
	INSERT,//添加记录
	QUERY, //查找记录
	UPDATE,//更新记录
	DELETE,//删除记录
	DESTROY,//删除表
}SheetOperater;

//创建数据库表信息
typedef struct tagSheetHead
{
	T_U8 u8IsValid; //有效标志
	T_U8 u8SheetId; //数据表ID，注意从0开始，一定要唯一的连续的,用作计算每一张表的偏移地址
	T_U16 u16RecordLen; //一条记录的大小btype	
	T_U32 u32SheetOffAddr; //数据表偏移地址
	T_U32 u32SheetSize; //数据表flash空间大小btype
}SheetHead;

static T_S32 Flash_crtl(T_S32 s32DbModuleId, SheetOperater enOperater, T_U32 u32OffAddr, T_U8 *pu8Data, T_U32 u32Len)
{	
	T_FlashCtlData stFlashData;
	T_FlashCmd enFlashCmd = FLASH_CMD_CNT;

	stFlashData.u32OffsetAddr = u32OffAddr + FLASH_DB_START;
	stFlashData.pu8DataBuf = pu8Data;
	stFlashData.u32Len = u32Len;

	if(QUERY == enOperater)
	{
		enFlashCmd = FLASH_CMD_READ;
	}else if(UPDATE == enOperater || INSERT == enOperater || DELETE== enOperater)
	{
		enFlashCmd = FLASH_CMD_WRITE;
	}else if(DESTROY == enOperater)
	{
		enFlashCmd = FLASH_CMD_ERASH;
	}
	
	return MDL_DRIVERMGR_Ioctl(s32DbModuleId, enFlashCmd, &stFlashData);
}

static SheetPoint *_SHEET_GetSheetPoint(SheetHandle *pstSheetHandle, T_U8 u8SheetId)
{	
	T_U8 i = 0;
	SheetPoint *pstSheepPoint = T_NULL;

	for(i=0; i<pstSheetHandle->u8SheetCount; i++)
	{
		pstSheepPoint = &pstSheetHandle->pstSheetPoint[i];

		if(pstSheepPoint->u8SheetId == u8SheetId)
		{
			return pstSheepPoint;
		}
	}

	return T_NULL;
}

static T_S32 _SHEET_QueryHead(T_S32 s32ModelId, T_U8 u8SheetId, SheetHead *pstSheetHead)
{	
	T_U16 u16HeadSizeCount = 0;

	do{		
		memset(pstSheetHead, 0, sizeof(SheetHead));
		
		if(RET_SUCCESS != Flash_crtl(s32ModelId, QUERY, u16HeadSizeCount, (T_U8 *)pstSheetHead, sizeof(SheetHead)))
				return RET_FAILED;

		//遍历全部没有数据表
		if(END_FLAG == pstSheetHead->u8IsValid)
		{		
			break;
		}else if(VALID_FLAG == pstSheetHead->u8IsValid) //查找表信息
		{
			//找到相关表
			if(pstSheetHead->u8SheetId == u8SheetId)
			{
				return RET_SUCCESS;
			}
		}
		
		u16HeadSizeCount = u16HeadSizeCount + sizeof(SheetHead);
		
	}while(u16HeadSizeCount < SHEET_HEAD_SIZE);
	
	return RET_FAILED;
}

static T_VOID _SHEET_Initial(SheetHandle *pstSheetHandle)
{
	T_U16 u16HeadSizeCount = 0;
	SheetHead stSheetHead;
	SheetPoint stSheetPoint;

	do{		
		memset(&stSheetHead, 0, sizeof(SheetHead));
		
		if(RET_SUCCESS != Flash_crtl(pstSheetHandle->s32ModelId, QUERY, u16HeadSizeCount, (T_U8 *)&stSheetHead, (T_U32)sizeof(SheetHead)))
		{
			pstSheetHandle->eStatus = ST_UNKNOWN;
			return ;
		}
		
		//到结尾退出
		if(END_FLAG == stSheetHead.u8IsValid)
		{		
			break;
		}else if(VALID_FLAG == stSheetHead.u8IsValid)
		{
			if(0 == pstSheetHandle->u8SheetCount)
			{
				pstSheetHandle->pstSheetPoint = (SheetPoint *)malloc(sizeof(SheetPoint));
			}else
			{
				SheetPoint *pstSheetPoint = (SheetPoint *)realloc(pstSheetHandle->pstSheetPoint, (pstSheetHandle->u8SheetCount+1)*sizeof(SheetPoint));
				pstSheetHandle->pstSheetPoint = pstSheetPoint;
			}

			ASSERT_EQUAL_RETURN(pstSheetHandle->pstSheetPoint, T_NULL);

			stSheetPoint.u32CurPoint = 0;
			stSheetPoint.u8SheetId = stSheetHead.u8SheetId;
			memcpy(&pstSheetHandle->pstSheetPoint[pstSheetHandle->u8SheetCount], &stSheetPoint, sizeof(SheetPoint));
			
			pstSheetHandle->u8SheetCount++;
		}
		
		u16HeadSizeCount = u16HeadSizeCount + sizeof(SheetHead);
	}while(SHEET_HEAD_SIZE - u16HeadSizeCount > sizeof(SheetHead));	
}

static T_S32 _SHEET_Create(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U16 u16RecordLen, T_U32 u32SheetSize)
{
	T_U16 u16RecordOffset = 0;
	T_U32 u32SheetOffAddr = 0;
	T_U16 u16HeadSizeCount = 0;
	T_U32 u32TempCount = 0;
	
	SheetHead stSheetHead;
	
	do{		
		memset(&stSheetHead, 0, sizeof(SheetHead));
		
		if(RET_SUCCESS != Flash_crtl(pstSheetHandle->s32ModelId, QUERY, u16HeadSizeCount, (T_U8 *)&stSheetHead, (T_U32)sizeof(SheetHead)))
		{
			pstSheetHandle->eStatus = ST_UNKNOWN;
			return RET_FAILED;
		}
		
		//记录无效
		if(INVALID_FLAG == stSheetHead.u8IsValid || END_FLAG == stSheetHead.u8IsValid)
		{	
			if(0 == u16RecordOffset) //当大于0说明已找到无效数据记录
			{
				u16RecordOffset = u16HeadSizeCount + 1;
			}

			//到结尾退出
			if(END_FLAG == stSheetHead.u8IsValid)
			{		
				break;
			}
		}

		if(VALID_FLAG == stSheetHead.u8IsValid)
		{
			//计算新建表的偏移地址
			u32TempCount = stSheetHead.u32SheetOffAddr + stSheetHead.u32SheetSize;

			//找出最后一个表的地址
			if(u32TempCount > u32SheetOffAddr)
			{
				u32SheetOffAddr = u32TempCount;
			}
			
			//表存在不再创建
			if(stSheetHead.u8SheetId == u8SheetId)
			{
				return RET_SUCCESS;
			}
		}
		
		u16HeadSizeCount = u16HeadSizeCount + sizeof(SheetHead);
	}while(SHEET_HEAD_SIZE - u16HeadSizeCount > sizeof(SheetHead));

	if(u16HeadSizeCount >= SHEET_HEAD_SIZE)
	{
		pstSheetHandle->eStatus = ST_EOF;
		return RET_FAILED;
	}

	if(0 == u32SheetOffAddr)
	{
		u32SheetOffAddr = SHEET_HEAD_SIZE;
	}
	
	stSheetHead.u8SheetId = u8SheetId;
	stSheetHead.u8IsValid = VALID_FLAG;
	stSheetHead.u32SheetOffAddr = u32SheetOffAddr;
	stSheetHead.u32SheetSize  = u32SheetSize;
	stSheetHead.u16RecordLen = u16RecordLen + 1;
				
	if(RET_SUCCESS != Flash_crtl(pstSheetHandle->s32ModelId, INSERT, u16RecordOffset - 1, (T_U8 *)&stSheetHead, sizeof(SheetHead)))
	{
		pstSheetHandle->eStatus = ST_NOWRITE;
		return RET_FAILED;
	}
	
	return RET_SUCCESS;
}

static T_S32 _SHEET_Insert(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U8 *pu8Data, T_S32 s32Len)
{
	T_U32 u32SheetSizeCount = 0;
	T_U8 *pu8TempData = T_NULL;
	T_U32 u32SheetOffAddr = 0;
	T_S32 s32Ret = RET_FAILED;
	SheetHead stSheetHead;

	if(RET_SUCCESS != _SHEET_QueryHead(pstSheetHandle->s32ModelId, u8SheetId, &stSheetHead))
	{
		pstSheetHandle->eStatus = ST_NOTABLE;
		return RET_FAILED;
	}
	
	if(s32Len != (stSheetHead.u16RecordLen - 1))
	{
		pstSheetHandle->eStatus = ST_FORMATE;
		return RET_FAILED;
	}

	pu8TempData = (T_U8 *)malloc(stSheetHead.u16RecordLen);
		
	ASSERT_EQUAL_RETVAL(pu8TempData, T_NULL, RET_FAILED);
		
	while(stSheetHead.u32SheetSize - u32SheetSizeCount > stSheetHead.u16RecordLen)
	{
		u32SheetOffAddr = u32SheetSizeCount + stSheetHead.u32SheetOffAddr;

		memset(pu8TempData, 0, stSheetHead.u16RecordLen);
		
		if(RET_SUCCESS != Flash_crtl(pstSheetHandle->s32ModelId, QUERY, u32SheetOffAddr, pu8TempData, stSheetHead.u16RecordLen))
		{						
			s32Ret = RET_FAILED;
			pstSheetHandle->eStatus = ST_UNKNOWN;
			break;
		}

		//找到一个空位置
		if(VALID_FLAG != pu8TempData[0])
		{
			memset(pu8TempData, 0, stSheetHead.u16RecordLen);
			
			pu8TempData[0] = VALID_FLAG;
			
			memcpy(&pu8TempData[1], pu8Data, stSheetHead.u16RecordLen - 1);
			s32Ret = Flash_crtl(pstSheetHandle->s32ModelId, INSERT, u32SheetOffAddr, pu8TempData, stSheetHead.u16RecordLen);

			if(s32Ret != RET_SUCCESS)
			{
				pstSheetHandle->eStatus = ST_NOWRITE;
				s32Ret = RET_FAILED;
			}
			
			break;
		}

		//计算遍历数据大小
		u32SheetSizeCount = u32SheetSizeCount + stSheetHead.u16RecordLen;
	}

	free(pu8TempData);
	pu8TempData = T_NULL;
	
	if(u32SheetSizeCount >= stSheetHead.u32SheetSize)
	{
		pstSheetHandle->eStatus = ST_NOSPACE;
	}
	
	return s32Ret;	
}

static T_S32 _SHEET_Delete(SheetHandle *pstSheetHandle, T_U8 u8SheetId)
{
	T_U8 *pu8TempData = T_NULL;
	T_U32 u32SheetOffAddr = 0;
	T_S32 s32Ret = RET_FAILED;
	SheetHead stSheetHead;
	SheetPoint *pstSheetPoint = T_NULL;

	if(RET_SUCCESS != _SHEET_QueryHead(pstSheetHandle->s32ModelId, u8SheetId, &stSheetHead))
	{
		pstSheetHandle->eStatus = ST_NOTABLE;
		return RET_FAILED;
	}
	
	pu8TempData = (T_U8 *)malloc(stSheetHead.u16RecordLen);
		
	ASSERT_EQUAL_RETVAL(pu8TempData, T_NULL, RET_FAILED);
	
	pstSheetPoint = _SHEET_GetSheetPoint(pstSheetHandle, u8SheetId);	
	u32SheetOffAddr = stSheetHead.u32SheetOffAddr + ((pstSheetPoint->u32CurPoint - 1) * stSheetHead.u16RecordLen);

	memset(pu8TempData, 0, stSheetHead.u16RecordLen);

	pu8TempData[0] = INVALID_FLAG;
	
	s32Ret = Flash_crtl(pstSheetHandle->s32ModelId, DELETE, u32SheetOffAddr, pu8TempData, stSheetHead.u16RecordLen);
	
	free(pu8TempData);
	pu8TempData = T_NULL;

	if(s32Ret != RET_SUCCESS)
	{
		pstSheetHandle->eStatus = ST_NOWRITE;
	}

	return s32Ret;
}

static T_S32 _SHEET_Update(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U8 *pu8Data, T_S32 s32Len)
{
	T_U8 *pu8TempData = T_NULL;
	T_U32 u32SheetOffAddr = 0;
	T_S32 s32Ret = RET_FAILED;
	SheetHead stSheetHead;
	SheetPoint *pstSheetPoint = T_NULL;

	if(RET_SUCCESS != _SHEET_QueryHead(pstSheetHandle->s32ModelId, u8SheetId, &stSheetHead))
	{
		pstSheetHandle->eStatus = ST_NOTABLE;
		return RET_FAILED;
	}
		
	ASSERT_NE_RETVAL(s32Len, (stSheetHead.u16RecordLen - 1), RET_FAILED);
	
	pu8TempData = (T_U8 *)malloc(stSheetHead.u16RecordLen);
		
	ASSERT_EQUAL_RETVAL(pu8TempData, T_NULL, RET_FAILED);
	
	pstSheetPoint = _SHEET_GetSheetPoint(pstSheetHandle, u8SheetId);

	if(pstSheetPoint->u32CurPoint > 0)
	{
		u32SheetOffAddr = stSheetHead.u32SheetOffAddr + ((pstSheetPoint->u32CurPoint - 1) * stSheetHead.u16RecordLen);
	}else if(0 == pstSheetPoint->u32CurPoint)
	{
		u32SheetOffAddr = stSheetHead.u32SheetOffAddr;
	}
	
	memset(pu8TempData, 0, stSheetHead.u16RecordLen);

	pu8TempData[0] = VALID_FLAG;
	memcpy(&pu8TempData[1], pu8Data, stSheetHead.u16RecordLen - 1);
	
	s32Ret = Flash_crtl(pstSheetHandle->s32ModelId, UPDATE, u32SheetOffAddr, pu8TempData, stSheetHead.u16RecordLen);
	
	free(pu8TempData);
	pu8TempData = T_NULL;

	if(s32Ret != RET_SUCCESS)
	{
		pstSheetHandle->eStatus = ST_NOWRITE;
	}
	
	return s32Ret;
}

static T_S32 _SHEET_Query(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U8 *pu8Data, T_S32 s32Len)
{
	T_U32 u32SheetSizeCount = 0;
	T_U8 *pu8TempData = T_NULL;
	T_U32 u32SheetOffAddr = 0;
	T_S32 s32Ret = RET_FAILED;
	SheetHead stSheetHead;
	SheetPoint *pstSheetPoint = T_NULL;

	if(RET_SUCCESS != _SHEET_QueryHead(pstSheetHandle->s32ModelId, u8SheetId, &stSheetHead))
	{
		pstSheetHandle->eStatus = ST_NOTABLE;
		return RET_FAILED;
	}
	
	ASSERT_NE_RETVAL(s32Len, (stSheetHead.u16RecordLen - 1), RET_FAILED);

	pu8TempData = (T_U8 *)malloc(stSheetHead.u16RecordLen);
		
	ASSERT_EQUAL_RETVAL(pu8TempData, T_NULL, RET_FAILED);
	
	pstSheetPoint = _SHEET_GetSheetPoint(pstSheetHandle, u8SheetId);

	u32SheetSizeCount = pstSheetPoint->u32CurPoint * stSheetHead.u16RecordLen;
		
	while(stSheetHead.u32SheetSize - u32SheetSizeCount > stSheetHead.u16RecordLen)
	{
		u32SheetOffAddr = u32SheetSizeCount + stSheetHead.u32SheetOffAddr;

		memset(pu8TempData, 0, stSheetHead.u16RecordLen);
		
		if(RET_SUCCESS != Flash_crtl(pstSheetHandle->s32ModelId, QUERY, u32SheetOffAddr, pu8TempData, stSheetHead.u16RecordLen))
		{						
			s32Ret = RET_FAILED;
			pstSheetHandle->eStatus = ST_UNKNOWN;
			break;
		}

		//递增查找记录位置，不管查找的数据是有效还是无效都递增
		pstSheetPoint->u32CurPoint++;
		
		//计算遍历数据大小
		u32SheetSizeCount = u32SheetSizeCount + stSheetHead.u16RecordLen;

		//找到一个有效数据
		if(VALID_FLAG == pu8TempData[0])
		{			
			memcpy(pu8Data, &pu8TempData[1], stSheetHead.u16RecordLen - 1);
			s32Ret = RET_SUCCESS;
			
			break;
		}else if(INVALID_FLAG == pu8TempData[0]) //无效数据
		{
			continue;
		}else if(END_FLAG == pu8TempData[0]) //到结尾退出
		{			
			s32Ret = RET_FAILED;
			pstSheetHandle->eStatus = ST_EOF;
			
			break;
		}
		
	}
	
	if(u32SheetSizeCount >= stSheetHead.u32SheetSize)
	{
		pstSheetHandle->eStatus = ST_EOF;
	}
	
	free(pu8TempData);
	pu8TempData = T_NULL;
	
	return s32Ret;	
}

static T_S32 _SHEET_Destory(SheetHandle *pstSheetHandle, T_U8 u8SheetId)
{
	T_U16 u16HeadSizeCount = 0;
	
	SheetHead stSheetHead;
	
	do{ 	
		memset(&stSheetHead, 0, sizeof(SheetHead));
		
		if(RET_SUCCESS != Flash_crtl(pstSheetHandle->s32ModelId, QUERY, u16HeadSizeCount, (T_U8 *)&stSheetHead, (T_U32)sizeof(SheetHead)))
		{
			pstSheetHandle->eStatus = ST_UNKNOWN;
			return RET_FAILED;
		}
		//到结尾退出
		if(END_FLAG == stSheetHead.u8IsValid)
		{				
			pstSheetHandle->eStatus = ST_EOF;
			return RET_FAILED;
		}

		if(VALID_FLAG == stSheetHead.u8IsValid)
		{
			//表存在
			if(stSheetHead.u8SheetId == u8SheetId)
			{
				//擦除flash
				if(RET_SUCCESS != Flash_crtl(pstSheetHandle->s32ModelId, DESTROY, stSheetHead.u32SheetOffAddr, T_NULL, stSheetHead.u32SheetSize))
				{
					pstSheetHandle->eStatus = ST_NOWRITE;
					return RET_FAILED;
				}

				memset(&stSheetHead, 0, sizeof(stSheetHead));

				stSheetHead.u8SheetId = 0;
				stSheetHead.u8IsValid = INVALID_FLAG;
				stSheetHead.u32SheetOffAddr = 0;
				stSheetHead.u32SheetSize  = 0;
				stSheetHead.u16RecordLen = 0;

				//更新表头信息
				if(RET_SUCCESS != Flash_crtl(pstSheetHandle->s32ModelId, UPDATE, u16HeadSizeCount, (T_U8 *)&stSheetHead, sizeof(SheetHead)))
				{
					pstSheetHandle->eStatus = ST_NOWRITE;
					return RET_FAILED;
				}

				return RET_SUCCESS;
			}
		}
		
		u16HeadSizeCount = u16HeadSizeCount + sizeof(SheetHead);
		
	}while(u16HeadSizeCount < SHEET_HEAD_SIZE);

	pstSheetHandle->eStatus = ST_NOTABLE;
	
	return RET_FAILED;
}

static T_S32 _SHEET_Clear(SheetHandle *pstSheetHandle, T_U8 u8SheetId)
{
    T_U16 u16HeadSizeCount = 0;
    
    SheetHead stSheetHead;
    
    do{     
        memset(&stSheetHead, 0, sizeof(SheetHead));
        
        if(RET_SUCCESS != Flash_crtl(pstSheetHandle->s32ModelId, QUERY, u16HeadSizeCount, (T_U8 *)&stSheetHead, (T_U32)sizeof(SheetHead)))
        {
            pstSheetHandle->eStatus = ST_UNKNOWN;
            return RET_FAILED;
        }
        //到结尾退出
        if(END_FLAG == stSheetHead.u8IsValid)
        {               
            pstSheetHandle->eStatus = ST_EOF;
            return RET_FAILED;
        }

        if(VALID_FLAG == stSheetHead.u8IsValid)
        {
            //表存在
            if(stSheetHead.u8SheetId == u8SheetId)
            {
                //擦除flash
                if(RET_SUCCESS != Flash_crtl(pstSheetHandle->s32ModelId, DESTROY, stSheetHead.u32SheetOffAddr, T_NULL, stSheetHead.u32SheetSize))
                {
                    pstSheetHandle->eStatus = ST_NOWRITE;
                    return RET_FAILED;
                }

                return RET_SUCCESS;
            }
        }
        
        u16HeadSizeCount = u16HeadSizeCount + sizeof(SheetHead);
        
    }while(u16HeadSizeCount < SHEET_HEAD_SIZE);

    pstSheetHandle->eStatus = ST_NOTABLE;
    
    return RET_FAILED;
}

/********************************************
 *功能：打开数据存储区
 *输入：数据表handle、数据存储区名称
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无。
*********************************************/
T_S32 MDL_SHEET_Open(SheetHandle *pstSheetHandle, T_S8 *ps8Name)
{	
	ASSERT_EQUAL_RETVAL(ps8Name, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;

	pstSheetHandle->s32ModelId = MDL_DRIVERMGR_Open(ps8Name);
	if(pstSheetHandle->s32ModelId < 0)
	{
		pstSheetHandle->eStatus = ST_NOOPEN;
		return RET_FAILED;
	}

	pstSheetHandle->u8SheetCount = 0;
		
	return RET_SUCCESS;
}

/********************************************
 *功能：创建数据表
 *输入：数据表handle、表号、
                   最大记录数、记录长度
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:创建表要按顺序创建，总储
 		量不能超过flash总容量。数据
 		表ID表示表的唯一性不能重复
*********************************************/
T_S32 MDL_SHEET_Create(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U32 u32RecordSize, T_U16 u16RecordLen)
{
	T_U32 u32SheetSize = 0;
	
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;

	u32SheetSize = u32RecordSize * u16RecordLen;

	if((u32SheetSize % SHEET_PAGE_SIZE) > 0)
	{
		u32SheetSize = (u32SheetSize/SHEET_PAGE_SIZE)*SHEET_PAGE_SIZE + SHEET_PAGE_SIZE;
	}

	if(u32SheetSize > SHEET_FLASH_SIZE)
	{
		pstSheetHandle->eStatus = ST_NOSPACE;
		return RET_FAILED;
	}

	return _SHEET_Create(pstSheetHandle, u8SheetId, u16RecordLen, u32SheetSize);
}

/********************************************
 *功能：初始化表的记录指针
 *输入：数据表handle
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:创建完所有表后调用该方法
*********************************************/
T_VOID MDL_SHEET_Initial(SheetHandle *pstSheetHandle)
{
	_SHEET_Initial(pstSheetHandle);
}

/********************************************
 *功能：在数据表中写入一条数据
 *输入：数据表handle、数据表ID，
                   写入数据、写入数据长度
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无。
*********************************************/
T_S32 MDL_SHEET_Insert(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U8 *pu8Data, T_S32 s32Len)
{		
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pu8Data, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;
	
	return _SHEET_Insert(pstSheetHandle, u8SheetId, pu8Data, s32Len);
}

/********************************************
 *功能：删除一条表记录
 *输入：数据表handle、表号
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:删除记录时删除当前记录。
             重新操作次过程是必须复位。
*********************************************/
T_S32 MDL_SHEET_Delete(SheetHandle *pstSheetHandle, T_U8 u8SheetId)
{		
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;

	return _SHEET_Delete(pstSheetHandle, u8SheetId);
}

/********************************************
 *功能：更新一条表记录
 *输入：数据表handle、表号、
                   更新的数据、数据长度
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:更新记录时更新当前记录。
             重新操作次过程是必须复位。
*********************************************/
T_S32 MDL_SHEET_Update(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U8 *pu8Data, T_S32 s32Len)
{		
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pu8Data, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;

	return _SHEET_Update(pstSheetHandle, u8SheetId, pu8Data, s32Len);
}

/********************************************
 *功能：查找一条表记录
 *输入：数据表handle、表号、
                   输出数据buff、数据长度
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:每查询一条记录，当前记录
 		指针偏移一个记录，直到复
 		位后记录指针指向第一条记
 		录。重新操作次过程是必须
 		复位。
*********************************************/
T_S32 MDL_SHEET_Query(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U8 *pu8Data, T_S32 s32Len)
{		
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pu8Data, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;

	return _SHEET_Query(pstSheetHandle, u8SheetId, pu8Data, s32Len);
}

/********************************************
 *功能：复位操作
 *输入：数据表handle
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:每做一次表操作前要复位数
             据表handle
*********************************************/
T_S32 MDL_SHEET_Reset(SheetHandle *pstSheetHandle, T_U8 u8SheetId)
{		
	SheetPoint *pstSheetPoint = T_NULL;
	
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);

	pstSheetPoint = _SHEET_GetSheetPoint(pstSheetHandle, u8SheetId);

	ASSERT_EQUAL_RETVAL(pstSheetPoint, T_NULL, RET_FAILED);

	pstSheetPoint->u32CurPoint = 0;
	
	return RET_SUCCESS;
}

/********************************************
 *功能：删除表
 *输入：数据表handle、数据表ID
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:删除表后，表中的原始记录
             都不存在
*********************************************/

T_S32 MDL_SHEET_Destory(SheetHandle *pstSheetHandle, T_U8 u8SheetId)
{
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;
	
	return _SHEET_Destory(pstSheetHandle, u8SheetId);
}


/********************************************
 *功能：表数据擦除
 *输入：数据表handle、数据表ID
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:表数据擦除后，表记录还在，原始数据已擦除
*********************************************/

T_S32 MDL_SHEET_Clear(SheetHandle *pstSheetHandle, T_U8 u8SheetId)
{
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;
	
	return _SHEET_Clear(pstSheetHandle, u8SheetId);
}


/********************************************
 *功能：获取操作状态
 *输入：数据表handle
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无
*********************************************/

SheetStatus MDL_SHEET_GetStatus(SheetHandle *pstSheetHandle)
{	
	return pstSheetHandle->eStatus;
}

/********************************************
 *功能：关闭数据存储区
 *输入：数据表handle
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无
*********************************************/
T_S32 MDL_SHEET_Close(SheetHandle *pstSheetHandle)
{
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	
	MDL_DRIVERMGR_Close(pstSheetHandle->s32ModelId);
	pstSheetHandle->s32ModelId = 0;

	if(T_NULL != pstSheetHandle->pstSheetPoint)
	{
		free(pstSheetHandle->pstSheetPoint);
	}
	
	return RET_SUCCESS;
}


