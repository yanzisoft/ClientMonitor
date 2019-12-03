
#ifndef __PacketHelper_H
#define __PacketHelper_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#ifdef __cplusplus
 extern "C" {
#endif


typedef enum 
{
	Command,
	Data
}PacketType;

typedef enum 
{
	  //[Description("�ͻ��˻�Ӧ���������")]
    /// <summary>
    /// ��ʱ�ϳ� �ͻ��˻�Ӧ���������
    /// </summary>
    ClientReplyOnline=0,
    //[Description("�ͻ��˸�����˵ı���֪ͨ")]
    /// <summary>
    /// �ͻ��˸�����˵ı���֪ͨ
    /// </summary>
    ClientNotifyToServerAlarm=1,
    
    //[Description("�ͻ��˸�����˵Ĺػ�֪ͨ")]
    /// <summary>
    /// �ͻ��˸�����˵Ĺػ�֪ͨ
    /// </summary>
    ClientNotifyToServerShutdown=2,


   
    //[Description("�����Ҫ��ı�����״̬")]
    /// <summary>
    /// �����Ҫ��ı�����״̬
    /// </summary>
    ServerRequireClientChangeRunState=3,


    //[Description("�����Ҫ��ı����������")]
    /// <summary>
    /// �����Ҫ��ı����������
    /// </summary>
    ServerRequireClientChangeSENS=4,
		
    //[Description("�����ѯ�ʿͻ����Ƿ�����")]
    /// <summary>
    /// ��ʱ�ϳ� �����ѯ�ʿͻ����Ƿ�����
    /// </summary>
    ServerAskClientIsOnline=0xFE,
		
		//[Description("�����Ҫ��ͻ��˹ػ�")]
    /// <summary>
    /// �����Ҫ��ͻ��˹ػ�
    /// </summary>
    ServerRequireClientShutdown=0xFF,


}CommandCodeType;

enum CommandParaType
{
    //[Description("ֹͣ")]
    /// <summary>
    /// ֹͣ
    /// </summary>
    Stop,
    //[Description("����")]
    /// <summary>
    /// ����
    /// </summary>
    Run,

    //[Description("У׼")]
    /// <summary>
    /// У׼
    /// </summary>
    Adjust
};

class PacketHelper 	
{

   	public:
    /// <summary>
    /// ������(��λ�ֽ�)
    /// </summary>
    static const uint8_t  PacketLength=10;
	static const uint8_t PacketHead='#';
	static const uint8_t  PacketTail='$';
	//----------------------------
	static const uint8_t  PacketHeadLength=1;
	static const uint8_t  PacketTailLength=1;
	//----------------------------
	static const uint8_t  PacketHeadIndex=0;
	static const uint8_t  SenderIDIndex=1;
	static const uint8_t  PacketTypeIndex=2;
	static const uint8_t  CommandCodeIndex=3;
	static const uint8_t  CommandCodeBytes=2;
	static const uint8_t  CommandParaIndex=CommandCodeIndex + CommandCodeBytes;
	static const uint8_t  CommandParaBytes=2;
	static const uint8_t  CRCIndex=CommandParaIndex+CommandParaBytes;
	static const uint8_t  CRCFieldBytes=2;
	static const uint8_t  PacketTailIndex= CRCIndex + CRCFieldBytes;//��β �±�
	static const uint8_t  DataSegFieldBytes= PacketLength - PacketHeadLength - PacketTailLength - CRCFieldBytes;
	static const uint8_t ServerSenderID = 0xFF;
	//�����ܳ���10�ֽ� # 0xDD 0x00 0x01 0x02 0x03 0x04 0xCC 0xCC $
	private:
	uint8_t* mPacketdata;
	public:
	PacketHelper();
	PacketHelper(const uint8_t* data);
	~PacketHelper();

	static uint8_t* CreatePacket(uint8_t senderId,PacketType type, uint16_t cmdcode, uint16_t para);
	static void DeleteArrayBuffer(uint8_t* buffer);
	static uint8_t* CreateCmdPacket(uint8_t senderId,CommandCodeType code, uint16_t para);
	static uint8_t* CreateDataPacket(uint8_t senderId,CommandCodeType code, uint16_t para);
	
	static uint8_t* GetPacketClientNotifyToServerAlarm(uint8_t senderId,uint8_t para);
	static uint8_t* GetPacketClientNotifyToServerShutdown(uint8_t senderId);	
	static uint8_t* GetPacketClientReplyOnline(uint8_t senderId);
	static uint8_t* GetPacketClientRegister(uint8_t senderId);
	static std::string ByteArrayToHexString(uint8_t buffer[],uint8_t buffersize);
	
	uint8_t GetSenderID();
	CommandCodeType GetCommandCode();
	uint16_t GetCommandPara();
	PacketType GetPacketType();
	uint16_t GetCRC();
	std::string Describe();
	std::string ToString();
};

uint16_t CRC16(uint8_t data[], int startIndex, int length);

#ifdef __cplusplus
}
#endif
#endif	 /*__PacketHelper_H*/
