#ifndef _DHT11_H_
#define _DHT11_H_

#include "types.h"

void dht11_request(void);
void dht11_response(void);
unsigned char dht11_data(void);
u8 dht11_read_all(u8 *hum_int, u8 *hum_dec, u8 *temp_int, u8 *temp_dec, u8 *checksum);

#endif
