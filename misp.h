#ifndef misp_h
#define misp_h

#include "Protocol.h"

int _misp_bluetooth(struct tPacket *packet,short y1_val,short y2_val, unsigned char W, short w_val);
unsigned char _rx_get_cmd(struct tPacket *packet);
float _rx_get_1f32(struct tPacket *packet);
unsigned char _rx_get_1b8(struct tPacket *packet);
unsigned char _rx_get_2b8(struct tPacket *packet);
float _rx_get_2f32(struct tPacket *packet);
float _rx_get_5f32(struct tPacket *packet);
float _rx_get_9f32(struct tPacket *packet);
unsigned int _rx_get_2u16(struct tPacket *packet);
unsigned int _rx_get_4u16(struct tPacket *packet);
short _rx_get_1short(struct tPacket *packet);
short _rx_get_2short(struct tPacket *packet);
unsigned char _rx_get_Weapon(struct tPacket *packet);
short _rx_get_Wep_val(struct tPacket *packet);
#define _rx_get_ident _rx_get_1b8
#define _rx_get_path _rx_get_1f32
#define _rx_get_lin_speed _rx_get_1f32
#define _rx_get_ang_speed _rx_get_5f32
#define _rx_get_dist _rx_get_9f32
#define _rx_get_ang _rx_get_5f32
#define _rx_get_buzzer _rx_get_1b8
#define _rx_get_buzzer_tone_freq _rx_get_2u16
#define _rx_get_buzzer_tone_dur _rx_get_4u16
#define _rx_get_linesense _rx_get_1b8
#define _rx_get_linesense_value _rx_get_2b8
#define _rx_get_lightsense _rx_get_1b8
#define _rx_get_lightsense_value _rx_get_2f32
#define _rx_get_led _rx_get_1b8
#define _rx_get_led_value _rx_get_2b8

#endif
