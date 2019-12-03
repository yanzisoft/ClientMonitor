#include "packethelper.h"
#include "stm32f10x.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "redirect.h"
#include "usart.h"
#include "test.h"


void TestPacketHelper()
{


	USART2_SystemInit_RCC_Configuration();
	USART_Configuration(USART2,9600);

	uint8_t data[PacketHelper::PacketLength]={'#',0x00,0x01,0x02,0x00/*需要跟enum定义对应,不然类型无法正转化*/,0x04,0x05,0xc6,0xc7,'$'};

	TEST(PacketHelper::PacketLength,0x0a);
	TEST(data[PacketHelper::PacketHeadIndex],'#');
	TEST(data[PacketHelper::SenderIDIndex],0x00);
	TEST(data[PacketHelper::PacketTypeIndex],0x01);
	TEST(data[PacketHelper::CommandCodeIndex],0x02);
	TEST(data[PacketHelper::CommandCodeIndex+1],0x00);
	TEST(data[PacketHelper::CommandParaIndex],0x04);
	TEST(data[PacketHelper::CommandParaIndex+1],0x05);
	TEST(data[PacketHelper::CRCIndex],0xc6);
	TEST(data[PacketHelper::CRCIndex+1],0xc7);
	TEST(data[PacketHelper::PacketTailIndex],'$');
	
	PacketHelper* x=new PacketHelper(data);
	TEST(x->GetSenderID(),data[PacketHelper::SenderIDIndex]);
	TEST(x->GetCommandCode(),data[PacketHelper::CommandCodeIndex]+data[PacketHelper::CommandCodeIndex+1]*256);
	TEST(x->GetCommandPara(),data[PacketHelper::CommandParaIndex]+data[PacketHelper::CommandParaIndex+1]*256);
	TEST(x->GetPacketType(),data[PacketHelper::PacketTypeIndex]);
	
	uint8_t* pdata=PacketHelper::CreateCmdPacket(0xc1,ClientNotifyToServerAlarm,0x1234);
	PacketHelper* x1=new PacketHelper(pdata);
	TEST(x1->GetSenderID(),pdata[PacketHelper::SenderIDIndex]);
	TEST(x1->GetCommandCode(),pdata[PacketHelper::CommandCodeIndex]+pdata[PacketHelper::CommandCodeIndex+1]*256);
	TEST(x1->GetCommandCode(),ClientNotifyToServerAlarm);
	TEST(x1->GetCommandPara(),pdata[PacketHelper::CommandParaIndex]+pdata[PacketHelper::CommandParaIndex+1]*256);
	TEST(x1->GetPacketType(),pdata[PacketHelper::PacketTypeIndex]);
	TEST(x1->GetPacketType(),Command);
	TEST(x1->GetCRC(),CRC16(pdata,PacketHelper::PacketHeadLength,PacketHelper::DataSegFieldBytes));
	PacketHelper::DeleteArrayBuffer(pdata);
	
	uint8_t* pdata2=PacketHelper::CreateDataPacket(0xc1,ClientNotifyToServerShutdown,0x4567);
	PacketHelper* x2=new PacketHelper(pdata2);
	TEST(x2->GetSenderID(),pdata2[PacketHelper::SenderIDIndex]);
	TEST(x2->GetCommandCode(),pdata2[PacketHelper::CommandCodeIndex]+pdata2[PacketHelper::CommandCodeIndex+1]*256);
	TEST(x2->GetCommandCode(),ClientNotifyToServerShutdown);
	TEST(x2->GetCommandPara(),pdata2[PacketHelper::CommandParaIndex]+pdata2[PacketHelper::CommandParaIndex+1]*256);
	TEST(x2->GetPacketType(),pdata2[PacketHelper::PacketTypeIndex]);
	TEST(x2->GetPacketType(),Data);
	TEST(x2->GetCRC(),CRC16(pdata2,PacketHelper::PacketHeadLength,PacketHelper::DataSegFieldBytes));
	PacketHelper::DeleteArrayBuffer(pdata2);
	delete x;
	delete x1;
	delete x2;
}
