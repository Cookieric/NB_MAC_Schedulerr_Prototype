#ifndef _DCI_NB_H_
#define _DCI_NB_H_

///  DCI Format Type 0 (180 kHz, 23 bits)
typedef struct DCIFormatN0{
  /// type = 0 => DCI Format N0, type = 1 => DCI Format N1, 1 bits
  uint8_t type;
  /// Subcarrier indication, 6 bits
  uint8_t scind;
  /// Resourse Assignment (RU Assignment), 3 bits
  uint8_t ResAssign;
  /// Modulation and Coding Scheme, 4 bits
  uint8_t mcs;
  /// New Data Indicator, 1 bits
  uint8_t ndi;
  /// Scheduling Delay, 2 bits
  uint8_t Scheddly;
  /// Repetition Number, 3 bits
  uint8_t RepNum;
  /// Redundancy version for HARQ (only use 0 and 2), 1 bits
  uint8_t rv;
  /// DCI subframe repetition Number, 2 bits
  uint8_t DCIRep;
};

typedef struct DCIFormatN0 DCIFormatN0_t;
#define sizeof_DDCIFormatN0_t 23

///  DCI Format Type N1 for User data
struct DCIFormatN1{
  /// type = 0 => DCI Format N0, type = 1 => DCI Format N1,1bits
  uint8_t type;
  //NPDCCH order indicator (set to 0), 1 bits
  uint8_t orderIndicator;
  // Scheduling Delay,3 bits
  uint8_t Scheddly;
  // Resourse Assignment (RU Assignment),3 bits
  uint8_t ResAssign;
  // Modulation and Coding Scheme,4 bits
  uint8_t mcs;
  // Repetition Number,4 bits
  uint8_t RepNum;
  // New Data Indicator,1 bits
  uint8_t ndi;
  // HARQ-ACK resource,4 bits
  uint8_t HARQackRes;
  // DCI subframe repetition Number,2 bits
  uint8_t DCIRep;
};


typedef struct DCIFormatN1 DCIFormatN1_t;
#define sizeof_DCIFormatN1_t 23

///  DCI Format Type N1 for initial RA
struct DCIFormatN1_RA{
  /// type = 0 => DCI Format N0, type = 1 => DCI Format N1, 1 bits
  uint8_t type;
  //NPDCCH order indicator (set to 0),1 bits
  uint8_t orderIndicator;
  // Start number of NPRACH repetiiton, 2 bits
  uint8_t Scheddly;
  // Subcarrier indication of NPRACH, 6 bits
  uint8_t scind;
  // All the remainging bits, 13 bits
  uint8_t remaingingBits;
};

typedef struct DCIFormatN1_RA DCIFormatN1_RA_t;
#define sizeof_DCIFormatN1_RA_t 23

///  DCI Format Type N1 for RAR
struct DCIFormatN1_RAR{
  /// type = 0 => DCI Format N0, type = 1 => DCI Format N1, 1 bits
  uint8_t type;
  //NPDCCH order indicator (set to 0),1 bits
  uint8_t orderIndicator;
  // Scheduling Delay, 3 bits
  uint8_t Scheddly;
  // Resourse Assignment (RU Assignment), 3 bits
  uint8_t ResAssign;
  // Modulation and Coding Scheme, 4 bits
  uint8_t mcs;
  // Repetition Number, 4 bits
  uint8_t RepNum;
  // Reserved 5 bits
  uint8_t Reserved;
  // DCI subframe repetition Number, 2 bits
  uint8_t DCIRep;
};

typedef struct DCIFormatN1_RAR DCIFormatN1_RAR_t;
#define sizeof_DCIFormatN1_RAR_t 23

//  DCI Format Type N2 for direct indication, 15 bits
struct DCIFormat2_Ind{
  //Flag for paging(1)/direct indication(0), set to 0,1 bits
  uint8_t type;
  //Direct indication information, 8 bits
  uint8_t directIndInf;
  // Reserved information bits, 6 bits
  uint8_t resInfoBits;
};

typedef struct DCIFormat2_Ind DCIFormat2_Ind_t;
#define sizeof_DCIFormat2_Ind_t 15

//  DCI Format Type N2 for Paging, 15 bits
struct DCIFormat2_Pag{
  //Flag for paging(1)/direct indication(0), set to 1,1 bits
  uint8_t type;
  // Resourse Assignment (RU Assignment), 3 bits
  uint8_t ResAssign;
  // Modulation and Coding Scheme, 4 bits
  uint8_t mcs;
  // Repetition Number, 4 bits
  uint8_t RepNum;
  // Reserved 3 bits
  uint8_t DCIRep;
};

typedef struct DCIFormat2_Pag DCIFormat2_Pagt;
#define sizeof_DCIFormat2_Pag_t 15
#endif
