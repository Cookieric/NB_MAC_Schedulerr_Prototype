///  DCI Format Type N0
struct DCIN0 {
  /// type = 0 => DCI Format N0, type = 1 => DCI Format N1
  uint32_t type:1;
  /// Subcarrier indication
  uint32_t scind:6;
  /// Resourse Assignment (RU Assignment)
  uint32_t ResAssign:3;
  /// Modulation and Coding Scheme and Redundancy Version
  uint32_t mcs:4;
  /// New Data Indicator
  uint32_t ndi:1;
  /// Scheduling Delay
  uint32_t Scheddly:2;
  /// Repetition Number
  uint32_t RepNum:3;
  /// Redundancy version for HARQ (only use 0 and 2)
  uint32_t rv:1;
  /// DCI subframe repetition Number
  uint32_t DCIRep:2;
} __attribute__ ((__packed__));

typedef struct DCIN0 DCIN0_t;
#define sizeof_DCIN0_t 23

