#ifndef __DISASSEMBLEBUFFER_H
#define __DISASSEMBLEBUFFER_H
#include <string>
#include <stdio.h>
#ifdef __cplusplus
	extern "C" {
#endif

#include "stdint.h"
#include "PacketHelper.h"
#include "stm32f10x.h"		
typedef void (*OnReceivePocketDataFun)(uint8_t packetdata[],uint8_t packetLength);
//typedef void (*OnReceiveErrorFun)(std::string errorMsg);
typedef void (*OnReceiveErrorFun)(char* errorMsg,uint8_t buffersize);		

class DisassembleBuffer
{
	public:
		DisassembleBuffer();
		~DisassembleBuffer();
		int FindHeadIndex(uint8_t data[],uint8_t dataLength, int startIndex, int MaxIndex);
		void Processing(uint8_t data[], int length);
		void SetReceivePocketDataHandler(OnReceivePocketDataFun pf);
		void SetReceiveErrorHandler(OnReceiveErrorFun pf);
		
		static const uint8_t PacketBufferLength=3 * PacketHelper::PacketLength;
		static const uint8_t ErrorMessageBufferLength=90;
	
	private:
		uint8_t PacketBuffer[PacketBufferLength];
		int validBytes;
		int headIndex;
		OnReceivePocketDataFun	OnReceivePocketData;
		OnReceiveErrorFun OnReceiveError;
		char ErrorMessageBuffer[ErrorMessageBufferLength];
		
};
		
#ifdef __cplusplus
	}
#endif
#endif
