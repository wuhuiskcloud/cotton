/*********************************************
* @文件: sheet.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2017-12-6
* @概要: 提供数据库操作增删改查
                   接口以及flash的管理。
*********************************************/

#ifndef SHEET_H
#define SHEET_H

#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus

#include "baseType.h"

typedef enum tagSheetStatus
{
	ST_OK = 0, //无错误
	ST_UNKNOWN = 1, //未知错误
	ST_NOTABLE = 2, //表不存在
	ST_NOEXIST = 3, //记录不存在
	ST_NOSPACE = 4, //存储空间不足，存储区满	
	ST_NOOPEN  = 5, //打开数据存储区错误
	ST_NOWRITE = 6, //写存储区错误
	ST_EOF     = 7, //存储区末端
	ST_FORMATE = 8, //数据格式错误
}SheetStatus;

typedef struct tagSheetPoint
{
	T_U8 u8SheetId;
	T_U32 u32CurPoint;
}SheetPoint;

typedef struct tagSheetHandle
{
	T_S32 s32ModelId; //flash ID
	T_U8 u8SheetCount;
	SheetPoint *pstSheetPoint; //维护每一张表的记录操作位置
	SheetStatus eStatus; //数据表操作状态
}SheetHandle;

/********************************************
 *功能：打开数据存储区
 *输入：数据表handle、数据存储区名称
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无。
*********************************************/
T_S32 MDL_SHEET_Open(SheetHandle *pstSheetHandle, T_S8 *ps8Name);
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
T_S32 MDL_SHEET_Create(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U32 u32RecordSize, T_U16 u16RecordLen);
/********************************************
 *功能：初始化表的记录指针
 *输入：数据表handle
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:创建完所有表后调用该方法
*********************************************/
T_VOID MDL_SHEET_Initial(SheetHandle *pstSheetHandle);
/********************************************
 *功能：在数据表中写入一条数据
 *输入：数据表handle、数据表ID，
                   写入数据、写入数据长度
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无。
*********************************************/
T_S32 MDL_SHEET_Insert(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U8 *pu8Data, T_S32 s32Len);
/********************************************
 *功能：删除一条表记录
 *输入：数据表handle、表号
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:删除记录时删除当前记录。
             重新操作次过程是必须复位。
*********************************************/
T_S32 MDL_SHEET_Delete(SheetHandle *pstSheetHandle, T_U8 u8SheetId);
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
T_S32 MDL_SHEET_Update(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U8 *pu8Data, T_S32 s32Len);
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
T_S32 MDL_SHEET_Query(SheetHandle *pstSheetHandle, T_U8 u8SheetId, T_U8 *pu8Data, T_S32 s32Len);
/********************************************
 *功能：复位操作
 *输入：数据表handle
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:每做一次表操作前要复位数
             据表handle
*********************************************/
T_S32 MDL_SHEET_Reset(SheetHandle *pstSheetHandle, T_U8 u8SheetId);
/********************************************
 *功能：删除表
 *输入：数据表handle、数据表ID
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:删除表后，表中的原始记录
             都不存在
*********************************************/
T_S32 MDL_SHEET_Destory(SheetHandle *pstSheetHandle, T_U8 u8SheetId);
/********************************************
 *功能：表数据擦除
 *输入：数据表handle、数据表ID
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:表数据擦除后，表记录还在，原始数据已擦除
*********************************************/
T_S32 MDL_SHEET_Clear(SheetHandle *pstSheetHandle, T_U8 u8SheetId);
/********************************************
 *功能：获取操作状态
 *输入：数据表handle
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无
*********************************************/
SheetStatus MDL_SHEET_GetStatus(SheetHandle *pstSheetHandle);
/********************************************
 *功能：关闭数据存储区
 *输入：数据表handle
 *输出：无.
 *条件：无
 *返回：成功返回0，失败返回-1
 注意:无
*********************************************/
T_S32 MDL_SHEET_Close(SheetHandle *pstSheetHandle);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end SHEET_H

