#ifndef _TYPES_H_
#define _TYPES_H_
#define LOG(...) printf(__VA_ARGS__)

#include <stdint.h>
// typedef unsigned char uint8_t;
// typedef char int8_t;
// typedef unsigned short uint16_t;
// typedef short int16_t;
// typedef unsigned long uint32_t;
// typedef long int32_t;


typedef uint32_t	frame_t;
typedef uint8_t	sub_frame_t;
typedef uint16_t	rnti_t;

typedef enum operationMode{
	inband_SamePCI=0,
	inband_DifferentPCI,
	guardband,
	standalone,

	num_operationMode
}operationMode_e;

typedef enum channel_E{
	NA=0,
	//DL
	NPSS,
	NSSS,
	NPBCH,
	SIB1,
	SIB2_3,
	NPDCCH,
	NPDSCH,
	//UL
	NPRACH,
	NPUSCH,

	L_channels//channels length
}channel_t;


#endif
