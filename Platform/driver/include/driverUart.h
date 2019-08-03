/*********************************************
* @文件: driverUart.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2019-7-14
* @概要: 实现串口驱动接口
*********************************************/
#ifndef _USER_UART_H__
#define _USER_UART_H__

#include "stm32f10x_usart.h"

T_S32 DRIVER_OpenUart(USART_TypeDef* USARTx, T_U32 u32BaudRate, T_U16 u16StopBit);
T_S32 DRIVER_IsUartData(USART_TypeDef* USARTx);
T_S32 DRIVER_UartWrite(USART_TypeDef* USARTx, T_S8 *ps8ReadBuf, T_U8 u8WriteLen);
T_S32 DRIVER_UartRead(USART_TypeDef* USARTx, T_S8 *ps8ReadBuf, T_U16 u16ReadLen);
#endif


