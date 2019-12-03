#include "usart.h"



void USART1_SystemInit_RCC_Configuration()
{
SystemInit();
//设置时钟
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1,  ENABLE);
}

void USART2_SystemInit_RCC_Configuration()
{
SystemInit();
//设置时钟
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO,  ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,  ENABLE);
}

void USART_Configuration(USART_TypeDef * USARTx,uint32_t BaudRate)
{
GPIO_InitTypeDef GPIO_InitStruct;
USART_InitTypeDef USART_InitStruct;

uint16_t GPIO_Pin_Send=GPIO_Pin_2;
uint16_t GPIO_Pin_Receive=GPIO_Pin_3;

if(USARTx==USART1){
	GPIO_Pin_Send=GPIO_Pin_9;
	GPIO_Pin_Receive=GPIO_Pin_10;
}

//PA2 发送引脚
GPIO_InitStruct.GPIO_Pin=GPIO_Pin_Send;
GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
GPIO_Init(GPIOA,&GPIO_InitStruct);
//PA3 没有实际使用 接收引脚
GPIO_InitStruct.GPIO_Pin=GPIO_Pin_Receive;
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
GPIO_Init(GPIOA,&GPIO_InitStruct);

USART_InitStruct.USART_BaudRate=BaudRate;
USART_InitStruct.USART_WordLength=USART_WordLength_8b;
USART_InitStruct.USART_StopBits=USART_StopBits_1;
USART_InitStruct.USART_Parity=USART_Parity_No;
USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
USART_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;

USART_Init(USARTx, &USART_InitStruct);
USART_Cmd(USARTx,ENABLE);
}
