#include "stm32f10x.h"
#include "packethelper.h"
#include "delay.h"
#include "disassemblebuffer.h"
#include "usartsenddata.h"

GPIO_TypeDef * GPIO_RUNState=GPIOA;
uint16_t GPIO_Pin_LED_RUNState=GPIO_Pin_13;

//PB12~15
//运行状态指示
GPIO_TypeDef * GPIO_LED=GPIOB;
uint16_t GPIO_Pin_LED_USART_IT_TXE=GPIO_Pin_12;
uint16_t GPIO_Pin_LED_USART_IT_TC=GPIO_Pin_13;
uint16_t GPIO_Pin_LED_USART_IT_RXNE=GPIO_Pin_14;
uint16_t GPIO_Pin_LED_USART_IT_IDLE=GPIO_Pin_15;
//PC6~9
// 红外检测
GPIO_TypeDef * GPIO_Alarm=GPIOC;
uint16_t GPIO_Pin_Alarm_LINE_North=GPIO_Pin_6;
uint16_t GPIO_Pin_Alarm_LINE_East=GPIO_Pin_7;
uint16_t GPIO_Pin_Alarm_LINE_South=GPIO_Pin_8;
uint16_t GPIO_Pin_Alarm_LINE_West=GPIO_Pin_9;

extern USART_TypeDef* USART_Receive;//=USART1; PA10
extern USART_TypeDef* USART_Send;//=USART2;	PA2
GPIO_TypeDef * GPIO_USART=GPIOA;

extern uint8_t ReceviceBuffer[PacketHelper::PacketLength];
extern uint8_t ReceviceBytes;

extern char* pSendBuffer;
extern uint8_t	SendLength;

//红外 灵敏度 越小越灵敏
uint8_t SENS=200;

void EnterSTANDBYMode();

uint8_t SenderId=0x01;
void TestPacketHelper();

//void OnReceiveError(std::string msg);
void OnReceiveError(char* buffer,uint8_t buffersize);
void OnReceivePocketData(uint8_t packetdata[],uint8_t packetLength);
CommandParaType RunState=Run;
// 运行状态指示灯
void GPIO_LED_Init(GPIO_InitTypeDef &GPIO_InitStruct)
{
	//单字节发送
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_LED_USART_IT_TXE;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_LED,&GPIO_InitStruct);
	GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_TXE);
	//单字节发送完成
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_LED_USART_IT_TC;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_LED,&GPIO_InitStruct);
	GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_TC);
	//接收寄存器非空
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_LED_USART_IT_RXNE;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_LED,&GPIO_InitStruct);
	GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_RXNE);
	//接收空闲
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_LED_USART_IT_IDLE;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_LED,&GPIO_InitStruct);
	GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_IDLE);
}


// 串口
void GPIO_USART_Init(GPIO_InitTypeDef &GPIO_InitStruct)
{
		//USART_Send 只用于接收 (即PA9没有实际使用)		PA10收 PA2发
	//PA9 没有实际使用 发送引脚	 
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIO_USART,&GPIO_InitStruct);
	//PA10 接收引脚	 
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIO_USART,&GPIO_InitStruct);

	//USART_Receive 只用于发送 (即PA3没有实际使用)
	//PA2 发送引脚
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIO_USART,&GPIO_InitStruct);
	//PA3 没有实际使用 接收引脚
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIO_USART,&GPIO_InitStruct);
}

// 红外 引脚
void GPIO_Alarm_Init(GPIO_InitTypeDef &GPIO_InitStruct)
{
	GPIOMode_TypeDef GPIO_Mode_IN=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_Alarm_LINE_North;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(GPIO_Alarm,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_Alarm_LINE_East;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(GPIO_Alarm,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_Alarm_LINE_South;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(GPIO_Alarm,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_Alarm_LINE_West;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(GPIO_Alarm,&GPIO_InitStruct);
}

// 红外中断
void EXTI_NVIC_Alarm_Init(EXTI_InitTypeDef &EXTI_InitStruct,NVIC_InitTypeDef &NVIC_InitStruct)
{
	EXTI_InitStruct.EXTI_Line=EXTI_Line6|EXTI_Line7|EXTI_Line8|EXTI_Line9;
	EXTI_InitStruct.EXTI_Mode= EXTI_Mode_Interrupt;
	// 红外 收到收红外信号为高电平 收不到时为低电平 注意红外设备的地线同电路板要共地线
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource6);	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource7);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource8);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource9);
	
	
	EXTI_Init(&EXTI_InitStruct);
	EXTI_ClearITPendingBit(EXTI_Line6|EXTI_Line7|EXTI_Line8|EXTI_Line9);
	
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;

	NVIC_Init(&NVIC_InitStruct);
}

void NVIC_USART_Init(NVIC_InitTypeDef &NVIC_InitStruct)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;

	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;

	NVIC_Init(&NVIC_InitStruct);
}

void USART_Config(USART_InitTypeDef &USART_InitStruct)
{
		//USART_Receive	  只用于接收
	USART_ClearITPendingBit(USART_Receive,	USART_IT_TXE|USART_IT_TC|USART_IT_RXNE|USART_IT_IDLE);
	//发送中断
	//USART_ITConfig(USART_Receive,USART_IT_TXE,ENABLE);	
	//传输完成中断
	//USART_ITConfig(USART_Receive,USART_IT_TC,ENABLE);
	//接收中断
	USART_ITConfig(USART_Receive,USART_IT_RXNE,ENABLE);
	//空闲总线中断
	//USART_ITConfig(USART_Receive,USART_IT_IDLE,ENABLE);

	USART_InitStruct.USART_BaudRate=9600;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;

	USART_Init(USART_Receive, &USART_InitStruct);
	USART_Cmd(USART_Receive,ENABLE);

	//USART_Send   只用于发送
	USART_ClearITPendingBit(USART_Send,	USART_IT_TXE|USART_IT_TC|USART_IT_RXNE);
	USART_ClearFlag(USART_Send,USART_FLAG_TXE|USART_FLAG_TC);
	//发送中断
	USART_ITConfig(USART_Send,USART_IT_TXE,DISABLE);	
	//传输完成中断
	USART_ITConfig(USART_Send,USART_IT_TC,ENABLE);
	//接收中断
	//USART_ITConfig(USART_Send,USART_IT_RXNE,ENABLE);
	//空闲总线中断
	//USART_ITConfig(USART_Send,USART_IT_IDLE,ENABLE);

	USART_Init(USART_Send, &USART_InitStruct);
	USART_Cmd(USART_Send,ENABLE);
}

void TIM_Init(NVIC_InitTypeDef &NVIC_InitStruct)
{
	TIM_DeInit(TIM1);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct; 
	TIM_TimeBaseInitStruct.TIM_Period = 2000; 
	TIM_TimeBaseInitStruct.TIM_Prescaler = 35999; 
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0; 
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM1, & TIM_TimeBaseInitStruct); 

	//TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	TIM_InternalClockConfig(TIM1);
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM1,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM1_UP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =4;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority	=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=	ENABLE;
	
	NVIC_Init(&NVIC_InitStruct);
}

int main()
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	USART_InitTypeDef USART_InitStruct;

	

	SystemInit();
	
	delay_init(72);

	//设置时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA,  ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1|RCC_APB2Periph_TIM1,  ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,  ENABLE);

	GPIO_USART_Init(GPIO_InitStruct);
	
	//是否正在正常运行指示灯
	if(SenderId==2){	
		GPIO_Pin_LED_RUNState=GPIO_Pin_12;	
	}
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_LED_RUNState;	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_RUNState,&GPIO_InitStruct);
	GPIO_SetBits(GPIO_RUNState,GPIO_Pin_LED_RUNState);

	GPIO_LED_Init(GPIO_InitStruct);

	//关机
	NVIC_SystemLPConfig(NVIC_LP_SLEEPDEEP,ENABLE);
	
	NVIC_USART_Init(NVIC_InitStruct);

	GPIO_Alarm_Init(GPIO_InitStruct);
	//红外 PC6~9
	EXTI_NVIC_Alarm_Init(EXTI_InitStruct,NVIC_InitStruct);
	
	
	USART_Config(USART_InitStruct);
	//TestPacketHelper();
	TIM_Init(NVIC_InitStruct);
		
	
	DisassembleBuffer* disBuffer=new DisassembleBuffer();
	// 设置拆分包错误处理程序
	disBuffer->SetReceiveErrorHandler(OnReceiveError);
	// 设置拆分包成功处理程序
	disBuffer->SetReceivePocketDataHandler(OnReceivePocketData);

	while(1){
		if(ReceviceBytes>0){
			// 正在处理拆分包
			disBuffer->Processing(ReceviceBuffer,ReceviceBytes);
			ReceviceBytes=0;	
		}
	}
}


void OnReceiveError(char* buffer,uint8_t buffersize)
{
	UsartSendData(buffer,buffersize);
}

void OnReceivePocketData(uint8_t packetdata[],uint8_t packetLength)
{
	PacketHelper* ph=new PacketHelper(packetdata);
	CommandCodeType code=ph->GetCommandCode();

	//测试用 翻译显示收到的指令描述
	printf("\r\n收到指令：%s",ph->Describe().c_str());
	
	switch(code)
	{
		default:
			//UsartSendData(packetdata,PacketHelper::PacketLength);
			printf("\r\n收到未处理指令：%s",ph->ToString().c_str());
			break;
		//服务端要求关机
		case ServerRequireClientShutdown:
			//printf("已关机");	
			EnterSTANDBYMode();
			
		break;
		//服务端要求改求运行状态
		case ServerRequireClientChangeRunState:
			RunState=(CommandParaType)ph->GetCommandPara();
			//printf("当前运行状态是:%d",RunState);
		//服务端要求改求红外灵敏度
		case ServerRequireClientChangeSENS:
			SENS=(CommandParaType)ph->GetCommandPara();
			//printf("当前红外灵敏度是:%d",SENS);
		break;
		//服务端询问客户端是否在线
		case ServerAskClientIsOnline:
			uint8_t* packetdata= PacketHelper::GetPacketClientReplyOnline(SenderId);
			
			UsartSendData(packetdata,PacketHelper::PacketLength);
			//UsartDisplayData(packetdata,PacketHelper::PacketLength);
			delete[] packetdata;
		
		break;

	}
	
	delete ph;
}

void EnterSTANDBYMode()
{
			//关LED屏
			
			
			//关掉LED状态指示灯
			//不知为什么PB12非得高电平才不亮
			GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_TXE);
			GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_TC);
			GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_RXNE);
			GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_IDLE);
			GPIO_SetBits(GPIOC,GPIO_Pin_13);
	
			//发送关机通知包给服务端
			uint8_t* packetdata= PacketHelper::GetPacketClientNotifyToServerShutdown(SenderId);
			
			UsartSendData(packetdata,PacketHelper::PacketLength);

			//UsartDisplayData(packetdata,PacketHelper::PacketLength);
			delete[] packetdata;			
			//PA0 为唤醒引脚
			PWR_WakeUpPinCmd(ENABLE);
			PWR_EnterSTANDBYMode();		
}

void EnterRunMode()
{
	//开始LED屏
	
	//给LED屏 传送显示内容
	
	//可选 播放音频 XX围栏进入某某状态 
}

