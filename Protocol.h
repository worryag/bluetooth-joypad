#ifndef protocol_h
#define protocol_h

#define TO_HOST '>'
#define FROM_HOST '<'

#define MAX_PAYLOAD_SIZE 16
#define PROTOCOL_MAX_PACKET (MAX_PAYLOAD_SIZE + 5)

struct tPacket
{
	char _header[2] = { '$', 'M' };           // Magic char
    char _dir = FROM_HOST;                    // Package way
    unsigned char _size;                      // Real size payload
    unsigned char _payload[MAX_PAYLOAD+SIZE]; // massive payload ( Cmd and Data)
    unsigned char _crc;                       // байт суммы по модулю 2 байтов полей _size и полезной части payload
};

int _protocol_crc_calc(struct tPacket *packet);

bool _protocol_crc_check(struct tPacket *packet);

int _protocol_p2d(struct tPacket *packet, unsigned char *data);

int _protocol_d2p(struct tPacket *packet, unsigned char *data);

#endif
