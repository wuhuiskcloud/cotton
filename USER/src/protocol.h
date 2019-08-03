#ifndef PROTOCOL_H
#define PROTOCOL_H

#define UUID_MAX_LEN 16;
#define BASE_ST_LEN 16

typedef struct tagPacketHead{
	char UUID[UUID_MAX_LEN];
	char BaseStationId[BASE_ST_LEN];
}T_PacketHead;

#endif