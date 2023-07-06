#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX2(x,y) ( \
    { __auto_type __x = (x); __auto_type __y = (y); \
      __x > __y ? __x : __y; })
#define MAX(x, y) (((x) > (y)) ? (x) : (y))  
float calculate_air_time( 	unsigned int sf, 
							unsigned int bw, 
							unsigned int explicitHeader, 
							unsigned int lowDrOptimize,
							unsigned int codingRate, 
							unsigned int preambleLength, 
							unsigned int size ) 
{
	float tSym;
	float tPreamble;
	unsigned int h;
	unsigned int de;
	unsigned int cr;
	float payloadSymbNb;
	float tPayload;
	float air_time;
	
	tSym = (pow(2, sf) / (bw * 1000) )* 1000;
	printf("tSym %f\n", tSym );
	
	tPreamble = (preambleLength + 4.25f) * tSym;
	printf("tPreamble %f\n", tPreamble );
	
	h = explicitHeader ? 0: 1; 
	printf("explicitHeader %d\n", h );
	
	de = ((lowDrOptimize == 1 && bw == 125 && sf >= 11 ) || lowDrOptimize == 1 ) ? 1 : 0;
	printf("lowDrOptimize %d\n", de );
	
	cr = codingRate-4;
	printf("codingRate %d\n", cr );
	
	payloadSymbNb = 8 + MAX(ceil((float)( (8 * size)-(4 * sf)+ 28 + 16 - (20 * h)) / (4 * (sf - (2 * de)))) * (cr + 4), 0);
	printf("payloadSymbNb %f\n", payloadSymbNb );
    
	tPayload = payloadSymbNb * tSym;
    air_time = (tPreamble + tPayload);
    
	return air_time;
}


#define SPREADING_FACTOR 		7
#define BANDWIDTH_KHZ 			125
#define EXPLICIT_HEADER 		1
#define LOWDR_OPTIMIZE 			0 	/* 0 -> SF7 ~SF10; 1-> SF11 e SF12 */
/*
 * @param codingRate coding rate, '4/5', '4/6', '4/7' or '4/8'. For LoRaWAN
 * this is always '4/5'. 
*/
#define CODING_RATE 			5 	/* 4/5 */
#define PREAMBLE_LENGTH_BYTES 	8
/*
*   @param size full packet size. For LoRaWAN this includes the LoRaWAN MAC
*   header (about 9 bytes when no MAC commands are included), the application
*   payload, and the MIC (4 bytes).
*/
#define OVERHEAD_MAC_BYTES  	9+4  
#define PAYLOAD_LENGTH_BYTES 	33


int main( void )
{
	/* airtime in ms */
 	printf("airtime in s = %f\n\n", calculate_air_time(
										SPREADING_FACTOR,
										BANDWIDTH_KHZ,
										EXPLICIT_HEADER,
										LOWDR_OPTIMIZE,
										CODING_RATE,
										PREAMBLE_LENGTH_BYTES, 
										(OVERHEAD_MAC_BYTES + PAYLOAD_LENGTH_BYTES)
									)
										/1000 //Converte para ms;
									);

	/* 1% max duty cycle */
 	printf("\nmax duty cycle in s = %f\n\n", calculate_air_time(
										SPREADING_FACTOR,
										BANDWIDTH_KHZ,
										EXPLICIT_HEADER,
										LOWDR_OPTIMIZE,
										CODING_RATE,
										PREAMBLE_LENGTH_BYTES, 
										(OVERHEAD_MAC_BYTES + PAYLOAD_LENGTH_BYTES)
									)
										/1000 *100 //Converte para ms;
									);

	{
		/* ttn fair access policy */
		float airtime  = calculate_air_time(
											SPREADING_FACTOR,
											BANDWIDTH_KHZ,
											EXPLICIT_HEADER,
											LOWDR_OPTIMIZE,
											CODING_RATE,
											PREAMBLE_LENGTH_BYTES, 
											(OVERHEAD_MAC_BYTES + PAYLOAD_LENGTH_BYTES)
										); //em segundos;
		
		// Maximum messages/day for 30 seconds TTN Fair Access Policy								
		float ttn_fap_messages = 30000 / airtime;
	  	float ttn_fap_delay = (24 * 3600) / ttn_fap_messages;
	  	
	 	printf("\nfair access policy in s= %f\n\n", ttn_fap_delay);		
	}

}
