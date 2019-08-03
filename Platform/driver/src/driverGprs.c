/*********************************************
* @文件: driverGprs.c
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-7-14
* @概要: 提供GPRS驱动接口
*********************************************/
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "driverFile.h"
#include "driverTimer.h"
#include "driverTime.h"
#include "ioctrl.h"
#include "driverUart.h"

//GPS AT 指令模块
#define GPS_AT_TX_PORT   		GPIOE     
#define GPS_AT_TX_PIN    		GPIO_Pin_4
#define GPS_AT_RX_PORT   		GPIOE      //RX
#define GPS_AT_RX_PIN    		GPIO_Pin_3

#define GPS_DATA_RX_PORT        GPIOC
#define GPS_DATA_RX_PIN         GPIO_Pin_2

#define GPS_DATA_TX_PORT        GPIOC
#define GPS_DATA_TX_PIN         GPIO_Pin_3

#define GPS_RESET_PORT          GPIOB     
#define GPS_RESET_PIN           GPIO_Pin_4

#define GPS_START_PORT          GPIOG
#define GPS_START_PIN           GPIO_Pin_5

#define GPS_INT_PORT            GPIOD
#define GPS_INT_PIN             GPIO_Pin_0

#define GPS_SLEEP_PORT          GPIOB
#define GPS_SLEEP_PIN           GPIO_Pin_1

#define GPS_HALT_PORT           GPIOC
#define GPS_HALT_PIN            GPIO_Pin_7

//串口回显设置
#define GPS_ATE_ON          "ATE1\r\n"   //关闭指令回显
#define GPS_ATE_OFF         "ATE0\r\n"   //打开指令回显
//A9G模块测试
#define GPS_TX_TEST         "AT\r\n"
#define GPS_RX_ERR          "ERROR" 
//GPS 设置
#define GPS_ENALBE          "AT+GPS=1\r\n"
#define GPS_DISENALBE       "AT+GPS=0\r\n"
#define GPS_READ_RATE       "AT+GPSRD=1\r\n"
//短息设置
#define GPS_SET_MSG_MODE    "AT+CMGF=1\r\n"   //设置短信格式 0:PDU模式  1:文本格式
#define GPS_SEND_MSG        "AT+CMGS=\"15915342743\"\r\n"  //发送短息
//SIM安装及是否可以上网  上网设置
#define GPS_TEST_SIM        "AT+CCID\r\n"   //测试SIM卡是否有装好
#define GPS_SIM_CREG        "AT+CREG?\r\n"  //查询网络状态 
#define GPS_CGATT           "AT+CGATT=1\r\n"  //附着网络，如果需要上网，这条指令是必选的
#define GPS_CGDCONT         "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n"  //设置PDP参数
#define GPS_CGACT           "AT+CGACT=1,1\r\n"  ////激活PDP，正确激活以后就可以上网了

//TCP设置
//#define GPS_CONNET_SERVER   "AT+CIPSTART=\"TCP\",\"baidu.com\",80\r\n"   
#define GPS_CONNET_SERVER   "AT+CIPSTART=\"TCP\",\"47.97.154.253\",4430\r\n"  
//#define GPS_CONNET_SERVER   "AT+CIPSTART=\"TCP\",\"122.114.122.174\",39039\r\n"
//查询TCP的连接状态
#define GPS_TCP_STATUS      "AT+CIPSTATUS?\r\n"
#define GPS_TCP_SEND        "at+cipsend="

//透传参数设置
#define GPS_DTU_PARA_REQ    "AT+CIPTCFG?\r\n"			//透传参数查询          
#define GPS_TCP_SET1        "AT+CIPTCFG=0,3\r\n"     //失败重传次数为3次
#define GPS_TCP_SET2        "AT+CIPTCFG=1,200\r\n"   //发送失败延时发延时200ms再发
#define GPS_TCP_SET3        "AT+CIPTCFG=2,10\r\n"    //触发数据包为10byte
#define GPS_TCP_SET4        "AT+CIPTCFG=3,1000\r\n"  //触发发送延时时长
#define GPS_DATA_MODE_CMD   "AT+CIPTMODE=1\r\n"     //设置GPS数据交互为透传模式
#define GPS_CMD_MODE        "+++"           		//退出命令模式  退出命令不能有换行符号
//LED灯状态操作
#define GPS_LED_FlASH      "AT+GPNT=0\r\n"     //设置为闪灯状态
#define GPS_LED_NOTICE     "AT+GPNT=1\r\n"     //设置为电平状态
#define GPS_LED_STATUS     "AT+GPNT=?\r\n"     //查询LED灯状态

//指令进入睡眠模式
#define GPS_SLEEE_EN       "AT+SLEEP=1\r\n"
#define GPS_GPSLP_EN       "AT+GPSLP=2\r\n"

//设置最小功能
#define GPS_CFUN           "AT+CFUN=1,0\r\n"


#define GPS_RESET_EN()		
#define GPS_RESET_DI()		

#define GPS_START_L()       
#define GPS_START_H()       

#define GPS_SLEEP_DI()         //唤醒
#define GPS_SLEEP_EN()            //睡眠

#define GPS_INT_L()            //唤醒

#define GPS_HALT_EN()         
#define GPS_HALT_DI()       

#define GPS_UART_RX_CACHE_LEN   320  //最大330
#define GPS_TASK_PEOD           30
#define GPS_CHK_RX_FULL_PEOD	50 
#define GPS_READ_BUF_LEN        100
#define GPS_TRACE_PEOD          10000

typedef enum
{
	GPS_INIT_CHECK = 0,
	GPS_NONE_STEP,
	GPS_RESET_STEP,
	GPS_NOMAL_STEP,
	GPS_NOMAL_WAIT_STEP,
	GPS_TEST_STEP,
    GPS_ATE0_STEP,  //设置关掉回显功能
    GPS_ATE1_STEP,  //开启回显
    GPS_DTU_REQ_STEP,
	GPS_SIM_STEP, 
	GPS_SIM_REGSTER_STEP,
	GPS_DATA_RATE_STEP,
	GPS_ON_STEP,
	GPS_OFF_STEP,
	GPS_NEMA_STEP,
    GPS_LOW_POWER_STEP,
	GPS_CONNECT_SERVER_STEP,
    GPS_CGATT_STEP,
    GPS_CGDCONT_STEP,
    GPS_CGACT_STEP,
    
    GPS_SET_PRAR1_STEP,
    GPS_SET_PRAR2_STEP,
    GPS_SET_PRAR3_STEP,
    GPS_SET_PRAR4_STEP,
	GPS_DATA_MODE_STEP, //进入TCP 透传模式
	GPS_CMD_MODE_STEP,  //串口命令模式
    //TCP 发送数据
    GPS_TCP_SEND_STEP,
	//短信设置
	GPS_MSG_MODE_STEP, //设置短信模式
    GPS_SEND_MSG_STEP, //发送短信
    //设置LED灯状态
    GPS_SET_LED_STEP,
    //
    GPS_SLEEP_STEP,
    GPS_GPL_STEP,

    GPS_CFUN_STEP,
    
}GpsInitStep;   //GPS 初始化步骤

typedef struct
{
	T_U8 u8D[GPS_UART_RX_CACHE_LEN]; 
	T_U16 u16Cnt; 
}GpsUartRevc; 

typedef struct
{
	T_U8 u8InitStep;
	T_U8 u8Return;
	T_U8 u8WaitCnt;
    T_U8 u8RevMode;
    T_U8 u8SetStatus;
  //  T_SIMStatus enSIMStatus; //SIM 卡状态 
}GpsClt; 


//static T_GpsTcpCtlData stTcpData;
static GpsClt stCtl;
static GpsUartRevc stRx; 
static T_U8 u8GpsReadPopFlag = 0;
static T_U8 u8GpsReadBuf[GPS_READ_BUF_LEN]; 
static T_U8 u8GpsReadLen = 0; 

static T_VOID Gps_Init(T_VOID)
{
	DRIVER_OpenUart(USART2, 115200, 1);	
}

/********************************************
 *功能：GPS串口发送函数
 *输入：发送的数据，数据长度
 *输出：无
 *条件：无
 *返回：无
 *注意: 无
********************************************/
static T_VOID Gps_UartSendBuf(T_U8 *u8Buf, T_U8 u8Len)
{
	DRIVER_UartWrite(USART2, (char *)u8Buf, u8Len);
}
/********************************************
 *功能：GPS模块自检函数
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 *注意: 无
********************************************/
static T_S32 Gps_SelfChk(T_U8 *pu8GprsBuf)
{
	T_U32 stOldTick,stNewTick;
	TimeTick stTimeTick;
	T_U8 u8GprsBuf[64]= {0};
	
	//GPS_RESET_DI();
	MDL_TIME_GetTimeTick(&stTimeTick);
	stOldTick = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec;
	stNewTick = stOldTick;
    
    if(T_NULL != strstr((char const *)pu8GprsBuf, "READY"))   //等待A9G模块内部初始完
    {
        printf("ready\n");
        Gps_UartSendBuf((T_U8 *)GPS_TX_TEST, sizeof(GPS_TX_TEST)-1);
		MDL_TIME_GetTimeTick(&stTimeTick);
		stOldTick = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec;
		stNewTick = stOldTick;
		while((stNewTick - stOldTick) < 500) 
		{
			DRIVER_IsUartData(USART2);
			DRIVER_UartRead(USART2, (char *)u8GprsBuf, sizeof(u8GprsBuf));
	        if(T_NULL != strstr((char const *)u8GprsBuf, "OK"))
	        {
	            return RET_SUCCESS;
	        }
	        
	        MDL_TIME_GetTimeTick(&stTimeTick);
			stNewTick = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec;
		}
       
        // SIM卡未安装好或者卡未注册不能使用
        if(T_NULL != strstr((char const *)pu8GprsBuf, "NO SIM CARD"))
        {
            printf("SIM Bad\n");
            return RET_FAILED;

        }
        
		MDL_TIME_GetTimeTick(&stTimeTick);
		stNewTick = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec;
	}
    
	

	return RET_FAILED;
}

/********************************************
 *功能：设置Gprs接受模式
 *输入：u8Mode
 *输出：无
 *条件：无
 *返回：无
 *注意:
********************************************/
static T_VOID  Gps_SetRevMode(T_U8 u8Mode)
{
    stCtl.u8RevMode = u8Mode;
}

/********************************************
 *功能：GPS串口初始化步骤设置
 *输入：GpsInitStep enStep
 *输出：无
 *条件：无
 *返回：无
 *注意:
********************************************/
static T_VOID Gps_SetMode(GpsInitStep enStep)
{
	stCtl.u8InitStep = enStep;
}


/********************************************
 *功能：GPS 读取定位数据任务
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 *注意:
*********************************************/
static T_VOID Gps_TaskTraceData(T_VOID *pvData)
{
    TimerHandle stTimeNode;
	T_U32 stTick;
	TimeTick stTimeTick;

    MDL_TIME_GetTimeTick(&stTimeTick);
	stTick = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec;

    print_debug(" read trace data\n");
	//GPS_HALT_EN();		
	stTimeNode.fTimerHandle = Gps_TaskTraceData;
	stTimeNode.pvData = T_NULL;
	stTimeNode.s32MSec = stTick + GPS_TRACE_PEOD;
	MDL_DRVTIMER_AddTimer(&stTimeNode);
}

/********************************************
 *功能：GPS 任务轮询函数
 *输入：无
 *输出：无
 *条件：无
 *返回：无
 *注意:
*********************************************/
static T_VOID Gps_TaskHandler(T_VOID *pvData)
{
    T_S8 *ps8Pos = T_NULL;
    T_U8 i=0;
    static T_U8 u8Return =0, u8RetryConnectFlag = 0;

	DRIVER_IsUartData(USART2);
	
    if(DRIVER_UartRead(USART2, (char *)stRx.u8D, sizeof(stRx.u8D))>0)
	{	
		if(stCtl.u8InitStep == GPS_INIT_CHECK)
		{
			if(RET_SUCCESS == Gps_SelfChk(stRx.u8D))
			{
				Gps_SetRevMode(GPS_USER_MODE);
		    	Gps_SetMode(GPS_ATE1_STEP); //GPS_MSG_MODE_STEP ///GPS_SIM_STEP  ////GPS_ATE0_STEP //GPS_TEST_STEP
				
			}
	    }
    }
	    
	switch(stCtl.u8InitStep)
    {
        case GPS_NONE_STEP:
		  break;

		case GPS_TEST_STEP:  //接下来的步骤要往下走要根据有没有收到返回来判断
		{
			//if(1 == stCtl.u8Return)
			{
				stCtl.u8Return = 0;
				Gps_UartSendBuf((T_U8 *)GPS_TX_TEST, sizeof(GPS_TX_TEST)-1);
				
				stCtl.u8InitStep = GPS_CGATT_STEP; 
			}
			
		}break;

        case GPS_ATE0_STEP:  //关闭AT指令回显功能
        {
        	stCtl.u8Return = 0;
            Gps_UartSendBuf((T_U8 *)GPS_ATE_OFF, sizeof(GPS_ATE_OFF)-1);
            
            stCtl.u8InitStep = GPS_SIM_STEP;
           
        }break;

        case GPS_ATE1_STEP: //开启AT指令回显功能
        {
        	stCtl.u8Return = 0;
            Gps_UartSendBuf((T_U8 *)GPS_ATE_ON, sizeof(GPS_ATE_ON)-1);
			
            stCtl.u8InitStep = GPS_SIM_STEP;
            //stCtl.u8InitStep = GPS_DTU_REQ_STEP;
            //stCtl.u8InitStep = GPS_SET_PRAR1_STEP;
        }break;

        case GPS_DTU_REQ_STEP: //测试可以查看透传参数
        {
        	if(stCtl.u8Return == 1)
        	{
				stCtl.u8Return =0;
	            Gps_UartSendBuf((T_U8 *)GPS_DTU_PARA_REQ, sizeof(GPS_DTU_PARA_REQ)-1);
	            stCtl.u8InitStep = GPS_SIM_STEP;
            }
            
        }break;

        case GPS_SET_PRAR1_STEP:
        {
			if(stCtl.u8Return == 1)
        	{
				stCtl.u8Return =0;
	            Gps_UartSendBuf((T_U8 *)GPS_TCP_SET1, sizeof(GPS_TCP_SET1)-1);
	            stCtl.u8InitStep = GPS_SET_PRAR2_STEP;
            }
        }break;

        case GPS_SET_PRAR2_STEP:
        {
			if(stCtl.u8Return == 1)
        	{
				stCtl.u8Return =0;
	            Gps_UartSendBuf((T_U8 *)GPS_TCP_SET2, sizeof(GPS_TCP_SET2)-1);
	            stCtl.u8InitStep = GPS_SET_PRAR3_STEP;
            }
        }break;

        case GPS_SET_PRAR3_STEP:
        {
			if(stCtl.u8Return == 1)
        	{
				stCtl.u8Return =0;
	            Gps_UartSendBuf((T_U8 *)GPS_TCP_SET3, sizeof(GPS_TCP_SET3)-1);
	            stCtl.u8InitStep = GPS_SET_PRAR4_STEP;
            }
        }break;

        case GPS_SET_PRAR4_STEP:
        {
			if(stCtl.u8Return == 1)
        	{
				stCtl.u8Return =0;
	            Gps_UartSendBuf((T_U8 *)GPS_TCP_SET4, sizeof(GPS_TCP_SET4)-1);
	            stCtl.u8InitStep = GPS_SIM_STEP;
            }
        }break;

        case GPS_SIM_STEP: //检测SIM卡是否安装好
        {
            if(1 == stCtl.u8Return)
            {
        		stCtl.u8Return = 0;
                Gps_UartSendBuf((T_U8 *)GPS_TEST_SIM, sizeof(GPS_TEST_SIM)-1);
                
                stCtl.u8InitStep = GPS_SIM_REGSTER_STEP; 
                //stCtl.u8InitStep = GPS_ON_STEP;
            }
   
        }break;

        case GPS_SIM_REGSTER_STEP:
        {
            if(1== stCtl.u8Return)
            {
            	stCtl.u8Return = 0;
                Gps_UartSendBuf((T_U8 *)GPS_SIM_CREG, sizeof(GPS_SIM_CREG)-1);
               
                stCtl.u8InitStep = GPS_CGATT_STEP; 
            }
        }break;
        
        case GPS_CGATT_STEP:
        {
            if(1== stCtl.u8Return)
            {
            	stCtl.u8Return = 0;
                Gps_UartSendBuf((T_U8 *)GPS_CGATT, sizeof(GPS_CGATT)-1);
                
                stCtl.u8InitStep = GPS_CGDCONT_STEP; 
            }
        }break;
        
        case GPS_CGDCONT_STEP:
        {
            if(1== stCtl.u8Return)
            {
            	stCtl.u8Return = 0;
                Gps_UartSendBuf((T_U8 *)GPS_CGDCONT, sizeof(GPS_CGDCONT)-1);
                
                stCtl.u8InitStep = GPS_CGACT_STEP; 
            }

        }break;
        
        case GPS_CGACT_STEP:
        {
            if(1== stCtl.u8Return)
            {
            	stCtl.u8Return = 0;
                Gps_UartSendBuf((T_U8 *)GPS_CGACT, sizeof(GPS_CGACT)-1);
                
                stCtl.u8InitStep = GPS_CONNECT_SERVER_STEP; 
            }

        }break;
      
        case GPS_CONNECT_SERVER_STEP:
        {
            if(1== stCtl.u8Return)
            {
            	stCtl.u8Return = 0;
            	u8Return = 0;
                Gps_UartSendBuf((T_U8 *)GPS_CONNET_SERVER, sizeof(GPS_CONNET_SERVER)-1);
                
                if(u8RetryConnectFlag == 0)
                {
                	stCtl.u8InitStep = GPS_ON_STEP; 
                }else
                {
                	stCtl.u8InitStep = GPS_DATA_MODE_STEP;
                }	
            }
        }break;
        
        /*************************************************/
        case GPS_ON_STEP: //开启GPS
        {
            if(1== u8Return)
            {
            	stCtl.u8Return = 0;
                Gps_UartSendBuf((T_U8 *)GPS_ENALBE, sizeof(GPS_ENALBE)-1);
                stCtl.u8InitStep = GPS_NEMA_STEP;
            }
     
        }break;

        case GPS_OFF_STEP:
        {
			if(1== stCtl.u8Return || 1 == stCtl.u8SetStatus)
            {
            	stCtl.u8Return = 0;
                Gps_UartSendBuf((T_U8 *)GPS_DISENALBE, sizeof(GPS_DISENALBE)-1);
                stCtl.u8InitStep = GPS_DATA_MODE_STEP;
            }
        }break;

        case GPS_NEMA_STEP:    //GPS 上报速率
        {
            if(1== stCtl.u8Return)
            {
            	stCtl.u8Return = 0;
                Gps_UartSendBuf((T_U8 *)GPS_READ_RATE, sizeof(GPS_READ_RATE)-1);

                stCtl.u8InitStep = GPS_NONE_STEP;
                //stCtl.u8InitStep = GPS_DATA_MODE_STEP;
                //stCtl.u8InitStep = GPS_SLEEP_STEP; 
                //stCtl.u8InitStep = GPS_GPL_STEP;
            }
            
        }break;

        case GPS_DATA_MODE_STEP:   //设置为透传模式
        {
            //if(1 == u8Return || 1 == stCtl.u8SetStatus)
            if(1 == stCtl.u8Return)
            {
            	u8Return =0;
            	stCtl.u8Return = 0;
            	stCtl.u8SetStatus =0;
                Gps_UartSendBuf((T_U8 *)GPS_DATA_MODE_CMD, sizeof(GPS_DATA_MODE_CMD)-1);
                
                stCtl.u8InitStep = GPS_NONE_STEP; 
                Gps_SetRevMode(GPS_DATA_MODE);
            }
            
        }break;

        case GPS_GPL_STEP:
        {
            if(1 == stCtl.u8Return)
            {
            	stCtl.u8Return = 0;
                Gps_UartSendBuf((T_U8 *)GPS_GPSLP_EN, sizeof(GPS_GPSLP_EN)-1);
                
                stCtl.u8InitStep = GPS_SLEEP_STEP; 
            }

        }break;

        case GPS_SLEEP_STEP:
        {
            if(1 == stCtl.u8Return)
            {
            	stCtl.u8Return = 0;
                Gps_UartSendBuf((T_U8 *)GPS_SLEEE_EN, sizeof(GPS_SLEEE_EN)-1);
                
                stCtl.u8InitStep = GPS_NONE_STEP; 
                //stCtl.u8InitStep = GPS_DATA_MODE_STEP; 
                //GPS_SLEEP_EN();   //发完 AT + SLEEP =1 \n\r指令后拉低IO25脚进入低功耗模式  该模式下发串口指令无效
                GPS_SLEEP_DI();
                memset(stRx.u8D, 0, sizeof(stRx.u8D));
                stRx.u16Cnt = 0;
            }
        }break;
        
        /**********************************************************************/
        case GPS_TCP_SEND_STEP:   //非透传模式下发送指令  步骤和发送短信一样
        {
          //  T_U8 u8TcpBuf[128]={"AT+CIPSEND=0,5,\"qwert\""}; //第一个参数表示链路 默认为链路0
            
            //Gps_UartSendBuf(stTcpData.pu8DataBuf, stTcpData.u8Len);

           // free(stTcpData.pu8DataBuf);

           // stTcpData.pu8DataBuf = T_NULL;
           // stTcpData.u8Len = 0;
            
        }break;
        
        default:break;
    }

		
    if(GPS_USER_MODE == stCtl.u8RevMode)
    {     
        if(T_NULL != strstr((const char *)stRx.u8D, "OK"))
		{
			stCtl.u8Return = 1;
            
		}if(T_NULL != strstr((const char *)stRx.u8D, "CONNECT OK"))
		{
			u8Return = 1;
            
		}else if( T_NULL != strstr((const char *)stRx.u8D, "+GPSRD:$GNGGA"))  //接收到的GPS数据  只取GNGGA格式数据
		{
            ps8Pos = strstr((const char *)stRx.u8D, "\r\n");
            if(T_NULL != ps8Pos)
            {
                i=0;
                while((T_S8 *)(stRx.u8D+i+9)  <  ps8Pos)
                {
                    if(i <= GPS_READ_BUF_LEN)  //防止数组越界
                    {
                        u8GpsReadBuf[i] = stRx.u8D[i+9];
                    }
                    
                    i++;
                }
                
                u8GpsReadLen = i;
                u8GpsReadPopFlag = 1;
            }
           
		}else if(T_NULL != strstr((const char *)stRx.u8D, "ERROR")) //出错要重新
        {
        	MDL_TIME_Delay(2000);
            Gps_SetMode(GPS_ATE1_STEP);
            printf("Cmd mode Error\n");
        }else if(T_NULL != strstr((const char *)stRx.u8D, "NO SIM CARD"))
        {
           // stCtl.enSIMStatus = SIM_NO_INSTALL;
        }/*else if(T_NULL != strstr((const char *)stRx.u8D, "+CREG: 1"))
        {
           // stCtl.enSIMStatus = SIM_NET;
           printf("+CREG:1");
        }*/else if(T_NULL != strstr((const char *)stRx.u8D, "CONNECT FAIL"))  //连接服务器失败
        {
        	MDL_TIME_Delay(2000);
            Gps_SetMode(GPS_ATE1_STEP);   //COMMAND NO RESPONSE!
        }else if(T_NULL != strstr((const char *)stRx.u8D, "COMMAND NO RESPONSE!"))
        {
        	MDL_TIME_Delay(2000);
            Gps_SetMode(GPS_ATE1_STEP);
        }

    }else if(GPS_DATA_MODE == stCtl.u8RevMode) 
    {
    	if(T_NULL != strstr((const char *)stRx.u8D, "ERROR") || T_NULL != strstr((const char *)stRx.u8D, "CLOSED"))
    	{	
    		u8RetryConnectFlag = 1;
    		stCtl.u8Return = 0;
    		Gps_SetRevMode(GPS_USER_MODE);
    		Gps_SetMode(GPS_TEST_STEP);
    		MDL_TIME_Delay(3000); //和前一次发送的指令相隔两秒以后才能退出透传模式
			Gps_UartSendBuf((T_U8 *)GPS_CMD_MODE, sizeof(GPS_CMD_MODE)-1);
			printf("ERROR TCP send\n");
    	}else
    	{
	        memcpy(u8GpsReadBuf, stRx.u8D, stRx.u16Cnt);
			u8GpsReadLen = stRx.u16Cnt;
			u8GpsReadPopFlag = 1;
    	}
    	

    }
	            
	memset(stRx.u8D, 0, sizeof(stRx.u8D));
	stRx.u16Cnt = 0;
	
}




/********************************************
 *功能：打开函数
 *输入：无
 *输出：无
 *条件：无
 *返回：成功:RET_SUCCESS 
 		失败:RET_FAILED 
 *注意:
*********************************************/
static T_S32 _Gps_Open(T_VOID)
{
	TimerHandle stTimeNode;
	T_U32 stTick;
	TimeTick stTimeTick;
	
	Gps_Init();
	Gps_SetMode(GPS_INIT_CHECK);
	/*if(RET_SUCCESS != Gps_SelfChk())
	{
		return RET_FAILED;
	}*/

	//Gps_SetRevMode(GPS_USER_MODE);
    //Gps_SetMode(GPS_ATE1_STEP); //GPS_MSG_MODE_STEP ///GPS_SIM_STEP  ////GPS_ATE0_STEP //GPS_TEST_STEP
	MDL_TIME_GetTimeTick(&stTimeTick);
	stTick = stTimeTick.u32Sec*1000 + stTimeTick.s32MSec;
	stTimeNode.fTimerHandle = T_NULL;//Gps_TaskHandler;
	stTimeNode.pvData = T_NULL;
	stTimeNode.s32MSec = stTick + GPS_TASK_PEOD;
	MDL_DRVTIMER_AddTimer(&stTimeNode);
		

	return RET_SUCCESS;
}

/********************************************
 *功能：控制接口
 *输入：s32Cmd :命令类型

 *条件：无
 *返回：成功:RET_SUCCESS 
 		失败:RET_FAILED 
*********************************************/
static T_S32 _Gps_Ioctl(T_S32 s32Cmd, T_VOID *pvData)
{
	
	switch(s32Cmd)
    {
    	case GPS_GET_WORK_MODE:
    	{
    		ASSERT_EQUAL_RETVAL(pvData, T_NULL, RET_FAILED);
			*(T_U8 *)pvData =  stCtl.u8RevMode;
    	}break;

    	case GPS_SET_DATA_MODE:
    	{
			Gps_SetMode(GPS_OFF_STEP);
			stCtl.u8SetStatus = 1;
    	}break;

   		default:break;
    }
	
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
 注意: 转换的值被扩大1000倍
*********************************************/
static T_S32 _Gps_Read(T_S8 *ps8DataBuf, T_S32 s32BufLen)
{
	ASSERT_EQUAL_RETVAL(ps8DataBuf, T_NULL, RET_FAILED);
	ASSERT_EQUAL_RETVAL(s32BufLen, 0, RET_FAILED);
	
	if(1 == u8GpsReadPopFlag)
	{
		u8GpsReadPopFlag = 0;
	
		if(s32BufLen > u8GpsReadLen)
		{
			memcpy(ps8DataBuf, u8GpsReadBuf, u8GpsReadLen);
			
			return u8GpsReadLen;
		}else
		{
			memcpy(ps8DataBuf, u8GpsReadBuf, s32BufLen);
			return s32BufLen;
		}
	}
	
	return RET_FAILED;
}

static T_S32 _Gps_Write(T_S8 *ps8DataBuf, T_S32 s32BufLen)
{
	ASSERT_EQUAL_RETVAL(ps8DataBuf, T_NULL, RET_FAILED);
	ASSERT_GT_RETVAL(s32BufLen, 255, RET_FAILED);
	
	Gps_UartSendBuf((T_U8 *)ps8DataBuf, s32BufLen);
	
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
static T_S32 _Gps_Pop(T_VOID)
{
	Gps_TaskHandler(T_NULL);
	if(0 == u8GpsReadPopFlag)
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

static T_S32 _Gps_Close(T_VOID)
{
	GPS_RESET_EN();

	return RET_SUCCESS;
}
/************************************************
 *功能：将GPS驱动模块添加到
 			驱动列表中。
 *输入：无
 *输出：无
 *条件：无
 *返回：成功0失败-1
 注意:
*************************************************/
T_S32 DRIVER_Gps_Init(T_VOID)
{
	FileOperation stOperation;

	stOperation.FileOpen = _Gps_Open;
	stOperation.FileRead = _Gps_Read;
	stOperation.FileWrite = _Gps_Write;
	stOperation.FileClose = _Gps_Close;
	stOperation.FileIoctl = _Gps_Ioctl;
	stOperation.FilePop = _Gps_Pop;
	stOperation.FileRelease = T_NULL;
	memset(stOperation.s8ModelName, 0, MODEL_NAME_LEN);
	strcpy(stOperation.s8ModelName, "GPS");
	
	return MDL_FILE_AddModel((FileOperation*)&stOperation);
}











