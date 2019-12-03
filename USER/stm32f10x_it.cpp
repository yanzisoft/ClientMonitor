/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "delay.h"
#include "PacketHelper.h"
#include "usartsenddata.h"

extern uint16_t GPIO_Pin_LED_USART_IT_TXE;//main.cpp
extern uint16_t GPIO_Pin_LED_USART_IT_TC;//main.cpp
extern uint16_t GPIO_Pin_LED_USART_IT_RXNE;//main.cpp
extern uint16_t GPIO_Pin_LED_USART_IT_IDLE;//main.cpp

USART_TypeDef* USART_Receive=USART1;
USART_TypeDef* USART_Send=USART2;

uint8_t ReceviceBuffer[PacketHelper::PacketLength];
uint8_t ReceviceBytes=0;

char* pSendBuffer=0;
uint8_t	SendLength=0;

bool IsGetFirstBufferAddress;
char* FirstBufferAddress;
bool IsSending=false;

extern GPIO_TypeDef * GPIO_Alarm;//main.cpp
extern GPIO_TypeDef * GPIO_LED;//main.cpp
//extern GPIO_TypeDef * GPIO_USART;
extern uint16_t GPIO_Pin_Alarm_LINE_North;//main.cpp
extern uint16_t GPIO_Pin_Alarm_LINE_East;//main.cpp
extern uint16_t GPIO_Pin_Alarm_LINE_South;//main.cpp
extern uint16_t GPIO_Pin_Alarm_LINE_West;//main.cpp
extern uint8_t SenderId;//main.cpp

extern GPIO_TypeDef * GPIO_RUNState;
extern uint16_t GPIO_Pin_LED_RUNState;

extern uint8_t SENS;//main.cpp

void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1,TIM_IT_Update)==SET){
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		if(GPIO_ReadOutputDataBit(GPIO_RUNState,GPIO_Pin_LED_RUNState)==SET){
		   	GPIO_ResetBits(GPIO_RUNState,GPIO_Pin_LED_RUNState);
		}else{
			GPIO_SetBits(GPIO_RUNState,GPIO_Pin_LED_RUNState);
		}	
	}		
}

void SendDataAlarmInfoToServer(uint32_t EXTI_LineX,uint16_t GPIO_Pin_Alarm_LINE_x,uint8_t lineid)
{
			
			delay_ms(SENS);
			// 红外 收到收红外信号为高电平 收不到时为低电平 注意红外设备的地线同电路板要共地线
			if(GPIO_ReadInputDataBit(GPIO_Alarm,GPIO_Pin_Alarm_LINE_x)==RESET){
				uint8_t* packetdata= PacketHelper::GetPacketClientNotifyToServerAlarm(SenderId,lineid);
				//uint8_t packetdata[10]={0x23, 0xff ,0x00 ,0xfe ,0x00, 0x00,0x00,0x00,0x22 ,0x24 };	
				UsartSendData(packetdata,PacketHelper::PacketLength);
				delete[] packetdata;
			}
			EXTI_ClearITPendingBit(EXTI_LineX);
}

// 红外中断
void EXTI9_5_IRQHandler(void)
{
	
	if(EXTI_GetITStatus(EXTI_Line6)==SET)
	{
			SendDataAlarmInfoToServer(EXTI_Line6,GPIO_Pin_Alarm_LINE_North,0x01);

	}
	
	if(EXTI_GetITStatus(EXTI_Line7)==SET)
	{
		SendDataAlarmInfoToServer(EXTI_Line7,GPIO_Pin_Alarm_LINE_East,0x02);
	}
	
	if(EXTI_GetITStatus(EXTI_Line8)==SET)
	{
		SendDataAlarmInfoToServer(EXTI_Line8,GPIO_Pin_Alarm_LINE_South,0x03);
	}
	
	if(EXTI_GetITStatus(EXTI_Line9)==SET)
	{
		SendDataAlarmInfoToServer(EXTI_Line9,GPIO_Pin_Alarm_LINE_West,0x04);
	}
}

/*仅用于发*/
void USART2_IRQHandler(void)
{
	
	//发送寄存器为空
	//当USART_DR中的数据传送到移位寄存器后，TXE被设置
	if(USART_GetITStatus(USART_Send, USART_IT_TXE)==SET)
	{
		
		USART_ClearITPendingBit(USART_Send,USART_IT_TXE);
 		if(GPIO_ReadOutputDataBit(GPIO_LED,GPIO_Pin_LED_USART_IT_TXE)==SET){
		   GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_TXE);
		}else{
		   GPIO_SetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_TXE);
		}
 		
		if(IsGetFirstBufferAddress){
			FirstBufferAddress=pSendBuffer;
			IsGetFirstBufferAddress=false;
		}
		if(pSendBuffer<(FirstBufferAddress+SendLength)){
		  	USART_SendData(USART_Send,*(pSendBuffer++));
		}else{
			IsSending=false;//发送完成	 
			USART_ITConfig(USART_Send,USART_IT_TXE,DISABLE);
			
		}
		//delay_ms(1000);
	}

	if(USART_GetITStatus(USART_Send, USART_IT_TC)==SET)
	{
 		if(GPIO_ReadOutputDataBit(GPIO_LED,GPIO_Pin_LED_USART_IT_TC)==SET){
		   GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_TC);
		}else{
		   GPIO_SetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_TC);
		}
		//delay_ms(1000);
		USART_ClearITPendingBit(USART_Send,USART_IT_TC);		  
	}
}



/*仅用于收*/
void USART1_IRQHandler(void)
{

	if(USART_GetITStatus(USART_Receive, USART_IT_RXNE)==SET)
	{


		if(GPIO_ReadOutputDataBit(GPIO_LED,GPIO_Pin_LED_USART_IT_RXNE)==SET){
		   GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_RXNE);
		}else{
		   GPIO_SetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_RXNE);
		}
		
		
		ReceviceBuffer[ReceviceBytes++]=USART_ReceiveData(USART_Receive);
		
		USART_ClearITPendingBit(USART_Receive,USART_IT_RXNE);
		
		//不能使用delay_ms(1000);
		//printf("x=%x",x);
	}

	if(USART_GetITStatus(USART_Receive, USART_IT_IDLE)==SET)
	{

		if(GPIO_ReadOutputDataBit(GPIO_LED,GPIO_Pin_LED_USART_IT_IDLE)==SET){
		   GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_IDLE);
		}else{
		   GPIO_SetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_IDLE);
		}
		//delay_ms(1000);
		USART_ClearITPendingBit(USART_Receive,USART_IT_IDLE);
	}
}
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
