/*********************************************
* @文件: driverFile.c
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-5-25
* @概要: 抽象驱动文件的操作接口。
			每一个驱动模块都是一个
			文件模块，在每一个驱动
			模块里面实现文件调用接
			口。在上层应用可以独立
			调用这些接口实现对外设
			的操作。
*********************************************/

#include "driverFile.h"

typedef struct tagFileManager
{
	FileOperation *pstFileModel;
	T_U8 u8FileCount;
}FileManager;

static FileManager g_stFileManager;

/********************************************
 *功能：初始化驱动文件
 *输入：无
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
*********************************************/
T_S32 MDL_FILE_Init(T_VOID)
{
	g_stFileManager.pstFileModel = T_NULL;
	g_stFileManager.u8FileCount = 0;

	return RET_SUCCESS;
}

/********************************************
 *功能：在文件管理表中加入一个
                   驱动模块
 *输入：驱动文件模块
 *输出：无.
 *条件：无
 *返回：成功0失败-1
 注意:无。
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
 *功能：获取文件驱动模块
 *输入：驱动文件模块ID
 *输出：无.
 *条件：无
 *返回：驱动文件模块
 注意:无。
*********************************************/
FileOperation * MDL_FILE_GetModel(T_S32 s32ModelId)
{
	ASSERT_GT_RETVAL(s32ModelId, g_stFileManager.u8FileCount, T_NULL);
	
	return &g_stFileManager.pstFileModel[s32ModelId];
}

/********************************************
 *功能：获取文件驱动模块
 *输入：驱动文件模块ID
 *输出：无.
 *条件：无
 *返回：驱动文件模块
 注意:无。
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
 *功能：去初始化驱动文件
 *输入：无
 *输出：无.
 *条件：无
 *返回：无
 注意:无。
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

