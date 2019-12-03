#ifndef __USARTSENDDATA_H
#define __USARTSENDDATA_H
#ifdef __cplusplus
	extern "C" {
#endif
	#include "stm32f10x.h"
	#include "stdint.h"
	extern bool IsGetFirstBufferAddress;
	extern char* FirstBufferAddress;
	extern char* pSendBuffer;
	extern uint8_t SendLength;
#ifdef __cplusplus
	}
#endif
	void UsartSendData(uint8_t* databuffer,uint8_t buffersize);
	void UsartSendData(char* databuffer,uint8_t buffersize);
	void UsartDisplayData(uint8_t* databuffer,uint8_t buffersize);
#endif
