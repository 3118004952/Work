#ifndef __zigbee_H
#define __zigbee_H

#include "system.h"

#include "stdio.h" 

extern u8 decode_flag;
extern u8 code[100];
extern u8 code_len;
extern u8 code_num;


void receive_data(u8 rec);

void zigbee_send(u8 addr1, u8 addr2, u8 *data);


#endif
