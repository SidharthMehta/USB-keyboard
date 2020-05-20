#ifndef BUFFER_DESCRIPTOR_H
#define BUFFER_DESCRIPTOR_H

#include <stdint.h>

typedef struct
{
    uint32_t field;
    void *address;
} buffer_descriptor_t;

#define BD_BC_SHIFT     (16)
#define BD_OWN_MASK     (1 << 7)
#define BD_DATA1_MASK   (1 << 6)
#define BD_KEEP_MASK    (1 << 5)
#define BD_NINC_MASK    (1 << 4)
#define BD_DTS_MASK     (1 << 3)
#define BD_STALL_MASK   (1 << 2)
#define BD_TOKEN_MASK   (0xF << 2)
#define BD_TOKEN_SHIFT  (2)

#define BD_GET_TOKEN(field) ((field & BD_TOKEN_MASK) >> BD_TOKEN_SHIFT)
#define BD_SET_FIELD(count, data1) ((count << BD_BC_SHIFT) | BD_OWN_MASK | (data1 ? BD_DATA1_MASK : 0x00) | BD_DTS_MASK)
#define BDT_INDEX(endpoint, direction, destination) ((endpoint << 2) | (direction << 1) | destination)

#endif
