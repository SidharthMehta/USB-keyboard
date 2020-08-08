#ifndef PROFILE_H
#define PROFILE_H

#include "system_MKL25Z4.h"

#define RET_ADX_OFFSET (0x18)
#define CUR_FRAME_SIZE (8)  // 0 if var is initialized as first auto var 
#define SAMPLE_FREQ_HZ_TO_TICKS(freq) ((SystemCoreClock/(2*freq))-1)

extern void Init_Profiling(void);

extern void Disable_Profiling(void);
extern void Enable_Profiling(void);
extern void Sort_Profile_Regions(void);

extern void Print_Results(void);
#endif
