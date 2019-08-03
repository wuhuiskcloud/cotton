/*********************************************
* @�ļ�: driverFile.c
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-5-25
* @��Ҫ: ���������ļ��Ĳ����ӿڡ�
			ÿһ������ģ�鶼��һ��
			�ļ�ģ�飬��ÿһ������
			ģ������ʵ���ļ����ý�
			�ڡ����ϲ�Ӧ�ÿ��Զ���
			������Щ�ӿ�ʵ�ֶ�����
			�Ĳ�����
*********************************************/

#include "driverFile.h"

typedef struct tagFileManager
{
	FileOperation *pstFileModel;
	T_U8 u8FileCount;
}FileManager;

static FileManager g_stFileManager;

/********************************************
 *���ܣ���ʼ�������ļ�
 *���룺��
 *�������.
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_FILE_Init(T_VOID)
{
	g_stFileManager.pstFileModel = T_NULL;
	g_stFileManager.u8FileCount = 0;

	return RET_SUCCESS;
}

/********************************************
 *���ܣ����ļ�������м���һ��
                   ����ģ��
 *���룺�����ļ�ģ��
 *�������.
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_FILE_AddModel(FileOperation *pstFileModel)
{
	FileOperation *pstTemp = T_NULL;
	
	ASSERT_EQUAL_RETVAL(pstFileModel, T_NULL, RET_FAILED);

	if(T_NULL == g_stFileManager.pstFileModel)
	{
		g_stFileManager.pstFileModel = (FileOperation *)malloc(1*sizeof(FileOperation));
	}else
	{
		pstTemp = (FileOperation *)realloc(g_stFileManager.pstFileModel, (g_stFileManager.u8FileCount+1)*sizeof(FileOperation));
		g_stFileManager.pstFileModel = pstTemp;
	}
	
	ASSERT_EQUAL_RETVAL(g_stFileManager.pstFileModel, T_NULL, RET_FAILED);
	
	memcpy(&g_stFileManager.pstFileModel[g_stFileManager.u8FileCount], pstFileModel, sizeof(FileOperation));
	g_stFileManager.u8FileCount++;
	
	return RET_SUCCESS;
}

/********************************************
 *���ܣ���ȡ�ļ�����ģ��
 *���룺�����ļ�ģ��ID
 *�������.
 *��������
 *���أ������ļ�ģ��
 ע��:�ޡ�
*********************************************/
FileOperation * MDL_FILE_GetModel(T_S32 s32ModelId)
{
	ASSERT_GT_RETVAL(s32ModelId, g_stFileManager.u8FileCount, T_NULL);
	
	return &g_stFileManager.pstFileModel[s32ModelId];
}

/********************************************
 *���ܣ���ȡ�ļ�����ģ��
 *���룺�����ļ�ģ��ID
 *�������.
 *��������
 *���أ������ļ�ģ��
 ע��:�ޡ�
*********************************************/
T_S32 MDL_FILE_GetModelId(T_S8 *ps8Name)
{
	T_S32 i=0;
	ASSERT_EQUAL_RETVAL(ps8Name, T_NULL, -1);

	for(i=0; i<g_stFileManager.u8FileCount; i++)
	{
		FileOperation *pstFileModel = &g_stFileManager.pstFileModel[i];
		
		if(!strcmp(ps8Name, pstFileModel->s8ModelName))
		{
			return i;
		}
	}
	
	return -1;
}

/********************************************
 *���ܣ�ȥ��ʼ�������ļ�
 *���룺��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_FILE_DeInit(T_VOID)
{
	T_S32 i = 0;

	for(i=0; i<g_stFileManager.u8FileCount; i++)
	{
		FileOperation *pstFileModel = &g_stFileManager.pstFileModel[i];

		if(T_NULL != pstFileModel->FileRelease)
		{
			pstFileModel->FileRelease();
		}
	}

	if(T_NULL != g_stFileManager.pstFileModel)
	{
		free(g_stFileManager.pstFileModel);
		g_stFileManager.pstFileModel = T_NULL;
	}
}

