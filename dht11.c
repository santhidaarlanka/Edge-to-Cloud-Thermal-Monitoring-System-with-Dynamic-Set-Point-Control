#include <lpc214x.h>
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "dht11.h"

#define input 0
#define output 1

#define DHT11 4	// DHT11 data pin connected to P0.4

void dht11_request(void)
{
	WRITEBIT(IODIR0, DHT11, output);	// Configure DHT11 pin as output (P0.4)
	WRITEBIT(IOCLR0, DHT11, 1);       // Pull LOW for 20ms
	delay_ms(20);
	WRITEBIT(IOSET0, DHT11, 1);       // Pull HIGH and wait for sensor response
}

void dht11_response(void)
{
	u16 timeout = 0;
	WRITEBIT(IODIR0, DHT11, input);	// Configure DHT11 pin as input 
	while(READBIT(IOPIN0, DHT11) == 1 && ++timeout < 10000); // Wait till response starts
	timeout = 0;
	while(READBIT(IOPIN0, DHT11) == 0 && ++timeout < 10000); // Wait till response is LOW 
	timeout = 0;
	while(READBIT(IOPIN0, DHT11) == 1 && ++timeout < 10000); // Wait till response is HIGH (End of response)
}

unsigned char dht11_data(void)
{
	unsigned char count;
	unsigned char data = 0;
	u16 timeout;
	for(count = 0; count < 8; count++)	// 8 bits of data 
	{
		timeout = 0;
		while(READBIT(IOPIN0, DHT11) == 0 && ++timeout < 10000);	// Wait till LOW period finishes
		delay_us(30);	                    // Wait >24us to sample 0 vs 1
		if (READBIT(IOPIN0, DHT11))         // If HIGH after 30us, bit is 1
			data = ((data << 1) | 0x01);
		else	                            // Otherwise bit is 0
			data = (data << 1);
		timeout = 0;
		while(READBIT(IOPIN0, DHT11) && ++timeout < 10000);	    // Wait for bit HIGH period to end
	}
	return data;
}

u8 dht11_read_all(u8 *hum_int, u8 *hum_dec, u8 *temp_int, u8 *temp_dec, u8 *checksum)
{
	u8 calc_sum;

	dht11_request();
	dht11_response();

	*hum_int  = dht11_data(); // Humidity Integer
	*hum_dec  = dht11_data(); // Humidity Decimal
	*temp_int = dht11_data(); // Temperature Integer
	*temp_dec = dht11_data(); // Temperature Decimal
	*checksum = dht11_data(); // Checksum Byte

	// Checksum is the 8-bit sum of the first 4 data bytes
	calc_sum = (u8)(*hum_int + *hum_dec + *temp_int + *temp_dec);

	// Validate checksum match and realistic sensor limits
	if (calc_sum == *checksum && *temp_int < 80 && *hum_int <= 100 && *checksum != 0)
	{
		return 1; // Checksum Valid & Data OK
	}
	return 0; // Checksum Error / Sensor Disconnected
}
