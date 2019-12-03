#include "PacketHelper.h"

PacketHelper::PacketHelper()
{
	if(mPacketdata==NULL){
		mPacketdata = new uint8_t[PacketLength];
	}
}

//void *memcpy(void *dest, const void *src, size_t n);
PacketHelper::PacketHelper(const uint8_t* data)
{
	if(mPacketdata==NULL){
		mPacketdata = new uint8_t[PacketLength];
	}
	//�ڴ濽��
	memcpy(mPacketdata,data,sizeof(uint8_t)*PacketLength);
}

PacketHelper::~PacketHelper()
{
	DeleteArrayBuffer(mPacketdata);
}

std::string PacketHelper::ByteArrayToHexString(uint8_t buffer[],uint8_t buffersize)
{
	char* hexstring=new char[buffersize*3+1];// +1��ʾ Ϊ��βЯ��'\0'�ַ� 

	for(int i=0;i<buffersize;i++){ //0,1,2,......,buffersize-1
			// sprintf���������ַ�ʱ�����ִ���βЯ��'\0'�ַ� 
			// �籾�β�����ʵ��4���ַ�:0xXX 0xXX 0x20 0x00
			std::sprintf(hexstring+i*3,"%02X ",buffer[i]/*0,1,...9*/);
	}
		
	std::string rtn=std::string(hexstring);
	delete[] hexstring;
	return rtn;

}

void PacketHelper::DeleteArrayBuffer(uint8_t* buffer)
{
	if(buffer!=0){
	  delete[](buffer);
	  buffer=0;
	}
}

uint8_t* PacketHelper::CreatePacket(uint8_t senderId,PacketType type, uint16_t cmdcode, uint16_t para)
{
	// { PacketHead, senderId, (uint8_t)type, (uint8_t)cmdcode,
	// (uint8_t)(cmdcode >> 8), (uint8_t)(para), (uint8_t)(para >> 8),
	// 0, 0, PacketTail };

  uint8_t* data = new uint8_t[PacketLength];
	data[PacketHeadIndex]= PacketHead;
	data[SenderIDIndex]= senderId;
	data[PacketTypeIndex]= (uint8_t)type;
	data[CommandCodeIndex]= (uint8_t)cmdcode;
	data[CommandCodeIndex+1]= (uint8_t)(cmdcode >> 8);
	data[CommandParaIndex]= (uint8_t)(para);
	data[CommandParaIndex+1]= (uint8_t)(para >> 8);

	uint16_t crc=CRC16(data,PacketHeadLength,DataSegFieldBytes);

	data[CRCIndex]= (uint8_t)crc;
	data[CRCIndex + 1]= (uint8_t)(crc >> 8);
	data[PacketTailIndex]= PacketTail;

    return data;

}

uint8_t* PacketHelper::CreateCmdPacket(uint8_t senderId,CommandCodeType code, uint16_t para)
{
   return CreatePacket(senderId,Command, code, para);
}
uint8_t* PacketHelper::CreateDataPacket(uint8_t senderId,CommandCodeType code, uint16_t para)
{
	return CreatePacket(senderId,Data, code, para);
}

/// <summary>
/// �ͻ��˸�����˵Ĺػ�֪ͨ
/// </summary>
/// <param name="senderId">�ͻ���id</param>
/// <returns>������</returns>
uint8_t* PacketHelper::GetPacketClientNotifyToServerShutdown(uint8_t senderId)
{
    return CreateCmdPacket(senderId,ClientNotifyToServerShutdown,0);
}


/// <summary>
/// �ͻ��˻�Ӧ���������
/// </summary>
/// <param name="senderId">�ͻ���id</param>
/// <returns></returns>
uint8_t* PacketHelper::GetPacketClientReplyOnline(uint8_t senderId)
{
    return CreateCmdPacket(senderId,ClientReplyOnline, 0);
}

/// <summary>
/// �ͻ���ע��
/// </summary>
/// <param name="senderId">�ͻ���id</param>
/// <returns></returns>
//uint8_t* PacketHelper::GetPacketClientRegister(uint8_t senderId)
//{
//    return CreateCmdPacket(senderId,ClientRegister, 0);
//}


/// <summary>
/// �ͻ��˸�����˵ı���֪ͨ
/// </summary>
/// <param name="senderId">�ͻ���id</param>
/// <param name="para">Χ����һ���ϱ���</param>
/// <returns></returns>
uint8_t* PacketHelper::GetPacketClientNotifyToServerAlarm(uint8_t senderId,uint8_t para)
{
	 return CreateCmdPacket(senderId,ClientNotifyToServerAlarm, para);
}

uint16_t CRC16(uint8_t data[], int startIndex, int length)
{
    int crc = 0xffff;
    int tmpMoveOut;
    int tmpNum;

    for ( int i= startIndex;i < (startIndex + length); i++)
    {
        tmpNum = (crc >> 8) & 0xFF;
        crc = data[i] ^ tmpNum;

        for ( int j = 0;j < 8; j++)
        {
            tmpMoveOut = crc & 0x0001;
            crc = crc >> 1;
            if (tmpMoveOut == 1)
            {
                crc = crc ^ 0xA001;//����ʽ�������㣬��������ʽ��ֵ���Ը����㷨��
            }
        }

    }
    return (uint16_t)crc;
}




uint8_t PacketHelper::GetSenderID()
{
	return mPacketdata[SenderIDIndex];
}

CommandCodeType PacketHelper::GetCommandCode()
{
	 return (CommandCodeType)((mPacketdata[CommandCodeIndex+1] <<8)+ mPacketdata[CommandCodeIndex]);
}

std::string PacketHelper::Describe()
{
	CommandCodeType t=this->GetCommandCode();
	std::string s;
	switch(t)
	{
		case ClientNotifyToServerAlarm:
			s="�ͻ��˸�����˵ı���֪ͨ";
		break;
		case ClientNotifyToServerShutdown:
			s="�ͻ��˸�����˵Ĺػ�֪ͨ";
		break;
		case ServerRequireClientShutdown:
			s="�����Ҫ��ͻ��˹ػ�";
		break;
		case ServerRequireClientChangeRunState:
			s="�����Ҫ��ı�����״̬";
		break;
		case ServerRequireClientChangeSENS:
			s="�����Ҫ��ı����������";
		break;
		case ServerAskClientIsOnline:
			s="�����ѯ�ʿͻ����Ƿ�����";
		break;
		case ClientReplyOnline:
			s="�ͻ��˻�Ӧ���������";
		break;
	}
		char desc[80];
		std::sprintf(desc,"%s ���Ӳ���:%04X",s.c_str(),this->GetCommandPara());
	
		return std::string(desc);
}

std::string PacketHelper::ToString()
{
	return PacketHelper::ByteArrayToHexString(mPacketdata,PacketHelper::PacketLength);
}

uint16_t PacketHelper::GetCommandPara()
{
	return (mPacketdata[CommandParaIndex+1] <<8)+ mPacketdata[CommandParaIndex];
}

PacketType PacketHelper::GetPacketType()
{
	return (PacketType)mPacketdata[PacketTypeIndex];
}

uint16_t PacketHelper::GetCRC()
{
	return (mPacketdata[CRCIndex+1] <<8)+ mPacketdata[CRCIndex];
}



