#include "cJSON.h"
#include "protocol.h"

static unsigned short uoi_skt_crc16(const unsigned char* data_p, int length)
{
	unsigned char  x;
	unsigned short crc = 0xFFFF;
	
	while (length-- > 0) 
	{
		x = (crc >> 8) ^ *data_p++;
		x ^= x >> 4;
		crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x << 5)) ^ ((unsigned short)x);
	}
	
	return crc;
}

static void PacketHead(T_PacketHead *pstHead, unsigned char *CmdType, char *pBuf, unsigned char *pLen)
{
	cJSON *packetHead = NULL;
	char *pMsg = NULL;
	packetHead= cJSON_CreateObject();
	
	
	if(packetHead = NULL)
	{
		return;
	}
	
	cJSON_AddStringToObject(root, "basestationid", "基站ID");
	cJSON_AddStringToObject(root, "msg_type",      "命令类型");
	cJSON_AddStringToObject(root, "uuid",          "终端ID");
	cJSON_AddStringToObject(root, "basestationid", "基站ID");
	Msg = cJSON_Print(root);
	strcpy(pBuf, pMsg);
	*pLen = strlen(pMsg);
	free(Msg);
	Msg = NULL;
	cJSON_Delete(packetHead);
	packetHead = NULL;
	

}

unsigned int AssemblePacket(char *InBuf, unsigned char InLen, char *outBuf, unsigned char OutLen)
{
	PacketHead();
	return 0;
}


unsigned int parsePacket(char *InBuf, unsigned char InLen, char *outBuf, unsigned char OutLen)
{

	return 0;
}



