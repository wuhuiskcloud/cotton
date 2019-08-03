/*********************************************
* @�ļ�: timer.h
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-5-25
* @��Ҫ: �ṩ��ʱ��С���߽ӿ�
*********************************************/

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C" {
#endif //end __cplusplus

#include "baseType.h"
#include "list.h"

typedef T_VOID (*TimeoutHandel)(T_VOID *pvArg);
typedef T_VOID (*DeleteHandel)(T_VOID *pvArg);

typedef struct tagTimerHead
{	
	struct list_head stTimerHead;
}TimerHead;

/********************************************
 *���ܣ�����һ����ʱ������ͷ
 *���룺��ʱ������ͷ��Ϣ
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_TIMER_Init(TimerHead *pstTimerHead);
/********************************************
 *���ܣ�����һ����ʱ��
 *���룺��ʱ������ͷ����ʱ����
                   ʶ����ʱ�����д�������
                   ��ʱ�䡢��ʱhandle������
 *�������.
 *��������
 *���أ��ɹ�0��ʧ��-1
 ע��:���д���Ϊ0��ʾ�������У�
              >0��ʾ����N�Ρ�
*********************************************/
T_S32 MDL_TIMER_Add(TimerHead *pstTimerHead, T_U8 u8Tok, T_U8 u8RunCount, TimeoutHandel fTimeoutHandle, 
								DeleteHandel fDeleteHandle, T_VOID *vDate);
/********************************************
 *���ܣ�����һ����ʱ��
 *���룺��ʱ������ͷ����ʱ����
 *�������.
 *��������
 *���أ�����˽������
 ע��:�ޡ�
*********************************************/
T_VOID *MDL_TIMER_GetPrivateData(TimerHead *pstTimerHead, T_U8 u8Tok);
/********************************************
 *���ܣ�����TOK������һ����ʱ��
 *���룺��ʱ������ͷ����ʱ����
 *�������.
 *��������
 *���أ��ɹ�����0ʧ�ܷ���-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_TIMER_Start(TimerHead *pstTimerHead, T_U8 u8Tok, T_S32 s32InterValMs);
/********************************************
 *���ܣ�����TOK��ֹͣһ����ʱ��
 *���룺��ʱ������ͷ����ʱ����
 *�������.
 *��������
 *���أ��ɹ�����0ʧ�ܷ���-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_TIMER_Stop(TimerHead *pstTimerHead, T_U8 u8Tok);
/********************************************
 *���ܣ�����һ����ʱ��
 *���룺��ʱ������ͷ����ǰʱ��
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_TIMER_Active(TimerHead *pstTimerHead, T_U32 u32CurTime);
/********************************************
 *���ܣ�ɾ��ĳ��ʱ��
 *���룺��ʱ������ͷ����ʱ����ʶ
 *�������.
 *��������
 *���أ��ɹ�����0ʧ�ܷ���-1
 ע��:�ޡ�
*********************************************/
T_S32 MDL_TIMER_Delete(TimerHead *pstTimerHead, T_U8 u8Tok);
/********************************************
 *���ܣ�ȥ��ʼ����ʱ������
 *���룺��ʱ������ͷ
 *�������.
 *��������
 *���أ���
 ע��:�ޡ�
*********************************************/
T_VOID MDL_TIMER_UnInit(TimerHead *pstTimerHead);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif //end TIMER_H
