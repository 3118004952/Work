#ifndef __zigbee_H
#define __zigbee_H

#include "sys.h" 
#include "stdio.h" 

extern u8 decode_flag;
extern u8 code[100];
extern u8 send_data[100];
extern u8 code_len;
extern u8 code_num;
extern u8 code_timing;
extern u8 online1;
extern u8 online2;

extern u8 rescue[];
extern u8 locate[];

void receive_data(u8 rec);

void decode_send(void);

void zigbee_send(u8 addr1, u8 addr2, u8 *data);

#endif
