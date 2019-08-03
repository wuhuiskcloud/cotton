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
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ����DMA1ʱ��
	//=DMA_Configuration===============================================//
	/*--- LUMMOD_UART_Rx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel5, DISABLE);                           // ��DMAͨ��
    DMA_DeInit(DMA1_Channel5);                                 // �ָ�ȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);// ���ô��ڽ������ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart1Handle.u8DMARxData;         // ���ý��ջ������׵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     // ��������Ϊ����Դ������Ĵ��� -> �ڴ滺����
    DMA_InitStructure.DMA_BufferSize = USART_RX_BUFF_MAX_LENGTH;                     // ��Ҫ�����ܽ��յ����ֽ���
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ��DMAģʽ
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);               // д������
    DMA_ClearFlag(DMA1_FLAG_GL5);                                 // ���DMA���б�־
    DMA_Cmd(DMA1_Channel5, ENABLE);                            // ��������DMAͨ�����ȴ���������
	/*--- LUMMOD_UART_Tx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel4, DISABLE);                           // ��DMAͨ��
    DMA_DeInit(DMA1_Channel4);                                 // �ָ�ȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned )(&USART1->DR);// ���ô��ڷ������ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart1Handle.u8DMATxData;         // ���÷��ͻ������׵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // ��������λĿ�꣬�ڴ滺���� ->����Ĵ���
    DMA_InitStructure.DMA_BufferSize = 0;                     // ��Ҫ���͵��ֽ�����������ʵ��������Ϊ0����Ϊ��ʵ��Ҫ���͵�ʱ�򣬻��������ô�ֵ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ��DMAģʽ
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);               // д������
    DMA_ClearFlag(DMA1_FLAG_GL4);                                 // ���DMA���б�־
    DMA_Cmd(DMA1_Channel4, DISABLE); // �ر�DMA
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);            // ��������DMAͨ���ж�
}
//---------------------------------------------------------------------//
static void USART1_Init(T_U32 u32BaudRate, T_U16 u16StopBit)
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
    USART_InitStructure.USART_StopBits = u16StopBit;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure USART1 */
    USART_InitStructure.USART_BaudRate = u32BaudRate;  //  ����������
    USART_Init(USART1, &USART_InitStructure);
    /* Enable USART1 Receive and Transmit interrupts */
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  // ���� ���ڿ���IDEL �ж�
	USART1_DMA_config();   // ���� DMA ����
/* Enable USARTy DMA TX request */
	USART_Cmd(USART1, ENABLE);  // ��������
    USART_DMACmd(USART1, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);  // ��������DMA���ͽ���
//  USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); // ��������DMA����

}


static T_U16 USART1_DMA_TX_Data(uint8_t* buff,uint16_t length)
{
	stUart1Handle.u8SendFlag = 1;
	memcpy(stUart1Handle.u8DMATxData, buff, length);
    DMA1_Channel4->CNDTR = length; // ����Ҫ���͵��ֽ���Ŀ
    DMA_Cmd(DMA1_Channel4, ENABLE);        //��ʼDMA���
    while(stUart1Handle.u8SendFlag == 1){
		;
	}
    return length;
}

static void USART1_DMA_RX_Data(void)
{
	DMA_Cmd(DMA1_Channel5, DISABLE);       // �ر�DMA ����ֹ����
    DMA_ClearFlag( DMA1_FLAG_GL5);           // ��DMA��־λ
    stUart1Handle.u8RxLen = USART_RX_BUFF_MAX_LENGTH - DMA_GetCurrDataCounter(DMA1_Channel5); //��ý��յ����ֽ���
    DMA1_Channel5->CNDTR = USART_RX_BUFF_MAX_LENGTH;    //  ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ
    DMA_Cmd(DMA1_Channel5, ENABLE);       //DMA �������ȴ����ݡ�ע�⣬����жϷ�������֡�����ʺܿ죬MCU����������˴ν��յ������ݣ��ж��ַ������ݵĻ������ﲻ�ܿ������������ݻᱻ���ǡ���2�ַ�ʽ�����
}

// TX send done IRQ
void DMA1_Channel4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC4))
    {
        DMA_ClearFlag(DMA1_FLAG_GL4);         // �����־
    	DMA_Cmd(DMA1_Channel4, DISABLE);   // �ر�DMAͨ��
    	stUart1Handle.u8SendFlag = 0;
    }
}

void USART1_IRQHandler(void)
{
	//�������
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  // �����ж�
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
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ����DMA1ʱ��
	//=DMA_Configuration===============================================//
	/*--- LUMMOD_UART_Rx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel6, DISABLE);                           // ��DMAͨ��
    DMA_DeInit(DMA1_Channel6);                                 // �ָ�ȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);// ���ô��ڽ������ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart2Handle.u8DMARxData;         // ���ý��ջ������׵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     // ��������Ϊ����Դ������Ĵ��� -> �ڴ滺����
    DMA_InitStructure.DMA_BufferSize = USART_RX_BUFF_MAX_LENGTH;                     // ��Ҫ�����ܽ��յ����ֽ���
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ��DMAģʽ
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);               // д������
    DMA_ClearFlag(DMA1_FLAG_GL6);                                 // ���DMA���б�־
    DMA_Cmd(DMA1_Channel6, ENABLE);                            // ��������DMAͨ�����ȴ���������
    
	/*--- LUMMOD_UART_Tx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel7, DISABLE);                           // ��DMAͨ��
    DMA_DeInit(DMA1_Channel7);                                 // �ָ�ȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned )(&USART2->DR);// ���ô��ڷ������ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart2Handle.u8DMATxData;         // ���÷��ͻ������׵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // ��������λĿ�꣬�ڴ滺���� ->����Ĵ���
    DMA_InitStructure.DMA_BufferSize = 0;                     // ��Ҫ���͵��ֽ�����������ʵ��������Ϊ0����Ϊ��ʵ��Ҫ���͵�ʱ�򣬻��������ô�ֵ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ��DMAģʽ
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);               // д������
    DMA_ClearFlag(DMA1_FLAG_GL7);                                 // ���DMA���б�־
    DMA_Cmd(DMA1_Channel7, DISABLE); // �ر�DMA
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);            // ��������DMAͨ���ж�
}


//------------------------------------------------------------------//
static void USART2_Init(T_U32 u32BaudRate, T_U16 u16StopBit)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure;

    //= System Clocks Configuration ==============================//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,  ENABLE ); // ������������IO�˿ڵ�ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // ��ʼ����ʱ��

	//=NVIC_Configuration======================================//
   	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
   	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;   // ����DMAͨ�����ж�����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // ���ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

   /* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;     // �����ж�����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//=GPIO_Configuration====================================================//
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);  // ������û����Ĭ��IO�ڣ����Խ���������ӳ�䣬������Ը����Լ���Ӳ���������ѡ��
	/* Configure USART2 Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // ���ڽ���IO�ڵ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		// PA3---RX
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Configure USART2 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // ���ڷ���IO�ڵ�����
   	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // �������óɸ�����ʽ���������   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		// PA2 -- TX
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	//------UART configuration----------------------------------------------------------//
	/* USART Format configuration ------------------------------------------------------*/
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // ���ڸ�ʽ����
    USART_InitStructure.USART_StopBits = u16StopBit;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure USART2 */
    USART_InitStructure.USART_BaudRate = u32BaudRate;  //  ����������
    USART_Init(USART2, &USART_InitStructure);
    /* Enable USART2 Receive and Transmit interrupts */
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  // ���� ���ڿ���IDEL �ж�
	USART2_DMA_config();   // ���� DMA ����
    /* Enable USARTy DMA TX request */
	USART_Cmd(USART2, ENABLE);  // ��������
    USART_DMACmd(USART2, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);  // ��������DMA���ͽ���
//  USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); // ��������DMA����
}


static T_U16 USART2_DMA_TX_Data(const uint8_t* buff,uint16_t length)
{
	stUart2Handle.u8SendFlag = 1;
	memcpy(stUart2Handle.u8DMATxData, buff, length);
    DMA1_Channel7->CNDTR = length; // ����Ҫ���͵��ֽ���Ŀ
    DMA_Cmd(DMA1_Channel7, ENABLE);        //��ʼDMA����
    while(stUart2Handle.u8SendFlag == 1){
		;
	}
    return length;
}

static void USART2_DMA_RX_Data(void)
{
	DMA_Cmd(DMA1_Channel6, DISABLE);       // �ر�DMA ����ֹ����
    DMA_ClearFlag( DMA1_FLAG_GL6);           // ��DMA��־λ
    stUart2Handle.u8RxLen = USART_RX_BUFF_MAX_LENGTH - DMA_GetCurrDataCounter(DMA1_Channel6); //��ý��յ����ֽ���
    DMA1_Channel6->CNDTR = USART_RX_BUFF_MAX_LENGTH;    //  ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ
    DMA_Cmd(DMA1_Channel6, ENABLE);       //DMA �������ȴ����ݡ�ע�⣬����жϷ�������֡�����ʺܿ죬MCU����������˴ν��յ������ݣ��ж��ַ������ݵĻ������ﲻ�ܿ������������ݻᱻ���ǡ���2�ַ�ʽ�����
}

// TX send done IRQ
void DMA1_Channel7_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC7))
    {
        DMA_ClearFlag(DMA1_FLAG_GL7);         // �����־
    	DMA_Cmd(DMA1_Channel7, DISABLE);   // �ر�DMAͨ��
    	stUart2Handle.u8SendFlag = 0;
    }
}

void USART2_IRQHandler(void)
{
	//�������
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  // �����ж�
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
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ����DMA1ʱ��
	//=DMA_Configuration===============================================//
	/*--- LUMMOD_UART_Rx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel3, DISABLE);                           // ��DMAͨ��
    DMA_DeInit(DMA1_Channel3);                                 // �ָ�ȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);// ���ô��ڽ������ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart3Handle.u8DMARxData;         // ���ý��ջ������׵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     // ��������Ϊ����Դ������Ĵ��� -> �ڴ滺����
    DMA_InitStructure.DMA_BufferSize = USART_RX_BUFF_MAX_LENGTH;                     // ��Ҫ�����ܽ��յ����ֽ���
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ��DMAģʽ
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);               // д������
    DMA_ClearFlag(DMA1_FLAG_GL3);                                 // ���DMA���б�־
    DMA_Cmd(DMA1_Channel3, ENABLE);                            // ��������DMAͨ�����ȴ���������
    
	/*--- LUMMOD_UART_Tx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel2, DISABLE);                           // ��DMAͨ��
    DMA_DeInit(DMA1_Channel2);                                 // �ָ�ȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned )(&USART3->DR);// ���ô��ڷ������ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart3Handle.u8DMATxData;         // ���÷��ͻ������׵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // ��������λĿ�꣬�ڴ滺���� ->����Ĵ���
    DMA_InitStructure.DMA_BufferSize = 0;                     // ��Ҫ���͵��ֽ�����������ʵ��������Ϊ0����Ϊ��ʵ��Ҫ���͵�ʱ�򣬻��������ô�ֵ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ��DMAģʽ
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);               // д������
    DMA_ClearFlag(DMA1_FLAG_GL2);                                 // ���DMA���б�־
    DMA_Cmd(DMA1_Channel2, DISABLE); // �ر�DMA
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);            // ��������DMAͨ���ж�
 
}

//-----------------------------------------------------------------//
static T_VOID  USART3_Init(T_U32 u32BaudRate, T_U16 u16StopBit)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure;

	//= System Clocks Configuration ==============================//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,  ENABLE ); // ������������IO�˿ڵ�ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // ��ʼ����ʱ��

	//=NVIC_Configuration======================================//
   	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
   	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;   // ����DMAͨ�����ж�����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // ���ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;     // �����ж�����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//=GPIO_Configuration====================================================//
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);  // ������û����Ĭ��IO�ڣ����Խ���������ӳ�䣬������Ը����Լ���Ӳ���������ѡ��
   	/* Configure USART3 Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // ���ڽ���IO�ڵ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Configure USART3 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // ���ڷ���IO�ڵ�����
   	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // �������óɸ�����ʽ���������   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	//------UART configuration----------------------------------------------------------//
	/* USART Format configuration ------------------------------------------------------*/
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // ���ڸ�ʽ����
    USART_InitStructure.USART_StopBits = u16StopBit;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure USART3 */
    USART_InitStructure.USART_BaudRate = u32BaudRate;  //  ����������
    USART_Init(USART3, &USART_InitStructure);
    /* Enable USART3 Receive and Transmit interrupts */
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  // ���� ���ڿ���IDEL �ж�

	USART3_DMA_config();   // ���� DMA ����
	/* Enable USARTy DMA TX request */
	USART_Cmd(USART3, ENABLE);  // ��������
    USART_DMACmd(USART3, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);  // ��������DMA���ͽ���
	//USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE); // ��������DMA����
}

static void USART3_DMA_RX_Data(void)
{
    DMA_Cmd(DMA1_Channel3, DISABLE);       // �ر�DMA ����ֹ����
    DMA_ClearFlag( DMA1_FLAG_GL3);           // ��DMA��־λ
    stUart3Handle.u8RxLen = USART_RX_BUFF_MAX_LENGTH - DMA_GetCurrDataCounter(DMA1_Channel3); //��ý��յ����ֽ���
    DMA1_Channel3->CNDTR = USART_RX_BUFF_MAX_LENGTH;    //  ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ
    DMA_Cmd(DMA1_Channel3, ENABLE);       //DMA �������ȴ����ݡ�ע�⣬����жϷ�������֡�����ʺܿ죬MCU����������˴ν��յ������ݣ��ж��ַ������ݵĻ������ﲻ�ܿ������������ݻᱻ���ǡ���2�ַ�ʽ�����
}

static T_U16 USART3_DMA_TX_Data(uint8_t* buff,uint16_t length)
{
	stUart3Handle.u8SendFlag = 1;
    memcpy(stUart3Handle.u8DMATxData, buff, length);
    DMA1_Channel2->CNDTR = length; // ����Ҫ���͵��ֽ���Ŀ
    DMA_Cmd(DMA1_Channel2, ENABLE);        //��ʼDMA����
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
        DMA_ClearFlag(DMA1_FLAG_GL2);         // �����־
    	DMA_Cmd(DMA1_Channel2, DISABLE);   // �ر�DMAͨ��
    	stUart3Handle.u8SendFlag = 0;
    }
}

// uart3 Rx receive
void USART3_IRQHandler(void)
{
	//�������
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  // �����ж�
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
 *���ܣ����ڴ򿪳�ʼ���ӿ�
 *���룺USARTx��u32BaudRate��u16StopBit
 *�������
 *��������
 *���أ�����д���ݵĳ���
 *ע��:
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
 *���ܣ��������ݶ��ӿ�
 *���룺USARTx��ps8ReadBuf�� u8WriteLen
 *�������
 *��������
 *���أ�����д���ݵĳ���
 *ע��:
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
 *���ܣ���������д�ӿ�
 *���룺USARTx��ps8ReadBuf�� u8WriteLen
 *�������
 *��������
 *���أ�����д���ݵĳ���
 *ע��:
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
 *���ܣ��������ݿɶ��ӿ�
 *���룺USARTx
 *�������
 *��������
 *���أ�����д���ݵĳ���
 *ע��:
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






