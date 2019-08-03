/*********************************************
* @�ļ�: driverGpio.c
* @����: wuhuiskt
* @�汾: v1.0.1
* @ʱ��: 2019-6-1
* @��Ҫ: GPIO������������������״̬��
*********************************************/
#if 0
#include "baseType.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x.h"
#include "driverFile.h"
#include "driverTimer.h"
#include "driverTime.h"
#include "ioctrl.h"
#include "driverManager.h"

/*��ͨGPIO ������ú궨��*/

#define SWITCH_GPIO_PORT 		GPIOC
#define SWITCH_GPIO_PIN 		GPIO_Pin_13

#define POWER4G_GPIO_PORT GPIOC
#define POWER4G_GPIO_PIN GPIO_Pin_9
#define SET_4GPOWER_GPIO(val) 	GPIO_WriteBit(POWER4G_GPIO_PORT, POWER4G_GPIO_PIN, (BitAction)val)

#define RESET4G_GPIO_PORT GPIOB
#define RESET4G_GPIO_PIN GPIO_Pin_0
#define SET_4GRESET_GPIO(val) 	GPIO_WriteBit(RESET4G_GPIO_PORT, RESET4G_GPIO_PIN, (BitAction)val)

#define LED_GPIO_PORT GPIOC
#define LED_GPIO_PIN GPIO_Pin_7
#define SET_LED_GPIO(val) 	GPIO_WriteBit(LED_GPIO_PORT, LED_GPIO_PIN, (BitAction)val)

#define FQ_GPIO_PORT GPIOA
#define FQ_GPIO_PIN GPIO_Pin_0
#define SET_FQ_GPIO(val) 	GPIO_WriteBit(FQ_GPIO_PORT, FQ_GPIO_PIN, (BitAction)val)

#define STRONG_GPIO_PORT GPIOB
#define STRONG_GPIO_PIN GPIO_Pin_11
#define SET_STRONG_GPIO(val) 	GPIO_WriteBit(STRONG_GPIO_PORT, STRONG_GPIO_PIN, (BitAction)val)

#define RESETIPC_GPIO_PORT GPIOA
#define RESETIPC_GPIO_PIN GPIO_Pin_3
#define SET_RESETIPC_GPIO(val) 	GPIO_WriteBit(RESETIPC_GPIO_PORT, RESETIPC_GPIO_PIN, (BitAction)val)

#define RESET_GPIO_PORT 		GPIOA
#define RESET_GPIO_PIN 			GPIO_Pin_4

#define RELAY_GPIO_PORT  		GPIOA
#define RELAY_GPIO_PIN 			GPIO_Pin_2
#define SET_RELAY_GPIO(val) 	GPIO_WriteBit(RELAY_GPIO_PORT, RELAY_GPIO_PIN, (BitAction)val)

static T_U8 u8ResetPopFlag = 0;
static GpioCrtl g_stGpioCrtl;

/*������������ú궨��*/
#define Beep_TimerIRQHandler  	TIM3_IRQHandler
#define Beep_GPIOH() 		  	GPIO_SetBits(GPIOA, GPIO_Pin_1)
#define Beep_GPIOL() 		  	GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#define Beep_TimerStop()	  	TIM_Cmd(BEEP_TIME_ID, DISABLE)
#define Beep_TimerStart()	  	TIM_Cmd(BEEP_TIME_ID, ENABLE)
#define BEEP_TIME_ID			TIM3
#define BEEP_FREQ				2000 //������Ƶ��Hz
#define BEEP_OPEN_DELAY_TIME   	200 //һ����������ʱ��

#define GPIO_CHK_PEOD	100 //IO�ڼ������ ms

#define FACTORY_GPIO_CHK_V_TIME 4000 //�ָ�����������ⰴ��ʱ��ms
#define SWITCH_GPIO_CHK_V_TIME 200
#define STRONG_GPIO_CHK_V_TIME 200
#define UNPLUG_GPIO_CHK_V_TIME 200

/********************************************
 *���ܣ�������ģ�� ��ʼ��
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/
static T_VOID Beep_Init(T_VOID)
{
	GPIO_InitTypeDef stGPIOConfig;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//PA13 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	stGPIOConfig.GPIO_Pin	= GPIO_Pin_1;
	stGPIOConfig.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIOConfig.GPIO_Mode	= GPIO_Mode_OUT;
	stGPIOConfig.GPIO_PuPd = GPIO_PuPd_NOPULL;
	stGPIOConfig.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &stGPIOConfig);
	Beep_GPIOL();
	
	//Timer 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = (1000000/BEEP_FREQ)/2;
	TIM_TimeBaseStructure.TIM_Prescaler = 47; //��Ƶ��1MHz	; ����(48000000/1000000)-1
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(BEEP_TIME_ID, &TIM_TimeBaseStructure);
	TIM_ITConfig(BEEP_TIME_ID, TIM_IT_Update, ENABLE);
	TIM_Cmd(BEEP_TIME_ID, DISABLE);
	
}
/********************************************
 *���ܣ�������IO�ڷ�ת
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/
T_VOID Beep_IORevesal(T_VOID)
{
	static T_U8 u8BeepIoS = 0;
	u8BeepIoS = 1 - u8BeepIoS;
	if(1 == u8BeepIoS)	
	{
		Beep_GPIOH();
	}else
	{
		Beep_GPIOL();
	}
	TIM_ClearITPendingBit(BEEP_TIME_ID, TIM_IT_Update);
}
/********************************************
 *���ܣ�timer3 ��ʱ�жϴ�����
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/
void Beep_TimerIRQHandler(void)
{
	Beep_IORevesal();
}

/********************************************
 *���ܣ�������ֹͣ��
 *���룺����
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/
static T_VOID Beep_StopRing(T_VOID *pvData)
{
	Beep_TimerStop();
	Beep_GPIOL();
}
/********************************************
 *���ܣ��ȴ�����
 *���룺����
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/
static T_VOID Beep_WaitRing(T_VOID *pvData)
{
	TimerHandle stTimeNode;
	T_U32 stTick;
	TimeTick stTimeTick;
	
	MDL_TIME_GetTimeTick(&stTimeTick);
	stTick = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec;
		
	stTimeNode.fTimerHandle = Beep_StopRing;
	stTimeNode.pvData = T_NULL;
	stTimeNode.s32MSec = stTick + BEEP_OPEN_DELAY_TIME;
	MDL_DRVTIMER_AddTimer(&stTimeNode);
	Beep_TimerStart();
}
/********************************************
 *���ܣ���������ʼ��
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/
static T_VOID Beep_StartRing(T_VOID)
{
	TimerHandle stTimeNode;
	T_U32 stTick;
	TimeTick stTimeTick;
	
	MDL_TIME_GetTimeTick(&stTimeTick);
	stTick = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec;
		
	stTimeNode.fTimerHandle = Beep_WaitRing;
	stTimeNode.pvData = T_NULL;
	stTimeNode.s32MSec = stTick + 10; //��ʱһ��ʱ�俪����������������������ܵ�Ӱ��
	MDL_DRVTIMER_AddTimer(&stTimeNode);
}
/********************************************
 *���ܣ���ͨGPIO ����
 *���룺��
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/
static T_VOID GPIO_Setup(T_VOID)
{
	GPIO_InitTypeDef stGpioDef;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	stGpioDef.GPIO_Mode = GPIO_Mode_OUT;
    stGpioDef.GPIO_OType = GPIO_OType_PP;   
    stGpioDef.GPIO_Pin = POWER4G_GPIO_PIN;
    stGpioDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
    stGpioDef.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(POWER4G_GPIO_PORT, &stGpioDef);

	stGpioDef.GPIO_Mode = GPIO_Mode_OUT;
    stGpioDef.GPIO_OType = GPIO_OType_PP;   
    stGpioDef.GPIO_Pin = RESET4G_GPIO_PIN;
    stGpioDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
    stGpioDef.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RESET4G_GPIO_PORT, &stGpioDef);

	stGpioDef.GPIO_Mode = GPIO_Mode_OUT;
    stGpioDef.GPIO_OType = GPIO_OType_PP;   
    stGpioDef.GPIO_Pin = LED_GPIO_PIN;
    stGpioDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
    stGpioDef.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GPIO_PORT, &stGpioDef);

	stGpioDef.GPIO_Mode = GPIO_Mode_IN;
    stGpioDef.GPIO_OType = GPIO_OType_OD;   
    stGpioDef.GPIO_Pin = FQ_GPIO_PIN;
    stGpioDef.GPIO_PuPd = GPIO_PuPd_UP;
    stGpioDef.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FQ_GPIO_PORT, &stGpioDef);

	stGpioDef.GPIO_Mode = GPIO_Mode_IN;
    stGpioDef.GPIO_OType = GPIO_OType_OD;   
    stGpioDef.GPIO_Pin = STRONG_GPIO_PIN;
    stGpioDef.GPIO_PuPd = GPIO_PuPd_UP;
    stGpioDef.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(STRONG_GPIO_PORT, &stGpioDef);

	stGpioDef.GPIO_Mode = GPIO_Mode_OUT;
    stGpioDef.GPIO_OType = GPIO_OType_PP;   
    stGpioDef.GPIO_Pin = RESETIPC_GPIO_PIN;
    stGpioDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
    stGpioDef.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RESETIPC_GPIO_PORT, &stGpioDef);
	
    stGpioDef.GPIO_Mode = GPIO_Mode_IN;
    stGpioDef.GPIO_OType = GPIO_OType_OD;   
    stGpioDef.GPIO_Pin =  RESET_GPIO_PIN;
    stGpioDef.GPIO_PuPd = GPIO_PuPd_UP;  //����
    stGpioDef.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RESET_GPIO_PORT, &stGpioDef);

	stGpioDef.GPIO_Mode = GPIO_Mode_OUT;
    stGpioDef.GPIO_OType = GPIO_OType_PP;   
    stGpioDef.GPIO_Pin = RELAY_GPIO_PIN;
    stGpioDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
    stGpioDef.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RELAY_GPIO_PORT, &stGpioDef);
	
	SET_RELAY_GPIO(0);
	SET_LED_GPIO(0);
	SET_RESETIPC_GPIO(1);
	SET_4GRESET_GPIO(0);
	SET_4GPOWER_GPIO(0);
}
/********************************************
 *���ܣ��ָ�������������
 *���룺����
 *�������
 *��������
 *���أ���
 ע��:
*********************************************/
static T_VOID CheckGpioHandler(T_VOID *pvData)
{
	static T_U16 u16GpioLTime_RESET = 0;
	static T_U16 u16GpioLTime_SWITCH = 0;
	static T_U16 u16GpioLTime_STRONG = 0;
	static T_U16 u16GpioLTime_UNPLUG = 0;
	
	TimerHandle stTimerHandle;
	TimeTick stTimeTick;

	/* ������� */
	if(0 == GPIO_ReadInputDataBit(RESET_GPIO_PORT, RESET_GPIO_PIN))
	{
		u16GpioLTime_RESET++;
		
		if(u16GpioLTime_RESET > (FACTORY_GPIO_CHK_V_TIME / GPIO_CHK_PEOD))
		{
			u16GpioLTime_RESET = 0;
			u8ResetPopFlag = 1;
			g_stGpioCrtl.eType |= GPIO_RECVY;
		}
	}else
	{
		u16GpioLTime_RESET = 0;
	}

	/* ���ż�� */
	if(0 == GPIO_ReadInputDataBit(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN))
	{
		u16GpioLTime_SWITCH++;
		
		if(u16GpioLTime_SWITCH > (SWITCH_GPIO_CHK_V_TIME / GPIO_CHK_PEOD))
		{
			u16GpioLTime_SWITCH = 0;
			u8ResetPopFlag = 1;
			g_stGpioCrtl.eType |= GPIO_SWITCH;
		}
	}else
	{
		u16GpioLTime_SWITCH = 0;
	}

	/* ������� */
	if(0 == GPIO_ReadInputDataBit(STRONG_GPIO_PORT, STRONG_GPIO_PIN))
	{
		u16GpioLTime_STRONG++;
		
		if(u16GpioLTime_STRONG > (STRONG_GPIO_CHK_V_TIME / GPIO_CHK_PEOD))
		{
			u16GpioLTime_STRONG = 0;
			u8ResetPopFlag = 1;
			g_stGpioCrtl.eType |= GPIO_STRONG;
		}
	}

	/* ������ */
	if(0 == GPIO_ReadInputDataBit(FQ_GPIO_PORT, FQ_GPIO_PIN))
	{
		u16GpioLTime_UNPLUG++;
		
		if(u16GpioLTime_UNPLUG > (UNPLUG_GPIO_CHK_V_TIME / GPIO_CHK_PEOD))
		{
			u16GpioLTime_UNPLUG = 0;
			u8ResetPopFlag = 1;
			g_stGpioCrtl.eType |= GPIO_UNPLUG;
		}
	}else
	{
		u16GpioLTime_UNPLUG = 0;
	}

	/* ������ӱ���ʱ���� */
	MDL_TIME_GetTimeTick(&stTimeTick);
	stTimerHandle.fTimerHandle = CheckGpioHandler;
	stTimerHandle.pvData = T_NULL;
	stTimerHandle.s32MSec = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec + GPIO_CHK_PEOD;
	MDL_DRVTIMER_AddTimer(&stTimerHandle);
}

/********************************************
 *���ܣ�GPIO �򿪺���
 *���룺��
 *�������
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
 ע��:
*********************************************/
static T_S32 _GPIO_Open(T_VOID)
{
	TimerHandle stTimerHandle;
	TimeTick stTimeTick;
	
	GPIO_Setup();
	Beep_Init();

	/* ��Ӹ�IO�ڼ�ⶨʱ�� */
	MDL_TIME_GetTimeTick(&stTimeTick);
	stTimerHandle.fTimerHandle = CheckGpioHandler;
	stTimerHandle.pvData = T_NULL;
	stTimerHandle.s32MSec = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec + GPIO_CHK_PEOD;
	MDL_DRVTIMER_AddTimer(&stTimerHandle);

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
static T_S32 _GPIO_Read(T_S8 *ps8DataBuf, T_S32 s32BufLen)
{
	T_S32 s32DataLen = sizeof(GpioCrtl);
	
	if(s32BufLen < s32DataLen || T_NULL == ps8DataBuf)
	{	
		return RET_FAILED;
	}
	
	memcpy(ps8DataBuf, &g_stGpioCrtl, s32DataLen);

	u8ResetPopFlag = 0;
	g_stGpioCrtl.eType = (GpioType)0;
	
	return s32DataLen;
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
static T_S32 _GPIO_Pop(T_VOID)
{
	if(!u8ResetPopFlag)
	{
		return RET_FAILED;
	}
	
	return RET_SUCCESS;
}

/********************************************
 *���ܣ����ƽӿ�
 *���룺s32Cmd :��������
 			1��GPIO_SET
 			2��GPIO_GET
 
 *��������
 *���أ��ɹ�:RET_SUCCESS 
 		ʧ��:RET_FAILED 
*********************************************/
static T_S32 _GPIO_Ioctl(T_S32 s32Cmd, T_VOID *pvData)
{
	GpioCrtl *pstGpioCrtl = T_NULL;

	if(T_NULL == pvData)
	{
		return RET_FAILED;
	}
	
	pstGpioCrtl = (GpioCrtl *)pvData;
	
	switch(s32Cmd)
	{
		case GPIO_SET:
		{
			if(GPIO_BEEP_OPEN & pstGpioCrtl->eType)
			{
				Beep_StartRing();
			}else if(GPIO_RELAY & pstGpioCrtl->eType)
			{
				if(0 != pstGpioCrtl->u8Val)
				{
					SET_RELAY_GPIO(1);
				}else
				{
					SET_RELAY_GPIO(0);
				}
			}else if(GPIO_LED & pstGpioCrtl->eType)
			{
				if(0 != pstGpioCrtl->u8Val)
				{
					SET_LED_GPIO(1);
				}else
				{
					SET_LED_GPIO(0);
				}	
			}else if(GPIO_IPC5V & pstGpioCrtl->eType)
			{
				if(0 != pstGpioCrtl->u8Val)
				{
					SET_RESETIPC_GPIO(1);
				}else
				{
					SET_RESETIPC_GPIO(0);
				}	
			}else if(GPIO_RST4G & pstGpioCrtl->eType)
			{
				if(0 != pstGpioCrtl->u8Val)
				{
					SET_4GRESET_GPIO(1);
				}else
				{
					SET_4GRESET_GPIO(0);
				}	
			}else if(GPIO_4GPOWER & pstGpioCrtl->eType)
			{
				if(0 != pstGpioCrtl->u8Val)
				{
					SET_4GPOWER_GPIO(1);
				}else
				{
					SET_4GPOWER_GPIO(0);
				}	
			}
		}
		break;

		case GPIO_GET:
			memcpy(pstGpioCrtl, &g_stGpioCrtl, sizeof(GpioCrtl));
		break;

		default:
			return RET_FAILED;
	}

	return RET_SUCCESS;
}

/********************************************
 *���ܣ���ʼ��GPIO File Module���ҽ���
                   ������ļ��б�
 *���룺��
 *�������.
 *��������
 *���أ���
 ע��:��
*********************************************/
T_S32 DRIVER_GPIO_Init(T_VOID)
{
    FileOperation stGpioFileOperation;

	memset(stGpioFileOperation.s8ModelName, 0, MODEL_NAME_LEN);
    strcpy(stGpioFileOperation.s8ModelName, "Gpio");
    stGpioFileOperation.FileIoctl = _GPIO_Ioctl;
    stGpioFileOperation.FileOpen = _GPIO_Open;
    stGpioFileOperation.FileRead = _GPIO_Read;
    stGpioFileOperation.FileWrite = T_NULL;
    stGpioFileOperation.FilePop = _GPIO_Pop;
    stGpioFileOperation.FileClose = T_NULL;
    stGpioFileOperation.FileRelease = T_NULL;
    
    return MDL_FILE_AddModel(&stGpioFileOperation);
}
#endif
