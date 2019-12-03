#include "disassemblebuffer.h"
#include "usartsenddata.h"//������
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
/// �˺����������ǰѽ��ջ������Ļ���һ���ڶ����ݰ��ֽ�Ϊһ�����������ݰ�
/// </summary>
/// <param name="data">���ջ�����</param>
/// <param name="length">�յ���Ч���ݳ���</param>

extern USART_TypeDef* USART_Send;//������
extern uint16_t GPIO_Pin_LED_USART_IT_IDLE;
void DisassembleBuffer::Processing(uint8_t data[], int length)
{
		//���µ����ݺϲ�PacketBuffer��������
		//��data�е����ݿ�����validBytesΪ�±�Ŀ�ʼPacketBuffer��������
		memcpy(PacketBuffer+validBytes,data,length);
		validBytes += length;
		
		//���PacketBuffer�е������Ƿ�������������ݰ�
		//����������������ݰ�(���ܴ��ڶ�����������ݰ�)�������д��ڵ��������ݰ�
		//������������������ݰ�����ʲôҲ�����ȴ����ո����������������������ݰ�

		// ����ҵ���ͷ             ���û�ҵ���ͷʲôҲ���������������ݲ��ϲ���PacketBuffer��������
		while (validBytes >= PacketHelper::PacketLength )
		{

				headIndex = FindHeadIndex(PacketBuffer,PacketBufferLength, headIndex + 1, validBytes);

				//����ҵ��˰�ͷ�Ұ�ͷ�±겻��0 ��ǰ�����ݰ�
				if (headIndex > 0)
				{   //ǰ�����ݰ�
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
				else if(headIndex==-1)//���δ�ҵ�
				{
						validBytes = 0;
						continue;
				}


				//headIndex==0 �����β��ȷ�����ִ�� ��Ȼ����ѭ��
				if (PacketBuffer[PacketHelper::PacketTailIndex] != PacketHelper::PacketTail)
				{
						//--------------����---------------------
						GPIO_TypeDef * GPIO_LED=GPIOB;
						if(GPIO_ReadOutputDataBit(GPIO_LED,GPIO_Pin_LED_USART_IT_IDLE)==SET){
							 GPIO_ResetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_IDLE);
						}else{
							 GPIO_SetBits(GPIO_LED,GPIO_Pin_LED_USART_IT_IDLE);
						}
						//--------------����---------------------
						continue;
				}

				uint8_t packetData [PacketHelper::PacketLength];
				//void *memcpy(void *dest, const void *src, size_t n);
				memcpy(packetData,PacketBuffer,PacketHelper::PacketLength);
				//UsartDisplayData(packetData,PacketHelper::PacketLength);
				//Copy(PacketBuffer, 0, packetData, PacketHelper::PacketHeadIndex, PacketHelper::PacketLength);
				//������CRC��ȷ
				
				/*�������ݶμ����CRC*/
				uint16_t crc=CRC16(PacketBuffer, PacketHelper::PacketHeadLength, PacketHelper::DataSegFieldBytes);/*�޷���16λ����*/
				/*ȡ�����ݰ����CRC*/
				uint16_t crcInPacket = (PacketBuffer[PacketHelper::CRCIndex+1]<<8)+PacketBuffer[PacketHelper::CRCIndex];/*�޷���16λ����*/
				
				if (crc == crcInPacket)
				{
						//�������ݶ�
						//Debug.WriteLine(BitConverter.ToString(packetData));
						//UsartDisplayData(packetData,PacketHelper::PacketLength);
						if (OnReceivePocketData != 0) OnReceivePocketData(packetData,PacketHelper::PacketLength);
				}
				else//������CRC����ִ�г�ʱ
				{
					
//					char ErrorMessageBuffer[80];
					std::string hexStringPacketData=PacketHelper::ByteArrayToHexString(packetData,PacketHelper::PacketLength);
					std::sprintf(ErrorMessageBuffer,"\r\n���ݰ�:%s���ݶε�CRCУ��ʧ�ܣ�����CRC:%04X ���е�CRC:%04X��",hexStringPacketData.c_str(),crc,crcInPacket);
//					if (OnReceiveError != 0) OnReceiveError(std::string(ErrorMessageBuffer));
						if (OnReceiveError != 0) OnReceiveError(ErrorMessageBuffer,ErrorMessageBufferLength);
						
				}

				//ǰ��δ���������
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





