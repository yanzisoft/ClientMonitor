#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"
#ifdef __cplusplus
	extern "C" {
#endif

void USART1_SystemInit_RCC_Configuration(void);
void USART2_SystemInit_RCC_Configuration(void);	
void USART_Configuration(USART_TypeDef * USARTx,uint32_t BaudRate);		
#ifdef __cplusplus
	}
#endif	
#endif
