#include <stdio.h>
#include <stdint.h>
#include "timers.h"
#include "region.h"
#include "profile.h"

volatile unsigned int adx_lost=0, num_lost=0; 
volatile unsigned long profile_samples=0;
unsigned char profiling_enabled = 0;

void Init_Profiling(void) {
	unsigned i;
	
	// Clear region counts
  for (i=0; i<NumProfileRegions; i++) {
	  RegionCount[i]=0;
  }
	
	// Initialize and start timer
	PIT_Init(SAMPLE_FREQ_HZ_TO_TICKS(10000));
	PIT_Start();
}

void Disable_Profiling(void) {
  profiling_enabled = 0;
}

void Enable_Profiling(void) {
  profiling_enabled = 1;
}

void Sort_Profile_Regions(void) {
    unsigned int i, j, temp;

    // Copy unsorted region numbers into table
    for (i = 0; i < NumProfileRegions; i++) {
        SortedRegions[i] = i;
    }
    // Sort those region numbers
    for (i = 0; i < NumProfileRegions; ++i) {
        for (j = i + 1; j < NumProfileRegions; ++j) {
            if (RegionCount[SortedRegions[i]] < RegionCount[SortedRegions[j]]) {
                temp = SortedRegions[i];
                SortedRegions[i] = SortedRegions[j];
                SortedRegions[j] = temp;
            }
        }
    }
}

void Print_Sorted_Profile(void) {
#if 0
	int i;
	printf("%d total samples, %d samples lost (last was 0x%x)\r\n", profile_samples, num_lost, adx_lost);
	for (i=0; i<NumProfileRegions; i++) {
		if (RegionCount[SortedRegions[i]] > 0) // just print out sampled regions
			printf("%d: \t%s\r\n", RegionCount[SortedRegions[i]], RegionTable[SortedRegions[i]].Name);
	}
#endif
}

