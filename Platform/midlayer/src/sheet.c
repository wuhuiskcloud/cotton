/*********************************************
* @�ļ�: sheet.h
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-5-30
* @��Ҫ: �ṩ���ݿ������ɾ�Ĳ�
                   �ӿ��Լ�flash�Ĺ���
*********************************************/
#include "sheet.h"
#include "driverFlash.h"
#include "driverManager.h"

#define SHEET_NAME_LEN 8

#define SHEET_FLASH_SIZE (1024*2)   //0x400000 //���ݴ洢���Ĵ�С(4096*15)
#define SHEET_PAGE_SIZE  1024     //4096 //���ݴ洢����С������С
#define SHEET_HEAD_SIZE  1024    //4096 //���ݼ�¼��Ϣ�洢����С

#define VALID_FLAG 0xFD //�¼�¼��Ϊ��Ч
#define INVALID_FLAG 0xFE //ɾ��������ݼ�¼Ϊ��Ч
#define END_FLAG 0xFF //�����ļ�ĩλ

//���ݿ��������
typedef enum tagSheetOperater
{
	CREATE,//������
	INSERT,//��Ӽ�¼
	QUERY, //���Ҽ�¼
	UPDATE,//���¼�¼
	DELETE,//ɾ����¼
	DESTROY,//ɾ����
}SheetOperater;

//�������ݿ����Ϣ
typedef struct tagSheetHead
{
	T_U8 u8IsValid; //��Ч��־
	T_U8 u8SheetId; //���ݱ�ID��ע���0��ʼ��һ��ҪΨһ��������,��������ÿһ�ű��ƫ�Ƶ�ַ
	T_U16 u16RecordLen; //һ����¼�Ĵ�Сbtype	
	T_U32 u32SheetOffAddr; //���ݱ�ƫ�Ƶ�ַ
	T_U32 u32SheetSize; //���ݱ�flash�ռ��Сbtype
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

		//����ȫ��û�����ݱ�
		if(END_FLAG == pstSheetHead->u8IsValid)
		{		
			break;
		}else if(VALID_FLAG == pstSheetHead->u8IsValid) //���ұ���Ϣ
		{
			//�ҵ���ر�
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
		
		//����β�˳�
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
		
		//��¼��Ч
		if(INVALID_FLAG == stSheetHead.u8IsValid || END_FLAG == stSheetHead.u8IsValid)
		{	
			if(0 == u16RecordOffset) //������0˵�����ҵ���Ч���ݼ�¼
			{
				u16RecordOffset = u16HeadSizeCount + 1;
			}

			//����β�˳�
			if(END_FLAG == stSheetHead.u8IsValid)
			{		
				break;
			}
		}

		if(VALID_FLAG == stSheetHead.u8IsValid)
		{
			//�����½����ƫ�Ƶ�ַ
			u32TempCount = stSheetHead.u32SheetOffAddr + stSheetHead.u32SheetSize;

			//�ҳ����һ����ĵ�ַ
			if(u32TempCount > u32SheetOffAddr)
			{
				u32SheetOffAddr = u32TempCount;
			}
			
			//����ڲ��ٴ���
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

		//�ҵ�һ����λ��
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

		//����������ݴ�С
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

		//�������Ҽ�¼λ�ã����ܲ��ҵ���������Ч������Ч������
		pstSheetPoint->u32CurPoint++;
		
		//����������ݴ�С
		u32SheetSizeCount = u32SheetSizeCount + stSheetHead.u16RecordLen;

		//�ҵ�һ����Ч����
		if(VALID_FLAG == pu8TempData[0])
		{			
			memcpy(pu8Data, &pu8TempData[1], stSheetHead.u16RecordLen - 1);
			s32Ret = RET_SUCCESS;
			
			break;
		}else if(INVALID_FLAG == pu8TempData[0]) //��Ч����
		{
			continue;
		}else if(END_FLAG == pu8TempData[0]) //����β�˳�
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
		//����β�˳�
		if(END_FLAG == stSheetHead.u8IsValid)
		{				
			pstSheetHandle->eStatus = ST_EOF;
			return RET_FAILED;
		}

		if(VALID_FLAG == stSheetHead.u8IsValid)
		{
			//�����
			if(stSheetHead.u8SheetId == u8SheetId)
			{
				//����flash
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

				//���±�ͷ��Ϣ
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
        //����β�˳�
        if(END_FLAG == stSheetHead.u8IsValid)
        {               
            pstSheetHandle->eStatus = ST_EOF;
            return RET_FAILED;
        }

        if(VALID_FLAG == stSheetHead.u8IsValid)
        {
            //�����
            if(stSheetHead.u8SheetId == u8SheetId)
            {
                //����flash
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
 *���ܣ������ݴ洢��
 *���룺���ݱ�handle�����ݴ洢������
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:�ޡ�
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
 *���ܣ��������ݱ�
 *���룺���ݱ�handle����š�
                   ����¼������¼����
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:������Ҫ��˳�򴴽����ܴ�
 		�����ܳ���flash������������
 		��ID��ʾ���Ψһ�Բ����ظ�
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
 *���ܣ���ʼ����ļ�¼ָ��
 *���룺���ݱ�handle
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:���������б����ø÷���
*********************************************/
T_VOID MDL_SHEET_Initial(SheetHandle *pstSheetHandle)
{
	_SHEET_Initial(pstSheetHandle);
}

/********************************************
 *���ܣ������ݱ���д��һ������
 *���룺���ݱ�handle�����ݱ�ID��
                   д�����ݡ�д�����ݳ���
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_SHEET_Insert(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U8 *pu8Data, T_S32 s32Len)
{		
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pu8Data, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;
	
	return _SHEET_Insert(pstSheetHandle, u8SheetId, pu8Data, s32Len);
}

/********************************************
 *���ܣ�ɾ��һ�����¼
 *���룺���ݱ�handle�����
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:ɾ����¼ʱɾ����ǰ��¼��
             ���²����ι����Ǳ��븴λ��
*********************************************/
T_S32 MDL_SHEET_Delete(SheetHandle *pstSheetHandle, T_U8 u8SheetId)
{		
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;

	return _SHEET_Delete(pstSheetHandle, u8SheetId);
}

/********************************************
 *���ܣ�����һ�����¼
 *���룺���ݱ�handle����š�
                   ���µ����ݡ����ݳ���
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:���¼�¼ʱ���µ�ǰ��¼��
             ���²����ι����Ǳ��븴λ��
*********************************************/
T_S32 MDL_SHEET_Update(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U8 *pu8Data, T_S32 s32Len)
{		
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pu8Data, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;

	return _SHEET_Update(pstSheetHandle, u8SheetId, pu8Data, s32Len);
}

/********************************************
 *���ܣ�����һ�����¼
 *���룺���ݱ�handle����š�
                   �������buff�����ݳ���
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:ÿ��ѯһ����¼����ǰ��¼
 		ָ��ƫ��һ����¼��ֱ����
 		λ���¼ָ��ָ���һ����
 		¼�����²����ι����Ǳ���
 		��λ��
*********************************************/
T_S32 MDL_SHEET_Query(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U8 *pu8Data, T_S32 s32Len)
{		
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(pu8Data, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;

	return _SHEET_Query(pstSheetHandle, u8SheetId, pu8Data, s32Len);
}

/********************************************
 *���ܣ���λ����
 *���룺���ݱ�handle
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:ÿ��һ�α����ǰҪ��λ��
             �ݱ�handle
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
 *���ܣ�ɾ����
 *���룺���ݱ�handle�����ݱ�ID
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:ɾ����󣬱��е�ԭʼ��¼
             ��������
*********************************************/

T_S32 MDL_SHEET_Destory(SheetHandle *pstSheetHandle, T_U8 u8SheetId)
{
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;
	
	return _SHEET_Destory(pstSheetHandle, u8SheetId);
}


/********************************************
 *���ܣ������ݲ���
 *���룺���ݱ�handle�����ݱ�ID
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:�����ݲ����󣬱��¼���ڣ�ԭʼ�����Ѳ���
*********************************************/

T_S32 MDL_SHEET_Clear(SheetHandle *pstSheetHandle, T_U8 u8SheetId)
{
	ASSERT_EQUAL_RETVAL(pstSheetHandle, T_NULL, RET_FAILED);
	
	pstSheetHandle->eStatus = ST_OK;
	
	return _SHEET_Clear(pstSheetHandle, u8SheetId);
}


/********************************************
 *���ܣ���ȡ����״̬
 *���룺���ݱ�handle
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:��
*********************************************/

SheetStatus MDL_SHEET_GetStatus(SheetHandle *pstSheetHandle)
{	
	return pstSheetHandle->eStatus;
}

/********************************************
 *���ܣ��ر����ݴ洢��
 *���룺���ݱ�handle
 *�������.
 *��������
 *���أ��ɹ�����0��ʧ�ܷ���-1
 ע��:��
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


