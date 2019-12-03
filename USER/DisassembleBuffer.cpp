#include "disassemblebuffer.h"
#include "usartsenddata.h"//测试用
DisassembleBuffer::DisassembleBuffer()
{
	validBytes = 0;
	headIndex=-1;
}

DisassembleBuffer::~DisassembleBuffer()
{
	
}

int DisassembleBuffer::FindHeadIndex(uint8_t data[],uint8_t dataLength, int startIndex, int MaxIndex)
{
	//Debug.WriteLine(string.Format("startIndex={0},MaxIndex={1}", startIndex, MaxIndex));
	int rtn = -1;
	if (startIndex < 0) startIndex = 0;

	if (dataLength < MaxIndex)
	{
			MaxIndex = dataLength;
	}
	for (int i = startIndex; i < MaxIndex - PacketHelper::PacketHeadLength; i++)
	{
			//if (data[i] == PacketHelper.PacketHead[0] && data[i + 1] == PacketHelper.PacketHead[1]) //?????
		if (data[i] ==PacketHelper::PacketHead) //
			{
					rtn = i;
					break;
			}
	}
	return rtn;
}

/// <summary>
/// 此函数的作用是把接收缓冲区的混在一起众多数据包分解为一个个独立数据包
/// </summary>
/// <param name="data">接收缓冲区</param>
/// <param name="length">收到的效数据长度</param>

extern USART_TypeDef* USART_Send;//测试用
extern uint16_t GPIO_Pin_LED_USART_IT_IDLE;
void DisassembleBuffer::Processing(uint8_t data[], int length)
{
		//把新的数据合并PacketBuffer缓冲区中
		//把data中的数据拷入以validBytes为下标的开始PacketBuffer缓冲区中
		memcpy(PacketBuffer+validBytes,data,length);
		validBytes += length;
		
		//检查PacketBuffer中的数据是否存在完整的数据包
		//如果存在完整的数据包(可能存在多个完整的数据包)则处理所有存在的完整数据包
		//如果不存在完整的数据包，则什么也不做等待接收更多的数据以组成完整的数据包

		// 如果找到包头             如果没找到包头什么也不做继续接收数据并合并到PacketBuffer缓冲区中
		while (validBytes >= PacketHelper::PacketLength )
		{

				headIndex = FindHeadIndex(PacketBuffer,PacketBufferLength, headIndex + 1, validBytes);

				//如果找到了包头且包头下标不等0 则前移数据包
				if (headIndex > 0)
				{   //前移数据包
						//Copy(PacketBuffer, headIndex, PacketBuffer, 0, validBytes - headIndex);
						//void *memmove( void* dest, const void* src, size_t count );
						memmove(PacketBuffer,PacketBuffer+headIndex,validBytes - headIndex);
						//memset(PacketBuffer+(validBytes - headIndex), 0,PacketHelper::PacketLength -(validBytes - headIndex));
						validBytes -= headIndex;
						headIndex = -1;
					
						//UsartSendData(PacketBuffer,validBytes);
						//USART_ITConfig(USART_Send,USART_IT_TXE,ENABLE);
						if (validBytes < PacketHelper::PacketLength)
						{
								continue;
						}
				}
				else if(headIndex==-1)//如果未找到
				{
						validBytes = 0;
						continue;
				}


				//headIndex==0 如果包尾正确则向后执行 不然继续循环
				if (PacketBuffer[PacketHelper::PacketTailIndex] != PacketHelper::PacketTail)
				{
						//--------------测试---------------------
						GPIO_TypeDef * GPIO_LED=GPIOB;
						if(GPIO_ReadOutputDataBit(GPIO_LED,GPIO_Pin_LED_USART_IT_IDLE)==SET){
							 GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_IDLE);
						}else{
							 GPIO_SetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_IDLE);
						}
						//--------------测试---------------------
						continue;
				}

				uint8_t packetData [PacketHelper::PacketLength];
				//void *memcpy(void *dest, const void *src, size_t n);
				memcpy(packetData,PacketBuffer,PacketHelper::PacketLength);
				//UsartDisplayData(packetData,PacketHelper::PacketLength);
				//Copy(PacketBuffer, 0, packetData, PacketHelper::PacketHeadIndex, PacketHelper::PacketLength);
				//如果检查CRC正确
				
				/*根据数据段计算的CRC*/
				uint16_t crc=CRC16(PacketBuffer, PacketHelper::PacketHeadLength, PacketHelper::DataSegFieldBytes);/*无符号16位整型*/
				/*取得数据包里的CRC*/
				uint16_t crcInPacket = (PacketBuffer[PacketHelper::CRCIndex+1]<<8)+PacketBuffer[PacketHelper::CRCIndex];/*无符号16位整型*/
				
				if (crc == crcInPacket)
				{
						//处理数据段
						//Debug.WriteLine(BitConverter.ToString(packetData));
						//UsartDisplayData(packetData,PacketHelper::PacketLength);
						if (OnReceivePocketData != 0) OnReceivePocketData(packetData,PacketHelper::PacketLength);
				}
				else//如果检查CRC错误报执行超时
				{
					
//					char ErrorMessageBuffer[80];
					std::string hexStringPacketData=PacketHelper::ByteArrayToHexString(packetData,PacketHelper::PacketLength);
					std::sprintf(ErrorMessageBuffer,"\r\n数据包:%s数据段的CRC校验失败：计算CRC:%04X 包中的CRC:%04X。",hexStringPacketData.c_str(),crc,crcInPacket);
//					if (OnReceiveError != 0) OnReceiveError(std::string(ErrorMessageBuffer));
						if (OnReceiveError != 0) OnReceiveError(ErrorMessageBuffer,ErrorMessageBufferLength);
						
				}

				//前移未处理的数据
				int unhandleLength = validBytes - PacketHelper::PacketLength;
				if (unhandleLength > 0)
				{
					memmove(PacketBuffer,PacketBuffer+PacketHelper::PacketLength,unhandleLength);
					//Copy(PacketBuffer, PacketHelper::PacketLength, PacketBuffer, 0, unhandleLength);
					validBytes -= PacketHelper::PacketLength;
					headIndex = -1;
				}
				else
				{
						validBytes = 0;
						headIndex = -1;
				}
		}
}

void DisassembleBuffer::SetReceivePocketDataHandler(OnReceivePocketDataFun pf)
{
	OnReceivePocketData=pf;
}
void DisassembleBuffer::SetReceiveErrorHandler(OnReceiveErrorFun pf)
{
	OnReceiveError=pf;
}





