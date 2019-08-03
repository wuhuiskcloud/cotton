/*********************************************
* @文件: driverNetwork.c
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-05-29
* @概要: 米峰通讯模块驱动接口
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

/*端口定义*/
#define NET_USART			USART3					//串口号
#define NET_USART_BTR		9600					//波特率

#define NET_RX_PORT   		GPIOC      				// NET RX == MCU TX
#define NET_RX_PIN    		GPIO_Pin_4

#define NET_TX_PORT   		GPIOC     				//NET TX == MCU RX
#define NET_TX_PIN    		GPIO_Pin_5

/* 逻辑相关数据定义 */
#define NET_TASK_PEOD			10 //每NET_TASK_PEOD==10(ms)检测一次是否有数据
#define NET_CHK_RX_FULL_PEOD	50 //如果NET_CHK_RX_FULL_PEOD==50(ms)没收到数据就表示一组数据接收完成
#define NET_UART_RX_CACHE_LEN	64 //缓冲数据最大长度

typedef struct
{
	T_U8 u8D[NET_UART_RX_CACHE_LEN]; 	//接收的数据
	T_U8 u8Cnt; 						//接收数据计数
	T_U8 u8Flag; 						//接收标志
	T_U8 u8FullFlag; 					//接收一组判断标志
} NETUartRevc; 

#define NET_UartIRQHandler  	USART3_4_IRQHandler

static NETUartRevc stNETRx; //接收数据

static T_U8 u8NETReadPopFlag = 0;
static T_U8 u8NETReadBuf[NET_UART_RX_CACHE_LEN]; //读取缓存
static T_U8 u8NETReadLen = 0; //读取的实际长度

/********************************************
 *功能：IO配置 初始化
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*********************************************/
static T_VOID NETUart_Init(T_VOID)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//= System Clocks Configuration ==============================//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,  ENABLE ); // 开启串口所在IO端口的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // 开始串口时钟

	//=NVIC_Configuration======================================//
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;   // 发送DMA通道的中断配置
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // 优先级设置
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	/* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;     // 串口中断配置
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	//=GPIO_Configuration====================================================//

	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);  // 我这里没有用默认IO口，所以进行了重新映射，这个可以根据自己的硬件情况配置选择
	/* Configure USART1 Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // 串口接收IO口的设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure USART1 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // 串口发送IO口的设置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 这里设置成复用形式的推挽输出   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	//------UART configuration----------------------------------------------------------//
	/* USART Format configuration ------------------------------------------------------*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // 串口格式配置
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/* Configure USART1 */
	USART_InitStructure.USART_BaudRate = 115200;  //  波特率设置
	USART_Init(USART1, &USART_InitStructure);
	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  // 开启 串口空闲IDEL 中断


	//USART1_DMA_config();   // 串口 DMA 配置

	/* Enable USARTy DMA TX request */
	//USART_Cmd(USART1, ENABLE);  // 开启串口
	//USART_DMACmd(USART1, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);  // 开启串口DMA发送接收
	//    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); // 开启串口DMA接收
	
	memset(&stNETRx, 0, sizeof(NETUartRevc));
}

/********************************************
 *功能：NET串口数据发送
 *输入：数据与长度
 *输出：无
 *条件：无
 *返回：无
 注意:
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
 *功能：NET串口中断
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*********************************************/
void NET_UartIRQHandler(void)
{
	T_U8 u8Rx = 0;
	
	if(USART_GetITStatus(NET_USART, USART_IT_RXNE) != RESET) //检查接收中断
	{	
		u8Rx = (T_U8)USART_ReceiveData(NET_USART);
		if(stNETRx.u8Cnt < NET_UART_RX_CACHE_LEN)
		{
			stNETRx.u8D[stNETRx.u8Cnt++] = u8Rx;
		}
		stNETRx.u8Flag = 1;
		stNETRx.u8FullFlag = 0;
		
		USART_ClearITPendingBit(NET_USART, USART_IT_RXNE); // 强制清中断
	}else
	{
		u8Rx = USART_ReceiveData(NET_USART);
		USART_ClearITPendingBit(NET_USART, USART_IT_ORE);
		USART_ClearITPendingBit(NET_USART, USART_IT_ERR);
	}
}

/********************************************
 *功能：NET串口任务函数
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 注意:
*********************************************/
static T_VOID NETUart_TaskHandler(T_VOID *pvData)
{
	TimerHandle stTimeNode;
	T_U32 stTick;
	TimeTick stTimeTick;
	
	/*接收数据处理部分*/
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
 *功能：打开NET串口
 *输入：无
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 		失败:RET_FAILED 
 注意:
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
 *功能：读取函数
 *输入：输出数据 及 数据长度
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 		失败:RET_FAILED 
 		或者长度
 注意:
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
 *功能：写入函数
 *输入：无
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 		失败:RET_FAILED 
 		或者长度
 注意:
*********************************************/
static T_S32 _NET_Write(T_S8 *ps8DataBuf, T_S32 s32BufLen)
{
	ASSERT_EQUAL_RETVAL(ps8DataBuf, T_NULL, RET_FAILED);
	ASSERT_GT_RETVAL(s32BufLen, 255, RET_FAILED);
	
	NET_UartSendBuf((T_U8 *)ps8DataBuf, s32BufLen);
	
	return s32BufLen;
}
/********************************************
 *功能：事件标志
 *输入：无
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 		失败:RET_FAILED 
 注意:
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
 *功能：关闭函数
 *输入：无
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 			失败:RET_FAILED 
 注意:
*********************************************/
static T_S32 _NET_Close(T_VOID)
{
	return RET_SUCCESS;
}

/************************************************
 *功能：将NET串口驱动模块添加到
 			驱动列表中。
 *输入：无
 *输出：无
 *条件：无
 *返回：成功0失败-1
 注意:
*************************************************/
T_S32 DRIVER_NetWork_Init(T_VOID)
{
	FileOperation stOperation;

	/* 添加驱动模块 */
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

