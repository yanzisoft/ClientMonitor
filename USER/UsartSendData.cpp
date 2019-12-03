#include "usartsenddata.h"
#include "stdio.h"

extern USART_TypeDef* USART_Send;
extern bool IsSending;

void UsartDisplayData(uint8_t* databuffer,uint8_t buffersize)
{
	while(IsSending);
	IsSending=true;
	USART_ClearFlag(USART_Send,USART_FLAG_TXE|USART_FLAG_TC);
	IsGetFirstBufferAddress=true;
	FirstBufferAddress=0;
	uint8_t buffersize2=buffersize+2;//+2 表示"\r\n "; "\r\n "; //12
	uint8_t buffersize3=buffersize2*3; //36
	char* charbuffer=new char[buffersize3+1];

	for(int i=0;i<buffersize2;i++){ //0,1,2,......,10,11
		if(i==0||i==buffersize2-1) //0,11
		{
			// sprintf函数操作字符时会在字串结尾携带'\0'字符 
			// 如本次操作其实共4个字符:0x0D 0x0A 0x20 0x00
			sprintf(charbuffer+i*3,"\r\n ");
		}
		else
		{
			// sprintf函数操作字符时会在字串结尾携带'\0'字符 
			// 如本次操作其实共4个字符:0xXX 0xXX 0x20 0x00
			sprintf(charbuffer+i*3,"%02X ",databuffer[i-1]/*0,1,...9*/);
		}
		
	}
	
	SendLength=buffersize3;
	pSendBuffer=(char*)charbuffer;
	
	USART_ITConfig(USART_Send,USART_IT_TXE,ENABLE);
	while(IsSending);
	if(charbuffer!=0) {
		delete[] charbuffer;
		charbuffer=0;
	}
}

void UsartSendData(uint8_t* databuffer,uint8_t buffersize)
{
	while(IsSending);
	IsSending=true;
	USART_ClearFlag(USART_Send,USART_FLAG_TXE|USART_FLAG_TC);
	IsGetFirstBufferAddress=true;
	FirstBufferAddress=0;
	SendLength=buffersize*sizeof(uint8_t);
	pSendBuffer=(char*)databuffer;
	USART_ITConfig(USART_Send,USART_IT_TXE,ENABLE);
	while(IsSending);
}

void UsartSendData(char* databuffer,uint8_t buffersize)
{
	while(IsSending);
	IsSending=true;
	USART_ClearFlag(USART_Send,USART_FLAG_TXE|USART_FLAG_TC);
	IsGetFirstBufferAddress=true;
	FirstBufferAddress=0;
	SendLength=buffersize*sizeof(char);
	pSendBuffer=databuffer;
	USART_ITConfig(USART_Send,USART_IT_TXE,ENABLE);
	while(IsSending);
}
