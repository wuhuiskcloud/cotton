#include "driverUart.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"
#include "baseType.h"
// UART1  TX--PA9  RX--PA10
// UART2  TX-PA02  RX-PA03
// UART3  TX-PB10  RX--PB11
#define USART_RX_BUFF_MAX_LENGTH	128
#define USART_TX_BUFF_MAX_LENGTH	128

typedef struct tagUartHander{
	T_U8 u8RxLen;
	T_U8 u8DMARxData[USART_RX_BUFF_MAX_LENGTH];
	T_U8 u8DMATxData[USART_TX_BUFF_MAX_LENGTH];
	T_U8 u8SendFlag;
	T_U8 u8RecevFlag;
	T_U8 u8PopFlag;
}T_UartHander;

static T_UartHander stUart1Handle = {0, 0, 0, 0, 0, 0};
static T_UartHander stUart2Handle = {0, 0, 0, 0, 0, 0};
static T_UartHander stUart3Handle = {0, 0, 0, 0, 0, 0};

static void USART1_DMA_config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
    /* DMA clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 开启DMA1时钟
	//=DMA_Configuration===============================================//
	/*--- LUMMOD_UART_Rx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel5, DISABLE);                           // 关DMA通道
    DMA_DeInit(DMA1_Channel5);                                 // 恢复缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);// 设置串口接收数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart1Handle.u8DMARxData;         // 设置接收缓冲区首地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     // 设置外设为数据源，外设寄存器 -> 内存缓冲区
    DMA_InitStructure.DMA_BufferSize = USART_RX_BUFF_MAX_LENGTH;                     // 需要最大可能接收到的字节数
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不做增加调整，调整不调整是DMA自动实现的
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存缓冲区地址增加调整
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度8位，1个字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据宽度8位，1个字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 单次传输模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // 优先级设置
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // 关闭内存到内存的DMA模式
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);               // 写入配置
    DMA_ClearFlag(DMA1_FLAG_GL5);                                 // 清除DMA所有标志
    DMA_Cmd(DMA1_Channel5, ENABLE);                            // 开启接收DMA通道，等待接收数据
	/*--- LUMMOD_UART_Tx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel4, DISABLE);                           // 关DMA通道
    DMA_DeInit(DMA1_Channel4);                                 // 恢复缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned )(&USART1->DR);// 设置串口发送数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart1Handle.u8DMATxData;         // 设置发送缓冲区首地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // 设置外设位目标，内存缓冲区 ->外设寄存器
    DMA_InitStructure.DMA_BufferSize = 0;                     // 需要发送的字节数，这里其实可以设置为0，因为在实际要发送的时候，会重新设置次值
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不做增加调整，调整不调整是DMA自动实现的
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存缓冲区地址增加调整
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度8位，1个字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据宽度8位，1个字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 单次传输模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // 优先级设置
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // 关闭内存到内存的DMA模式
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);               // 写入配置
    DMA_ClearFlag(DMA1_FLAG_GL4);                                 // 清除DMA所有标志
    DMA_Cmd(DMA1_Channel4, DISABLE); // 关闭DMA
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);            // 开启发送DMA通道中断
}
//---------------------------------------------------------------------//
static void USART1_Init(T_U32 u32BaudRate, T_U16 u16StopBit)
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
    USART_InitStructure.USART_StopBits = u16StopBit;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure USART1 */
    USART_InitStructure.USART_BaudRate = u32BaudRate;  //  波特率设置
    USART_Init(USART1, &USART_InitStructure);
    /* Enable USART1 Receive and Transmit interrupts */
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  // 开启 串口空闲IDEL 中断
	USART1_DMA_config();   // 串口 DMA 配置
/* Enable USARTy DMA TX request */
	USART_Cmd(USART1, ENABLE);  // 开启串口
    USART_DMACmd(USART1, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);  // 开启串口DMA发送接收
//  USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); // 开启串口DMA接收

}


static T_U16 USART1_DMA_TX_Data(uint8_t* buff,uint16_t length)
{
	stUart1Handle.u8SendFlag = 1;
	memcpy(stUart1Handle.u8DMATxData, buff, length);
    DMA1_Channel4->CNDTR = length; // 设置要发送的字节数目
    DMA_Cmd(DMA1_Channel4, ENABLE);        //开始DMA发�
    while(stUart1Handle.u8SendFlag == 1){
		;
	}
    return length;
}

static void USART1_DMA_RX_Data(void)
{
	DMA_Cmd(DMA1_Channel5, DISABLE);       // 关闭DMA ，防止干扰
    DMA_ClearFlag( DMA1_FLAG_GL5);           // 清DMA标志位
    stUart1Handle.u8RxLen = USART_RX_BUFF_MAX_LENGTH - DMA_GetCurrDataCounter(DMA1_Channel5); //获得接收到的字节数
    DMA1_Channel5->CNDTR = USART_RX_BUFF_MAX_LENGTH;    //  重新赋值计数值，必须大于等于最大可能接收到的数据帧数目
    DMA_Cmd(DMA1_Channel5, ENABLE);       //DMA 开启，等待数据。注意，如果中断发送数据帧的速率很快，MCU来不及处理此次接收到的数据，中断又发来数据的话，这里不能开启，否则数据会被覆盖。有2种方式解决。
}

// TX send done IRQ
void DMA1_Channel4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC4))
    {
        DMA_ClearFlag(DMA1_FLAG_GL4);         // 清除标志
    	DMA_Cmd(DMA1_Channel4, DISABLE);   // 关闭DMA通道
    	stUart1Handle.u8SendFlag = 0;
    }
}

void USART1_IRQHandler(void)
{
	//接收完成
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  // 空闲中断
    {
    	if(stUart1Handle.u8RecevFlag == 0 && stUart1Handle.u8RxLen < USART_RX_BUFF_MAX_LENGTH)
    	{
			 USART1_DMA_RX_Data();
    	}
       
        USART_ReceiveData(USART1); // Clear IDLE interrupt flag bit
        stUart1Handle.u8RecevFlag = 1;
    }
}

//
static void USART2_DMA_config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
    /* DMA clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 开启DMA1时钟
	//=DMA_Configuration===============================================//
	/*--- LUMMOD_UART_Rx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel6, DISABLE);                           // 关DMA通道
    DMA_DeInit(DMA1_Channel6);                                 // 恢复缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);// 设置串口接收数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart2Handle.u8DMARxData;         // 设置接收缓冲区首地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     // 设置外设为数据源，外设寄存器 -> 内存缓冲区
    DMA_InitStructure.DMA_BufferSize = USART_RX_BUFF_MAX_LENGTH;                     // 需要最大可能接收到的字节数
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不做增加调整，调整不调整是DMA自动实现的
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存缓冲区地址增加调整
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度8位，1个字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据宽度8位，1个字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 单次传输模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // 优先级设置
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // 关闭内存到内存的DMA模式
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);               // 写入配置
    DMA_ClearFlag(DMA1_FLAG_GL6);                                 // 清除DMA所有标志
    DMA_Cmd(DMA1_Channel6, ENABLE);                            // 开启接收DMA通道，等待接收数据
    
	/*--- LUMMOD_UART_Tx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel7, DISABLE);                           // 关DMA通道
    DMA_DeInit(DMA1_Channel7);                                 // 恢复缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned )(&USART2->DR);// 设置串口发送数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart2Handle.u8DMATxData;         // 设置发送缓冲区首地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // 设置外设位目标，内存缓冲区 ->外设寄存器
    DMA_InitStructure.DMA_BufferSize = 0;                     // 需要发送的字节数，这里其实可以设置为0，因为在实际要发送的时候，会重新设置次值
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不做增加调整，调整不调整是DMA自动实现的
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存缓冲区地址增加调整
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度8位，1个字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据宽度8位，1个字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 单次传输模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // 优先级设置
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // 关闭内存到内存的DMA模式
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);               // 写入配置
    DMA_ClearFlag(DMA1_FLAG_GL7);                                 // 清除DMA所有标志
    DMA_Cmd(DMA1_Channel7, DISABLE); // 关闭DMA
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);            // 开启发送DMA通道中断
}


//------------------------------------------------------------------//
static void USART2_Init(T_U32 u32BaudRate, T_U16 u16StopBit)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure;

    //= System Clocks Configuration ==============================//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,  ENABLE ); // 开启串口所在IO端口的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // 开始串口时钟

	//=NVIC_Configuration======================================//
   	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
   	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;   // 发送DMA通道的中断配置
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // 优先级设置
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

   /* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;     // 串口中断配置
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//=GPIO_Configuration====================================================//
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);  // 我这里没有用默认IO口，所以进行了重新映射，这个可以根据自己的硬件情况配置选择
	/* Configure USART2 Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // 串口接收IO口的设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		// PA3---RX
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Configure USART2 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // 串口发送IO口的设置
   	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 这里设置成复用形式的推挽输出   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		// PA2 -- TX
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	//------UART configuration----------------------------------------------------------//
	/* USART Format configuration ------------------------------------------------------*/
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // 串口格式配置
    USART_InitStructure.USART_StopBits = u16StopBit;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure USART2 */
    USART_InitStructure.USART_BaudRate = u32BaudRate;  //  波特率设置
    USART_Init(USART2, &USART_InitStructure);
    /* Enable USART2 Receive and Transmit interrupts */
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  // 开启 串口空闲IDEL 中断
	USART2_DMA_config();   // 串口 DMA 配置
    /* Enable USARTy DMA TX request */
	USART_Cmd(USART2, ENABLE);  // 开启串口
    USART_DMACmd(USART2, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);  // 开启串口DMA发送接收
//  USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); // 开启串口DMA接收
}


static T_U16 USART2_DMA_TX_Data(const uint8_t* buff,uint16_t length)
{
	stUart2Handle.u8SendFlag = 1;
	memcpy(stUart2Handle.u8DMATxData, buff, length);
    DMA1_Channel7->CNDTR = length; // 设置要发送的字节数目
    DMA_Cmd(DMA1_Channel7, ENABLE);        //开始DMA发送
    while(stUart2Handle.u8SendFlag == 1){
		;
	}
    return length;
}

static void USART2_DMA_RX_Data(void)
{
	DMA_Cmd(DMA1_Channel6, DISABLE);       // 关闭DMA ，防止干扰
    DMA_ClearFlag( DMA1_FLAG_GL6);           // 清DMA标志位
    stUart2Handle.u8RxLen = USART_RX_BUFF_MAX_LENGTH - DMA_GetCurrDataCounter(DMA1_Channel6); //获得接收到的字节数
    DMA1_Channel6->CNDTR = USART_RX_BUFF_MAX_LENGTH;    //  重新赋值计数值，必须大于等于最大可能接收到的数据帧数目
    DMA_Cmd(DMA1_Channel6, ENABLE);       //DMA 开启，等待数据。注意，如果中断发送数据帧的速率很快，MCU来不及处理此次接收到的数据，中断又发来数据的话，这里不能开启，否则数据会被覆盖。有2种方式解决。
}

// TX send done IRQ
void DMA1_Channel7_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC7))
    {
        DMA_ClearFlag(DMA1_FLAG_GL7);         // 清除标志
    	DMA_Cmd(DMA1_Channel7, DISABLE);   // 关闭DMA通道
    	stUart2Handle.u8SendFlag = 0;
    }
}

void USART2_IRQHandler(void)
{
	//接收完成
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  // 空闲中断
    {
    	if(stUart2Handle.u8RecevFlag == 0 && stUart2Handle.u8RxLen < USART_RX_BUFF_MAX_LENGTH)
		{
        	USART2_DMA_RX_Data();
        }
        
        USART_ReceiveData( USART2 ); // Clear IDLE interrupt flag bit
    }else
    {

    }

	stUart2Handle.u8RecevFlag = 1;
}


static void USART3_DMA_config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
    /* DMA clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 开启DMA1时钟
	//=DMA_Configuration===============================================//
	/*--- LUMMOD_UART_Rx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel3, DISABLE);                           // 关DMA通道
    DMA_DeInit(DMA1_Channel3);                                 // 恢复缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);// 设置串口接收数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart3Handle.u8DMARxData;         // 设置接收缓冲区首地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     // 设置外设为数据源，外设寄存器 -> 内存缓冲区
    DMA_InitStructure.DMA_BufferSize = USART_RX_BUFF_MAX_LENGTH;                     // 需要最大可能接收到的字节数
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不做增加调整，调整不调整是DMA自动实现的
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存缓冲区地址增加调整
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度8位，1个字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据宽度8位，1个字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 单次传输模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // 优先级设置
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // 关闭内存到内存的DMA模式
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);               // 写入配置
    DMA_ClearFlag(DMA1_FLAG_GL3);                                 // 清除DMA所有标志
    DMA_Cmd(DMA1_Channel3, ENABLE);                            // 开启接收DMA通道，等待接收数据
    
	/*--- LUMMOD_UART_Tx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel2, DISABLE);                           // 关DMA通道
    DMA_DeInit(DMA1_Channel2);                                 // 恢复缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned )(&USART3->DR);// 设置串口发送数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart3Handle.u8DMATxData;         // 设置发送缓冲区首地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // 设置外设位目标，内存缓冲区 ->外设寄存器
    DMA_InitStructure.DMA_BufferSize = 0;                     // 需要发送的字节数，这里其实可以设置为0，因为在实际要发送的时候，会重新设置次值
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不做增加调整，调整不调整是DMA自动实现的
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存缓冲区地址增加调整
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度8位，1个字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据宽度8位，1个字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 单次传输模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // 优先级设置
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // 关闭内存到内存的DMA模式
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);               // 写入配置
    DMA_ClearFlag(DMA1_FLAG_GL2);                                 // 清除DMA所有标志
    DMA_Cmd(DMA1_Channel2, DISABLE); // 关闭DMA
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);            // 开启发送DMA通道中断
 
}

//-----------------------------------------------------------------//
static T_VOID  USART3_Init(T_U32 u32BaudRate, T_U16 u16StopBit)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure;

	//= System Clocks Configuration ==============================//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,  ENABLE ); // 开启串口所在IO端口的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // 开始串口时钟

	//=NVIC_Configuration======================================//
   	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
   	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;   // 发送DMA通道的中断配置
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // 优先级设置
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;     // 串口中断配置
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//=GPIO_Configuration====================================================//
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);  // 我这里没有用默认IO口，所以进行了重新映射，这个可以根据自己的硬件情况配置选择
   	/* Configure USART3 Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // 串口接收IO口的设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Configure USART3 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // 串口发送IO口的设置
   	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 这里设置成复用形式的推挽输出   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	//------UART configuration----------------------------------------------------------//
	/* USART Format configuration ------------------------------------------------------*/
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // 串口格式配置
    USART_InitStructure.USART_StopBits = u16StopBit;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure USART3 */
    USART_InitStructure.USART_BaudRate = u32BaudRate;  //  波特率设置
    USART_Init(USART3, &USART_InitStructure);
    /* Enable USART3 Receive and Transmit interrupts */
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  // 开启 串口空闲IDEL 中断

	USART3_DMA_config();   // 串口 DMA 配置
	/* Enable USARTy DMA TX request */
	USART_Cmd(USART3, ENABLE);  // 开启串口
    USART_DMACmd(USART3, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);  // 开启串口DMA发送接收
	//USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE); // 开启串口DMA接收
}

static void USART3_DMA_RX_Data(void)
{
    DMA_Cmd(DMA1_Channel3, DISABLE);       // 关闭DMA ，防止干扰
    DMA_ClearFlag( DMA1_FLAG_GL3);           // 清DMA标志位
    stUart3Handle.u8RxLen = USART_RX_BUFF_MAX_LENGTH - DMA_GetCurrDataCounter(DMA1_Channel3); //获得接收到的字节数
    DMA1_Channel3->CNDTR = USART_RX_BUFF_MAX_LENGTH;    //  重新赋值计数值，必须大于等于最大可能接收到的数据帧数目
    DMA_Cmd(DMA1_Channel3, ENABLE);       //DMA 开启，等待数据。注意，如果中断发送数据帧的速率很快，MCU来不及处理此次接收到的数据，中断又发来数据的话，这里不能开启，否则数据会被覆盖。有2种方式解决。
}

static T_U16 USART3_DMA_TX_Data(uint8_t* buff,uint16_t length)
{
	stUart3Handle.u8SendFlag = 1;
    memcpy(stUart3Handle.u8DMATxData, buff, length);
    DMA1_Channel2->CNDTR = length; // 设置要发送的字节数目
    DMA_Cmd(DMA1_Channel2, ENABLE);        //开始DMA发送
    while(stUart3Handle.u8SendFlag == 1){
		;
	}

    return length;
}

// TX send done IRQ
void DMA1_Channel2_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_FLAG_TC2))
    {
        DMA_ClearFlag(DMA1_FLAG_GL2);         // 清除标志
    	DMA_Cmd(DMA1_Channel2, DISABLE);   // 关闭DMA通道
    	stUart3Handle.u8SendFlag = 0;
    }
}

// uart3 Rx receive
void USART3_IRQHandler(void)
{
	//接收完成
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  // 空闲中断
    {
    	if(stUart3Handle.u8RecevFlag == 0 && stUart3Handle.u8RxLen < USART_RX_BUFF_MAX_LENGTH)
		{
        	USART3_DMA_RX_Data();
        }
        
        USART_ReceiveData( USART3 ); // Clear IDLE interrupt flag bit

        stUart3Handle.u8RecevFlag = 1;
    }
}

/********************************************
 *功能：串口打开初始化接口
 *输入：USARTx，u32BaudRate，u16StopBit
 *输出：无
 *条件：无
 *返回：串口写数据的长度
 *注意:
********************************************/
T_S32 DRIVER_OpenUart(USART_TypeDef* USARTx, T_U32 u32BaudRate, T_U16 u16StopBit)
{
	if(USARTx == USART1)
	{
		USART1_Init(u32BaudRate, u16StopBit);
	}else if(USARTx == USART2)
	{
		USART2_Init(u32BaudRate,  u16StopBit);
	}else if(USARTx == USART3)
	{
		USART3_Init(u32BaudRate,  u16StopBit);
	}
	
	return RET_SUCCESS;
}

/********************************************
 *功能：串口数据读接口
 *输入：USARTx，ps8ReadBuf， u8WriteLen
 *输出：无
 *条件：无
 *返回：串口写数据的长度
 *注意:
********************************************/
T_S32 DRIVER_UartRead(USART_TypeDef* USARTx, T_S8 *ps8ReadBuf, T_U16 u16ReadLen)
{
	ASSERT_EQUAL_RETVAL(ps8ReadBuf, T_NULL, RET_FAILED);

	if(USARTx == USART1)
	{
		if(stUart1Handle.u8RxLen > 0)
		{
			memcpy(ps8ReadBuf, stUart1Handle.u8DMARxData, stUart1Handle.u8RxLen);
			memset(stUart1Handle.u8DMARxData, 0, sizeof(stUart1Handle.u8DMARxData));
			//stUart1Handle.u8RecevFlag = 0;
			if(u16ReadLen > stUart1Handle.u8RxLen)
			{
				u16ReadLen =  stUart1Handle.u8RxLen;
			}
			
			stUart1Handle.u8RxLen = 0;

			return u16ReadLen;
		}
	}else if(USARTx == USART2)
	{
		if( stUart2Handle.u8RxLen > 0)
		{
			memcpy(ps8ReadBuf, stUart2Handle.u8DMARxData, stUart2Handle.u8RxLen);
			memset(stUart2Handle.u8DMARxData, 0, sizeof(stUart2Handle.u8DMARxData));
			//stUart2Handle.u8RecevFlag = 0;
			if(u16ReadLen > stUart2Handle.u8RxLen)
			{
				u16ReadLen =  stUart2Handle.u8RxLen;
			}
			
			u16ReadLen =  stUart2Handle.u8RxLen;
			stUart2Handle.u8RxLen = 0;

			return u16ReadLen;
		}
	}else if(USARTx == USART3)
	{
		if(stUart3Handle.u8RxLen > 0)
		{
			memcpy(ps8ReadBuf, stUart3Handle.u8DMARxData, stUart3Handle.u8RxLen);
			memset(stUart3Handle.u8DMARxData, 0, sizeof(stUart3Handle.u8DMARxData));
			//stUart3Handle.u8RecevFlag = 0;
			if(u16ReadLen > stUart3Handle.u8RxLen)
			{
				u16ReadLen =  stUart3Handle.u8RxLen;
			}
			stUart3Handle.u8RxLen = 0;

			return u16ReadLen;
		}
	}
	
	return 0;
}

/********************************************
 *功能：串口数据写接口
 *输入：USARTx，ps8ReadBuf， u8WriteLen
 *输出：无
 *条件：无
 *返回：串口写数据的长度
 *注意:
********************************************/
T_S32 DRIVER_UartWrite(USART_TypeDef* USARTx, T_S8 *ps8WriteBuf, T_U8 u8WriteLen)
{
    ASSERT_EQUAL_RETVAL(ps8WriteBuf, T_NULL, RET_FAILED);
    
	if(USART1 == USARTx)
	{
		return USART1_DMA_TX_Data((T_U8 *)ps8WriteBuf, u8WriteLen);
	}else if(USART2 == USARTx)
	{
		return USART2_DMA_TX_Data((T_U8 *)ps8WriteBuf, u8WriteLen);
	}else if(USART3 == USARTx)
	{
		return USART3_DMA_TX_Data((T_U8 *)ps8WriteBuf, u8WriteLen);	
	}
	
	return RET_FAILED;
}
/********************************************
 *功能：串口数据可读接口
 *输入：USARTx
 *输出：无
 *条件：无
 *返回：串口写数据的长度
 *注意:
********************************************/
T_S32 DRIVER_IsUartData(USART_TypeDef* USARTx)
{
	if(USART1 == USARTx)
	{
		if(stUart1Handle.u8RecevFlag == 1)
		{
			stUart1Handle.u8RecevFlag = 0;
			return RET_SUCCESS;
		}
	}else if(USART2 == USARTx)
	{
		if(stUart2Handle.u8RecevFlag == 1)
		{
			stUart2Handle.u8RecevFlag = 0;
			return RET_SUCCESS;
		}
	}else if(USART3 == USARTx)
	{
		if(stUart3Handle.u8RecevFlag == 1)
		{
			stUart3Handle.u8RecevFlag = 0;
			return RET_SUCCESS;
		}
	}

	return RET_FAILED;
}






