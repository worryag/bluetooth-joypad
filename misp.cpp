#include "Protocol.h"
#include "misp.h"

int _misp_bluetooth(struct tPacket *packet, short y1_val, short y2_val, unsigned char W, short w_val)
{
  if(packet==nullptr)
    return -1;
  packet->_dir = FROM_HOST;
  unsigned char *p = packet->_payload;
  *((short*)p)=y1_val;
  p=p+sizeof(short);
  *((short*)p)=y2_val;
  p=p+sizeof(short);
  *((unsigned char*)p)=W;
  p=p+sizeof(unsigned char);
  *((short*)p)=w_val;
  p=p+sizeof(short);
  packet->_size =3*sizeof(short)+sizeof(unsigned char);
  return _protocol_crc_calc(packet);
}

short _rx_get_1short(struct tPacket *packet)
{
  unsigned char *p=packet->_payload;
  return *((short*)p);
}

short _rx_get_2short(struct tPacket *packet)
{
  unsigned char *p=packet->_payload+2;
  return *((short*)p);
}

unsigned char _rx_get_Weapon(struct tPacket *packet)
{
  unsigned char *p = packet->_payload + 4;
  return *p;
}
short _rx_get_Wep_val(struct tPacket *packet)
{
  unsigned char *p=packet->_payload+5;
  return *((short*)p);
}
/***************************************************************************/

unsigned char _rx_get_cmd(struct tPacket *packet)
{
  unsigned char *p = packet->_payload;
  return *p;
}

float _rx_get_1f32(struct tPacket *packet)
{
  unsigned char *p = packet->_payload + 1;
  return *((float*)p);
}

unsigned char _rx_get_1b8(struct tPacket *packet)
{
  unsigned char *p = packet->_payload + 1;
  return *p;
}

unsigned char _rx_get_2b8(struct tPacket *packet)
{
  unsigned char *p = packet->_payload + 2;
  return *p;
}

float _rx_get_2f32(struct tPacket *packet)
{
  unsigned char *p = packet->_payload + 2;
  return *((float*)p);
}

float _rx_get_5f32(struct tPacket *packet)
{
  unsigned char *p = packet->_payload + 5;
  return *((float*)p);
}

float _rx_get_9f32(struct tPacket *packet)
{
  unsigned char *p = packet->_payload + 9;
  return *((float*)p);
}

unsigned int _rx_get_2u16(struct tPacket *packet)
{
  unsigned char *p = packet->_payload + 2;
  return *((unsigned int*)p);
}

unsigned int _rx_get_4u16(struct tPacket *packet)
{
  unsigned char *p = packet->_payload + 4;
  return *((unsigned int*)p);
}
