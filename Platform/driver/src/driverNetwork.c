/*********************************************
* @�ļ�: driverNetwork.c
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-05-29
* @��Ҫ: �׷�ͨѶģ�������ӿ�
*********************************************/
#include "stm32f10x_usart.h"
#include "misc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "baseType.h"
#include "ioctrl.h"
#include "driverTime.h"
#include "driverTimer.h"
#include "driverFile.h"

/*�˿ڶ���*/
#define NET_USART			USART3					//���ں�
#define NET_USART_BTR		9600					//������

#define NET_RX_PORT   		GPIOC      				// NET RX == MCU TX
#define NET_RX_PIN    		GPIO_Pin_4

#define NET_TX_PORT   		GPIOC     				//NET TX == MCU RX
#define NET_TX_PIN    		GPIO_Pin_5

/* �߼�������ݶ��� */
#define NET_TASK_PEOD			10 //ÿNET_TASK_PEOD==10(ms)���һ���Ƿ�������
#define NET_CHK_RX_FULL_PEOD	50 //���NET_CHK_RX_FULL_PEOD==50(ms)û�յ����ݾͱ�ʾһ�����ݽ������
#define NET_UART_RX_CACHE_LEN	64 //����������󳤶�

typedef struct
{
	T_U8 u8D[NET_UART_RX_CACHE_LEN]; 	//���յ�����
	T_U8 u8Cnt; 						//�������ݼ���
	T_U8 u8Flag; 						//���ձ�־
	T_U8 u8FullFlag; 					//����һ���жϱ�־
} NETUartRevc; 

#define NET_UartIRQHandler  	USART3_4_IRQHandler

static NETUartRevc stNETRx; //��������

static T_U8 u8NETReadPopFlag = 0;
static T_U8 u8NETReadBuf[NET_UART_RX_CACHE_LEN]; //��ȡ����
static T_U8 u8NETReadLen = 0; //��ȡ��ʵ�ʳ���

/********************************************
 *���ܣ�IO���� ��ʼ��
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/
static T_VOID NETUart_Init(T_VOID)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//= System Clocks Configuration ==============================//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,  ENABLE ); // ������������IO�˿ڵ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // ��ʼ����ʱ��

	//=NVIC_Configuration======================================//
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;   // ����DMAͨ�����ж�����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // ���ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	/* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;     // �����ж�����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	//=GPIO_Configuration====================================================//

	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);  // ������û����Ĭ��IO�ڣ����Խ���������ӳ�䣬������Ը����Լ���Ӳ���������ѡ��
	/* Configure USART1 Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // ���ڽ���IO�ڵ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure USART1 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // ���ڷ���IO�ڵ�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // �������óɸ�����ʽ���������   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	//------UART configuration----------------------------------------------------------//
	/* USART Format configuration ------------------------------------------------------*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // ���ڸ�ʽ����
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/* Configure USART1 */
	USART_InitStructure.USART_BaudRate = 115200;  //  ����������
	USART_Init(USART1, &USART_InitStructure);
	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  // ���� ���ڿ���IDEL �ж�


	//USART1_DMA_config();   // ���� DMA ����

	/* Enable USARTy DMA TX request */
	//USART_Cmd(USART1, ENABLE);  // ��������
	//USART_DMACmd(USART1, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);  // ��������DMA���ͽ���
	//    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); // ��������DMA����
	
	memset(&stNETRx, 0, sizeof(NETUartRevc));
}

/********************************************
 *���ܣ�NET�������ݷ���
 *���룺�����볤��
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/
static T_VOID NET_UartSendBuf(T_U8 *u8Buf, T_U8 u8Len)
{
	T_U8 u8i = 0;
	
	for(u8i = 0; u8i < u8Len; u8i++)
	{
		USART_SendData(NET_USART, u8Buf[u8i]);
		while(USART_GetFlagStatus(NET_USART, USART_FLAG_TXE) == RESET);
	}
}

/********************************************
 *���ܣ�NET�����ж�
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/
void NET_UartIRQHandler(void)
{
	T_U8 u8Rx = 0;
	
	if(USART_GetITStatus(NET_USART, USART_IT_RXNE) != RESET) //�������ж�
	{	
		u8Rx = (T_U8)USART_ReceiveData(NET_USART);
		if(stNETRx.u8Cnt < NET_UART_RX_CACHE_LEN)
		{
			stNETRx.u8D[stNETRx.u8Cnt++] = u8Rx;
		}
		stNETRx.u8Flag = 1;
		stNETRx.u8FullFlag = 0;
		
		USART_ClearITPendingBit(NET_USART, USART_IT_RXNE); // ǿ�����ж�
	}else
	{
		u8Rx = USART_ReceiveData(NET_USART);
		USART_ClearITPendingBit(NET_USART, USART_IT_ORE);
		USART_ClearITPendingBit(NET_USART, USART_IT_ERR);
	}
}

/********************************************
 *���ܣ�NET����������
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/
static T_VOID NETUart_TaskHandler(T_VOID *pvData)
{
	TimerHandle stTimeNode;
	T_U32 stTick;
	TimeTick stTimeTick;
	
	/*�������ݴ�����*/
	if(1 == stNETRx.u8Flag)
	{
		stNETRx.u8FullFlag++;
		
		if(stNETRx.u8FullFlag >= (NET_CHK_RX_FULL_PEOD / NET_TASK_PEOD))
		{
			memset(u8NETReadBuf, 0, sizeof(u8NETReadBuf));
			memcpy(u8NETReadBuf, stNETRx.u8D, stNETRx.u8Cnt);
			u8NETReadLen = stNETRx.u8Cnt;
			u8NETReadPopFlag = 1;
			
			stNETRx.u8Cnt = 0;
			stNETRx.u8Flag = 0;
			stNETRx.u8FullFlag = 0;
		}
	}

	MDL_TIME_GetTimeTick(&stTimeTick);
	stTick = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec;
			
	stTimeNode.fTimerHandle = NETUart_TaskHandler;
	stTimeNode.pvData = T_NULL;
	stTimeNode.s32MSec = stTick + NET_TASK_PEOD;
	MDL_DRVTIMER_AddTimer(&stTimeNode);
}

/********************************************
 *���ܣ���NET����
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
 ע��:
*********************************************/
static T_S32 _NET_Open(T_VOID)
{
	TimerHandle stTimeNode;
	T_U32 stTick;
	TimeTick stTimeTick;
	
	NETUart_Init();

	MDL_TIME_GetTimeTick(&stTimeTick);
	stTick = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec;
	stTimeNode.fTimerHandle = NETUart_TaskHandler;
	stTimeNode.pvData = T_NULL;
	stTimeNode.s32MSec = stTick + NET_TASK_PEOD;
	MDL_DRVTIMER_AddTimer(&stTimeNode);
			
	return RET_SUCCESS;
}

/********************************************
 *���ܣ���ȡ����
 *���룺������� �� ���ݳ���
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
 		���߳���
 ע��:
*********************************************/
static T_S32 _NET_Read(T_S8 *ps8DataBuf, T_S32 s32BufLen)
{
	ASSERT_EQUAL_RETVAL(ps8DataBuf, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(s32BufLen, 0, RET_FAILED);
	
	if(1 == u8NETReadPopFlag)
	{
		u8NETReadPopFlag = 0;

		memset(ps8DataBuf, 0, s32BufLen);
		if(s32BufLen > u8NETReadLen)
		{
			memcpy(ps8DataBuf, u8NETReadBuf, u8NETReadLen);
			return u8NETReadLen;
		}else
		{
			memcpy(ps8DataBuf, u8NETReadBuf, s32BufLen);
			return s32BufLen;
		}
	}
	
	return RET_FAILED;
}

/********************************************
 *���ܣ�д�뺯��
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
 		���߳���
 ע��:
*********************************************/
static T_S32 _NET_Write(T_S8 *ps8DataBuf, T_S32 s32BufLen)
{
	ASSERT_EQUAL_RETVAL(ps8DataBuf, T_NULL, RET_FAILED);
	ASSERT_GT_RETVAL(s32BufLen, 255, RET_FAILED);
	
	NET_UartSendBuf((T_U8 *)ps8DataBuf, s32BufLen);
	
	return s32BufLen;
}
/********************************************
 *���ܣ��¼���־
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
 ע��:
*********************************************/
static T_S32 _NET_Pop(T_VOID)
{
	if(0 == u8NETReadPopFlag)
	{
		return RET_FAILED;
	}	
	
	return RET_SUCCESS;
}

/********************************************
 *���ܣ��رպ���
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 			ʧ��:RET_FAILED 
 ע��:
*********************************************/
static T_S32 _NET_Close(T_VOID)
{
	return RET_SUCCESS;
}

/************************************************
 *���ܣ���NET��������ģ����ӵ�
 			�����б��С�
 *���룺��
 *�������
 *��������
 *���أ��ɹ�0ʧ��-1
 ע��:
*************************************************/
T_S32 DRIVER_NetWork_Init(T_VOID)
{
	FileOperation stOperation;

	/* �������ģ�� */
	stOperation.FileOpen = _NET_Open;
	stOperation.FileRead = _NET_Read;
	stOperation.FileWrite = _NET_Write;
	stOperation.FileClose = _NET_Close;
	stOperation.FileIoctl = T_NULL;
	stOperation.FilePop = _NET_Pop;
	stOperation.FileRelease = T_NULL;
	memset(stOperation.s8ModelName, 0, MODEL_NAME_LEN);
	memcpy(stOperation.s8ModelName, "NET", strlen("NET"));
	
	return MDL_FILE_AddModel((FileOperation*)&stOperation);
}

