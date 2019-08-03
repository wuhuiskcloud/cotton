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
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ¿ªÆôDMA1Ê±ÖÓ
	//=DMA_Configuration===============================================//
	/*--- LUMMOD_UART_Rx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel5, DISABLE);                           // ¹ØDMAÍ¨µÀ
    DMA_DeInit(DMA1_Channel5);                                 // »Ö¸´È±Ê¡Öµ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);// ÉèÖÃ´®¿Ú½ÓÊÕÊı¾İ¼Ä´æÆ÷
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart1Handle.u8DMARxData;         // ÉèÖÃ½ÓÊÕ»º³åÇøÊ×µØÖ·
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     // ÉèÖÃÍâÉèÎªÊı¾İÔ´£¬ÍâÉè¼Ä´æÆ÷ -> ÄÚ´æ»º³åÇø
    DMA_InitStructure.DMA_BufferSize = USART_RX_BUFF_MAX_LENGTH;                     // ĞèÒª×î´ó¿ÉÄÜ½ÓÊÕµ½µÄ×Ö½ÚÊı
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // ÍâÉèµØÖ·²»×öÔö¼Óµ÷Õû£¬µ÷Õû²»µ÷ÕûÊÇDMA×Ô¶¯ÊµÏÖµÄ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // ÄÚ´æ»º³åÇøµØÖ·Ôö¼Óµ÷Õû
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // ÍâÉèÊı¾İ¿í¶È8Î»£¬1¸ö×Ö½Ú
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // ÄÚ´æÊı¾İ¿í¶È8Î»£¬1¸ö×Ö½Ú
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // µ¥´Î´«ÊäÄ£Ê½
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ÓÅÏÈ¼¶ÉèÖÃ
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // ¹Ø±ÕÄÚ´æµ½ÄÚ´æµÄDMAÄ£Ê½
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);               // Ğ´ÈëÅäÖÃ
    DMA_ClearFlag(DMA1_FLAG_GL5);                                 // Çå³ıDMAËùÓĞ±êÖ¾
    DMA_Cmd(DMA1_Channel5, ENABLE);                            // ¿ªÆô½ÓÊÕDMAÍ¨µÀ£¬µÈ´ı½ÓÊÕÊı¾İ
	/*--- LUMMOD_UART_Tx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel4, DISABLE);                           // ¹ØDMAÍ¨µÀ
    DMA_DeInit(DMA1_Channel4);                                 // »Ö¸´È±Ê¡Öµ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned )(&USART1->DR);// ÉèÖÃ´®¿Ú·¢ËÍÊı¾İ¼Ä´æÆ÷
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart1Handle.u8DMATxData;         // ÉèÖÃ·¢ËÍ»º³åÇøÊ×µØÖ·
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // ÉèÖÃÍâÉèÎ»Ä¿±ê£¬ÄÚ´æ»º³åÇø ->ÍâÉè¼Ä´æÆ÷
    DMA_InitStructure.DMA_BufferSize = 0;                     // ĞèÒª·¢ËÍµÄ×Ö½ÚÊı£¬ÕâÀïÆäÊµ¿ÉÒÔÉèÖÃÎª0£¬ÒòÎªÔÚÊµ¼ÊÒª·¢ËÍµÄÊ±ºò£¬»áÖØĞÂÉèÖÃ´ÎÖµ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // ÍâÉèµØÖ·²»×öÔö¼Óµ÷Õû£¬µ÷Õû²»µ÷ÕûÊÇDMA×Ô¶¯ÊµÏÖµÄ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // ÄÚ´æ»º³åÇøµØÖ·Ôö¼Óµ÷Õû
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // ÍâÉèÊı¾İ¿í¶È8Î»£¬1¸ö×Ö½Ú
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // ÄÚ´æÊı¾İ¿í¶È8Î»£¬1¸ö×Ö½Ú
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // µ¥´Î´«ÊäÄ£Ê½
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ÓÅÏÈ¼¶ÉèÖÃ
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // ¹Ø±ÕÄÚ´æµ½ÄÚ´æµÄDMAÄ£Ê½
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);               // Ğ´ÈëÅäÖÃ
    DMA_ClearFlag(DMA1_FLAG_GL4);                                 // Çå³ıDMAËùÓĞ±êÖ¾
    DMA_Cmd(DMA1_Channel4, DISABLE); // ¹Ø±ÕDMA
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);            // ¿ªÆô·¢ËÍDMAÍ¨µÀÖĞ¶Ï
}
//---------------------------------------------------------------------//
static void USART1_Init(T_U32 u32BaudRate, T_U16 u16StopBit)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure;
	//= System Clocks Configuration ==============================//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,  ENABLE ); // ¿ªÆô´®¿ÚËùÔÚIO¶Ë¿ÚµÄÊ±ÖÓ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // ¿ªÊ¼´®¿ÚÊ±ÖÓ
	//=NVIC_Configuration======================================//
   	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
   	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;   // ·¢ËÍDMAÍ¨µÀµÄÖĞ¶ÏÅäÖÃ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // ÓÅÏÈ¼¶ÉèÖÃ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;     // ´®¿ÚÖĞ¶ÏÅäÖÃ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//=GPIO_Configuration====================================================//
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);  // ÎÒÕâÀïÃ»ÓĞÓÃÄ¬ÈÏIO¿Ú£¬ËùÒÔ½øĞĞÁËÖØĞÂÓ³Éä£¬Õâ¸ö¿ÉÒÔ¸ù¾İ×Ô¼ºµÄÓ²¼şÇé¿öÅäÖÃÑ¡Ôñ
	 /* Configure USART1 Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // ´®¿Ú½ÓÊÕIO¿ÚµÄÉèÖÃ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure USART1 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // ´®¿Ú·¢ËÍIO¿ÚµÄÉèÖÃ
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // ÕâÀïÉèÖÃ³É¸´ÓÃĞÎÊ½µÄÍÆÍìÊä³ö   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//------UART configuration----------------------------------------------------------//
	/* USART Format configuration ------------------------------------------------------*/
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // ´®¿Ú¸ñÊ½ÅäÖÃ
    USART_InitStructure.USART_StopBits = u16StopBit;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure USART1 */
    USART_InitStructure.USART_BaudRate = u32BaudRate;  //  ²¨ÌØÂÊÉèÖÃ
    USART_Init(USART1, &USART_InitStructure);
    /* Enable USART1 Receive and Transmit interrupts */
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  // ¿ªÆô ´®¿Ú¿ÕÏĞIDEL ÖĞ¶Ï
	USART1_DMA_config();   // ´®¿Ú DMA ÅäÖÃ
/* Enable USARTy DMA TX request */
	USART_Cmd(USART1, ENABLE);  // ¿ªÆô´®¿Ú
    USART_DMACmd(USART1, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);  // ¿ªÆô´®¿ÚDMA·¢ËÍ½ÓÊÕ
//  USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); // ¿ªÆô´®¿ÚDMA½ÓÊÕ

}


static T_U16 USART1_DMA_TX_Data(uint8_t* buff,uint16_t length)
{
	stUart1Handle.u8SendFlag = 1;
	memcpy(stUart1Handle.u8DMATxData, buff, length);
    DMA1_Channel4->CNDTR = length; // ÉèÖÃÒª·¢ËÍµÄ×Ö½ÚÊıÄ¿
    DMA_Cmd(DMA1_Channel4, ENABLE);        //¿ªÊ¼DMA·¢Ë
    while(stUart1Handle.u8SendFlag == 1){
		;
	}
    return length;
}

static void USART1_DMA_RX_Data(void)
{
	DMA_Cmd(DMA1_Channel5, DISABLE);       // ¹Ø±ÕDMA £¬·ÀÖ¹¸ÉÈÅ
    DMA_ClearFlag( DMA1_FLAG_GL5);           // ÇåDMA±êÖ¾Î»
    stUart1Handle.u8RxLen = USART_RX_BUFF_MAX_LENGTH - DMA_GetCurrDataCounter(DMA1_Channel5); //»ñµÃ½ÓÊÕµ½µÄ×Ö½ÚÊı
    DMA1_Channel5->CNDTR = USART_RX_BUFF_MAX_LENGTH;    //  ÖØĞÂ¸³Öµ¼ÆÊıÖµ£¬±ØĞë´óÓÚµÈÓÚ×î´ó¿ÉÄÜ½ÓÊÕµ½µÄÊı¾İÖ¡ÊıÄ¿
    DMA_Cmd(DMA1_Channel5, ENABLE);       //DMA ¿ªÆô£¬µÈ´ıÊı¾İ¡£×¢Òâ£¬Èç¹ûÖĞ¶Ï·¢ËÍÊı¾İÖ¡µÄËÙÂÊºÜ¿ì£¬MCUÀ´²»¼°´¦Àí´Ë´Î½ÓÊÕµ½µÄÊı¾İ£¬ÖĞ¶ÏÓÖ·¢À´Êı¾İµÄ»°£¬ÕâÀï²»ÄÜ¿ªÆô£¬·ñÔòÊı¾İ»á±»¸²¸Ç¡£ÓĞ2ÖÖ·½Ê½½â¾ö¡£
}

// TX send done IRQ
void DMA1_Channel4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC4))
    {
        DMA_ClearFlag(DMA1_FLAG_GL4);         // Çå³ı±êÖ¾
    	DMA_Cmd(DMA1_Channel4, DISABLE);   // ¹Ø±ÕDMAÍ¨µÀ
    	stUart1Handle.u8SendFlag = 0;
    }
}

void USART1_IRQHandler(void)
{
	//½ÓÊÕÍê³É
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  // ¿ÕÏĞÖĞ¶Ï
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
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ¿ªÆôDMA1Ê±ÖÓ
	//=DMA_Configuration===============================================//
	/*--- LUMMOD_UART_Rx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel6, DISABLE);                           // ¹ØDMAÍ¨µÀ
    DMA_DeInit(DMA1_Channel6);                                 // »Ö¸´È±Ê¡Öµ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);// ÉèÖÃ´®¿Ú½ÓÊÕÊı¾İ¼Ä´æÆ÷
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart2Handle.u8DMARxData;         // ÉèÖÃ½ÓÊÕ»º³åÇøÊ×µØÖ·
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     // ÉèÖÃÍâÉèÎªÊı¾İÔ´£¬ÍâÉè¼Ä´æÆ÷ -> ÄÚ´æ»º³åÇø
    DMA_InitStructure.DMA_BufferSize = USART_RX_BUFF_MAX_LENGTH;                     // ĞèÒª×î´ó¿ÉÄÜ½ÓÊÕµ½µÄ×Ö½ÚÊı
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // ÍâÉèµØÖ·²»×öÔö¼Óµ÷Õû£¬µ÷Õû²»µ÷ÕûÊÇDMA×Ô¶¯ÊµÏÖµÄ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // ÄÚ´æ»º³åÇøµØÖ·Ôö¼Óµ÷Õû
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // ÍâÉèÊı¾İ¿í¶È8Î»£¬1¸ö×Ö½Ú
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // ÄÚ´æÊı¾İ¿í¶È8Î»£¬1¸ö×Ö½Ú
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // µ¥´Î´«ÊäÄ£Ê½
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ÓÅÏÈ¼¶ÉèÖÃ
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // ¹Ø±ÕÄÚ´æµ½ÄÚ´æµÄDMAÄ£Ê½
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);               // Ğ´ÈëÅäÖÃ
    DMA_ClearFlag(DMA1_FLAG_GL6);                                 // Çå³ıDMAËùÓĞ±êÖ¾
    DMA_Cmd(DMA1_Channel6, ENABLE);                            // ¿ªÆô½ÓÊÕDMAÍ¨µÀ£¬µÈ´ı½ÓÊÕÊı¾İ
    
	/*--- LUMMOD_UART_Tx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel7, DISABLE);                           // ¹ØDMAÍ¨µÀ
    DMA_DeInit(DMA1_Channel7);                                 // »Ö¸´È±Ê¡Öµ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned )(&USART2->DR);// ÉèÖÃ´®¿Ú·¢ËÍÊı¾İ¼Ä´æÆ÷
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart2Handle.u8DMATxData;         // ÉèÖÃ·¢ËÍ»º³åÇøÊ×µØÖ·
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // ÉèÖÃÍâÉèÎ»Ä¿±ê£¬ÄÚ´æ»º³åÇø ->ÍâÉè¼Ä´æÆ÷
    DMA_InitStructure.DMA_BufferSize = 0;                     // ĞèÒª·¢ËÍµÄ×Ö½ÚÊı£¬ÕâÀïÆäÊµ¿ÉÒÔÉèÖÃÎª0£¬ÒòÎªÔÚÊµ¼ÊÒª·¢ËÍµÄÊ±ºò£¬»áÖØĞÂÉèÖÃ´ÎÖµ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // ÍâÉèµØÖ·²»×öÔö¼Óµ÷Õû£¬µ÷Õû²»µ÷ÕûÊÇDMA×Ô¶¯ÊµÏÖµÄ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // ÄÚ´æ»º³åÇøµØÖ·Ôö¼Óµ÷Õû
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // ÍâÉèÊı¾İ¿í¶È8Î»£¬1¸ö×Ö½Ú
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // ÄÚ´æÊı¾İ¿í¶È8Î»£¬1¸ö×Ö½Ú
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // µ¥´Î´«ÊäÄ£Ê½
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ÓÅÏÈ¼¶ÉèÖÃ
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // ¹Ø±ÕÄÚ´æµ½ÄÚ´æµÄDMAÄ£Ê½
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);               // Ğ´ÈëÅäÖÃ
    DMA_ClearFlag(DMA1_FLAG_GL7);                                 // Çå³ıDMAËùÓĞ±êÖ¾
    DMA_Cmd(DMA1_Channel7, DISABLE); // ¹Ø±ÕDMA
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);            // ¿ªÆô·¢ËÍDMAÍ¨µÀÖĞ¶Ï
}


//------------------------------------------------------------------//
static void USART2_Init(T_U32 u32BaudRate, T_U16 u16StopBit)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure;

    //= System Clocks Configuration ==============================//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,  ENABLE ); // ¿ªÆô´®¿ÚËùÔÚIO¶Ë¿ÚµÄÊ±ÖÓ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // ¿ªÊ¼´®¿ÚÊ±ÖÓ

	//=NVIC_Configuration======================================//
   	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
   	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;   // ·¢ËÍDMAÍ¨µÀµÄÖĞ¶ÏÅäÖÃ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // ÓÅÏÈ¼¶ÉèÖÃ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

   /* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;     // ´®¿ÚÖĞ¶ÏÅäÖÃ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//=GPIO_Configuration====================================================//
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);  // ÎÒÕâÀïÃ»ÓĞÓÃÄ¬ÈÏIO¿Ú£¬ËùÒÔ½øĞĞÁËÖØĞÂÓ³Éä£¬Õâ¸ö¿ÉÒÔ¸ù¾İ×Ô¼ºµÄÓ²¼şÇé¿öÅäÖÃÑ¡Ôñ
	/* Configure USART2 Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // ´®¿Ú½ÓÊÕIO¿ÚµÄÉèÖÃ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		// PA3---RX
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Configure USART2 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // ´®¿Ú·¢ËÍIO¿ÚµÄÉèÖÃ
   	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // ÕâÀïÉèÖÃ³É¸´ÓÃĞÎÊ½µÄÍÆÍìÊä³ö   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		// PA2 -- TX
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	//------UART configuration----------------------------------------------------------//
	/* USART Format configuration ------------------------------------------------------*/
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // ´®¿Ú¸ñÊ½ÅäÖÃ
    USART_InitStructure.USART_StopBits = u16StopBit;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure USART2 */
    USART_InitStructure.USART_BaudRate = u32BaudRate;  //  ²¨ÌØÂÊÉèÖÃ
    USART_Init(USART2, &USART_InitStructure);
    /* Enable USART2 Receive and Transmit interrupts */
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  // ¿ªÆô ´®¿Ú¿ÕÏĞIDEL ÖĞ¶Ï
	USART2_DMA_config();   // ´®¿Ú DMA ÅäÖÃ
    /* Enable USARTy DMA TX request */
	USART_Cmd(USART2, ENABLE);  // ¿ªÆô´®¿Ú
    USART_DMACmd(USART2, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);  // ¿ªÆô´®¿ÚDMA·¢ËÍ½ÓÊÕ
//  USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); // ¿ªÆô´®¿ÚDMA½ÓÊÕ
}


static T_U16 USART2_DMA_TX_Data(const uint8_t* buff,uint16_t length)
{
	stUart2Handle.u8SendFlag = 1;
	memcpy(stUart2Handle.u8DMATxData, buff, length);
    DMA1_Channel7->CNDTR = length; // ÉèÖÃÒª·¢ËÍµÄ×Ö½ÚÊıÄ¿
    DMA_Cmd(DMA1_Channel7, ENABLE);        //¿ªÊ¼DMA·¢ËÍ
    while(stUart2Handle.u8SendFlag == 1){
		;
	}
    return length;
}

static void USART2_DMA_RX_Data(void)
{
	DMA_Cmd(DMA1_Channel6, DISABLE);       // ¹Ø±ÕDMA £¬·ÀÖ¹¸ÉÈÅ
    DMA_ClearFlag( DMA1_FLAG_GL6);           // ÇåDMA±êÖ¾Î»
    stUart2Handle.u8RxLen = USART_RX_BUFF_MAX_LENGTH - DMA_GetCurrDataCounter(DMA1_Channel6); //»ñµÃ½ÓÊÕµ½µÄ×Ö½ÚÊı
    DMA1_Channel6->CNDTR = USART_RX_BUFF_MAX_LENGTH;    //  ÖØĞÂ¸³Öµ¼ÆÊıÖµ£¬±ØĞë´óÓÚµÈÓÚ×î´ó¿ÉÄÜ½ÓÊÕµ½µÄÊı¾İÖ¡ÊıÄ¿
    DMA_Cmd(DMA1_Channel6, ENABLE);       //DMA ¿ªÆô£¬µÈ´ıÊı¾İ¡£×¢Òâ£¬Èç¹ûÖĞ¶Ï·¢ËÍÊı¾İÖ¡µÄËÙÂÊºÜ¿ì£¬MCUÀ´²»¼°´¦Àí´Ë´Î½ÓÊÕµ½µÄÊı¾İ£¬ÖĞ¶ÏÓÖ·¢À´Êı¾İµÄ»°£¬ÕâÀï²»ÄÜ¿ªÆô£¬·ñÔòÊı¾İ»á±»¸²¸Ç¡£ÓĞ2ÖÖ·½Ê½½â¾ö¡£
}

// TX send done IRQ
void DMA1_Channel7_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC7))
    {
        DMA_ClearFlag(DMA1_FLAG_GL7);         // Çå³ı±êÖ¾
    	DMA_Cmd(DMA1_Channel7, DISABLE);   // ¹Ø±ÕDMAÍ¨µÀ
    	stUart2Handle.u8SendFlag = 0;
    }
}

void USART2_IRQHandler(void)
{
	//½ÓÊÕÍê³É
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  // ¿ÕÏĞÖĞ¶Ï
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
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ¿ªÆôDMA1Ê±ÖÓ
	//=DMA_Configuration===============================================//
	/*--- LUMMOD_UART_Rx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel3, DISABLE);                           // ¹ØDMAÍ¨µÀ
    DMA_DeInit(DMA1_Channel3);                                 // »Ö¸´È±Ê¡Öµ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);// ÉèÖÃ´®¿Ú½ÓÊÕÊı¾İ¼Ä´æÆ÷
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart3Handle.u8DMARxData;         // ÉèÖÃ½ÓÊÕ»º³åÇøÊ×µØÖ·
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     // ÉèÖÃÍâÉèÎªÊı¾İÔ´£¬ÍâÉè¼Ä´æÆ÷ -> ÄÚ´æ»º³åÇø
    DMA_InitStructure.DMA_BufferSize = USART_RX_BUFF_MAX_LENGTH;                     // ĞèÒª×î´ó¿ÉÄÜ½ÓÊÕµ½µÄ×Ö½ÚÊı
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // ÍâÉèµØÖ·²»×öÔö¼Óµ÷Õû£¬µ÷Õû²»µ÷ÕûÊÇDMA×Ô¶¯ÊµÏÖµÄ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // ÄÚ´æ»º³åÇøµØÖ·Ôö¼Óµ÷Õû
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // ÍâÉèÊı¾İ¿í¶È8Î»£¬1¸ö×Ö½Ú
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // ÄÚ´æÊı¾İ¿í¶È8Î»£¬1¸ö×Ö½Ú
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // µ¥´Î´«ÊäÄ£Ê½
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ÓÅÏÈ¼¶ÉèÖÃ
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // ¹Ø±ÕÄÚ´æµ½ÄÚ´æµÄDMAÄ£Ê½
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);               // Ğ´ÈëÅäÖÃ
    DMA_ClearFlag(DMA1_FLAG_GL3);                                 // Çå³ıDMAËùÓĞ±êÖ¾
    DMA_Cmd(DMA1_Channel3, ENABLE);                            // ¿ªÆô½ÓÊÕDMAÍ¨µÀ£¬µÈ´ı½ÓÊÕÊı¾İ
    
	/*--- LUMMOD_UART_Tx_DMA_Channel DMA Config ---*/
    DMA_Cmd(DMA1_Channel2, DISABLE);                           // ¹ØDMAÍ¨µÀ
    DMA_DeInit(DMA1_Channel2);                                 // »Ö¸´È±Ê¡Öµ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned )(&USART3->DR);// ÉèÖÃ´®¿Ú·¢ËÍÊı¾İ¼Ä´æÆ÷
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)stUart3Handle.u8DMATxData;         // ÉèÖÃ·¢ËÍ»º³åÇøÊ×µØÖ·
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // ÉèÖÃÍâÉèÎ»Ä¿±ê£¬ÄÚ´æ»º³åÇø ->ÍâÉè¼Ä´æÆ÷
    DMA_InitStructure.DMA_BufferSize = 0;                     // ĞèÒª·¢ËÍµÄ×Ö½ÚÊı£¬ÕâÀïÆäÊµ¿ÉÒÔÉèÖÃÎª0£¬ÒòÎªÔÚÊµ¼ÊÒª·¢ËÍµÄÊ±ºò£¬»áÖØĞÂÉèÖÃ´ÎÖµ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // ÍâÉèµØÖ·²»×öÔö¼Óµ÷Õû£¬µ÷Õû²»µ÷ÕûÊÇDMA×Ô¶¯ÊµÏÖµÄ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // ÄÚ´æ»º³åÇøµØÖ·Ôö¼Óµ÷Õû
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // ÍâÉèÊı¾İ¿í¶È8Î»£¬1¸ö×Ö½Ú
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // ÄÚ´æÊı¾İ¿í¶È8Î»£¬1¸ö×Ö½Ú
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // µ¥´Î´«ÊäÄ£Ê½
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ÓÅÏÈ¼¶ÉèÖÃ
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // ¹Ø±ÕÄÚ´æµ½ÄÚ´æµÄDMAÄ£Ê½
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);               // Ğ´ÈëÅäÖÃ
    DMA_ClearFlag(DMA1_FLAG_GL2);                                 // Çå³ıDMAËùÓĞ±êÖ¾
    DMA_Cmd(DMA1_Channel2, DISABLE); // ¹Ø±ÕDMA
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);            // ¿ªÆô·¢ËÍDMAÍ¨µÀÖĞ¶Ï
 
}

//-----------------------------------------------------------------//
static T_VOID  USART3_Init(T_U32 u32BaudRate, T_U16 u16StopBit)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure;

	//= System Clocks Configuration ==============================//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,  ENABLE ); // ¿ªÆô´®¿ÚËùÔÚIO¶Ë¿ÚµÄÊ±ÖÓ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // ¿ªÊ¼´®¿ÚÊ±ÖÓ

	//=NVIC_Configuration======================================//
   	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
   	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;   // ·¢ËÍDMAÍ¨µÀµÄÖĞ¶ÏÅäÖÃ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // ÓÅÏÈ¼¶ÉèÖÃ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;     // ´®¿ÚÖĞ¶ÏÅäÖÃ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//=GPIO_Configuration====================================================//
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);  // ÎÒÕâÀïÃ»ÓĞÓÃÄ¬ÈÏIO¿Ú£¬ËùÒÔ½øĞĞÁËÖØĞÂÓ³Éä£¬Õâ¸ö¿ÉÒÔ¸ù¾İ×Ô¼ºµÄÓ²¼şÇé¿öÅäÖÃÑ¡Ôñ
   	/* Configure USART3 Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // ´®¿Ú½ÓÊÕIO¿ÚµÄÉèÖÃ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Configure USART3 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // ´®¿Ú·¢ËÍIO¿ÚµÄÉèÖÃ
   	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // ÕâÀïÉèÖÃ³É¸´ÓÃĞÎÊ½µÄÍÆÍìÊä³ö   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	//------UART configuration----------------------------------------------------------//
	/* USART Format configuration ------------------------------------------------------*/
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // ´®¿Ú¸ñÊ½ÅäÖÃ
    USART_InitStructure.USART_StopBits = u16StopBit;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure USART3 */
    USART_InitStructure.USART_BaudRate = u32BaudRate;  //  ²¨ÌØÂÊÉèÖÃ
    USART_Init(USART3, &USART_InitStructure);
    /* Enable USART3 Receive and Transmit interrupts */
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  // ¿ªÆô ´®¿Ú¿ÕÏĞIDEL ÖĞ¶Ï

	USART3_DMA_config();   // ´®¿Ú DMA ÅäÖÃ
	/* Enable USARTy DMA TX request */
	USART_Cmd(USART3, ENABLE);  // ¿ªÆô´®¿Ú
    USART_DMACmd(USART3, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);  // ¿ªÆô´®¿ÚDMA·¢ËÍ½ÓÊÕ
	//USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE); // ¿ªÆô´®¿ÚDMA½ÓÊÕ
}

static void USART3_DMA_RX_Data(void)
{
    DMA_Cmd(DMA1_Channel3, DISABLE);       // ¹Ø±ÕDMA £¬·ÀÖ¹¸ÉÈÅ
    DMA_ClearFlag( DMA1_FLAG_GL3);           // ÇåDMA±êÖ¾Î»
    stUart3Handle.u8RxLen = USART_RX_BUFF_MAX_LENGTH - DMA_GetCurrDataCounter(DMA1_Channel3); //»ñµÃ½ÓÊÕµ½µÄ×Ö½ÚÊı
    DMA1_Channel3->CNDTR = USART_RX_BUFF_MAX_LENGTH;    //  ÖØĞÂ¸³Öµ¼ÆÊıÖµ£¬±ØĞë´óÓÚµÈÓÚ×î´ó¿ÉÄÜ½ÓÊÕµ½µÄÊı¾İÖ¡ÊıÄ¿
    DMA_Cmd(DMA1_Channel3, ENABLE);       //DMA ¿ªÆô£¬µÈ´ıÊı¾İ¡£×¢Òâ£¬Èç¹ûÖĞ¶Ï·¢ËÍÊı¾İÖ¡µÄËÙÂÊºÜ¿ì£¬MCUÀ´²»¼°´¦Àí´Ë´Î½ÓÊÕµ½µÄÊı¾İ£¬ÖĞ¶ÏÓÖ·¢À´Êı¾İµÄ»°£¬ÕâÀï²»ÄÜ¿ªÆô£¬·ñÔòÊı¾İ»á±»¸²¸Ç¡£ÓĞ2ÖÖ·½Ê½½â¾ö¡£
}

static T_U16 USART3_DMA_TX_Data(uint8_t* buff,uint16_t length)
{
	stUart3Handle.u8SendFlag = 1;
    memcpy(stUart3Handle.u8DMATxData, buff, length);
    DMA1_Channel2->CNDTR = length; // ÉèÖÃÒª·¢ËÍµÄ×Ö½ÚÊıÄ¿
    DMA_Cmd(DMA1_Channel2, ENABLE);        //¿ªÊ¼DMA·¢ËÍ
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
        DMA_ClearFlag(DMA1_FLAG_GL2);         // Çå³ı±êÖ¾
    	DMA_Cmd(DMA1_Channel2, DISABLE);   // ¹Ø±ÕDMAÍ¨µÀ
    	stUart3Handle.u8SendFlag = 0;
    }
}

// uart3 Rx receive
void USART3_IRQHandler(void)
{
	//½ÓÊÕÍê³É
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  // ¿ÕÏĞÖĞ¶Ï
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
 *¹¦ÄÜ£º´®¿Ú´ò¿ª³õÊ¼»¯½Ó¿Ú
 *ÊäÈë£ºUSARTx£¬u32BaudRate£¬u16StopBit
 *Êä³ö£ºÎŞ
 *Ìõ¼ş£ºÎŞ
 *·µ»Ø£º´®¿ÚĞ´Êı¾İµÄ³¤¶È
 *×¢Òâ:
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
 *¹¦ÄÜ£º´®¿ÚÊı¾İ¶Á½Ó¿Ú
 *ÊäÈë£ºUSARTx£¬ps8ReadBuf£¬ u8WriteLen
 *Êä³ö£ºÎŞ
 *Ìõ¼ş£ºÎŞ
 *·µ»Ø£º´®¿ÚĞ´Êı¾İµÄ³¤¶È
 *×¢Òâ:
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
 *¹¦ÄÜ£º´®¿ÚÊı¾İĞ´½Ó¿Ú
 *ÊäÈë£ºUSARTx£¬ps8ReadBuf£¬ u8WriteLen
 *Êä³ö£ºÎŞ
 *Ìõ¼ş£ºÎŞ
 *·µ»Ø£º´®¿ÚĞ´Êı¾İµÄ³¤¶È
 *×¢Òâ:
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
 *¹¦ÄÜ£º´®¿ÚÊı¾İ¿É¶Á½Ó¿Ú
 *ÊäÈë£ºUSARTx
 *Êä³ö£ºÎŞ
 *Ìõ¼ş£ºÎŞ
 *·µ»Ø£º´®¿ÚĞ´Êı¾İµÄ³¤¶È
 *×¢Òâ:
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






