#include <stdio.h>
#include <stdint.h>

typedef struct { char* c; char b; } a;

struct DCI_S {
  /// type = 0 => DCI Format 0, type = 1 => DCI Format 1A
  uint32_t type:1;
  /// Hopping flag
  uint32_t hopping:1;
  /// RB Assignment (ceil(log2(N_RB_UL*(N_RB_UL+1)/2)) bits)
  uint32_t rballoc:9;
  /// Modulation and Coding Scheme and Redundancy Version
  uint32_t mcs:5;
  /// New Data Indicator
  uint32_t ndi:1;
  /// Power Control
  uint32_t TPC:2;
  /// Cyclic shift
  uint32_t cshift:3;
  /// DAI (TDD)
  uint32_t ulindex:2;
  /// CQI Request
  uint32_t cqi_req:1;
  /// Padding to get to size of DCI1A
  uint32_t padding:2;
} __attribute__ ((__packed__));

int main()
{
    printf("sizeof(a) == %d\n", sizeof(a));
	struct DCI_S DCI;
	printf("DCI0_5MHz_TDD0:%d\n", sizeof(DCI));
}
