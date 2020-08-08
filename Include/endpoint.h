#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdint.h>
#include "buffer_descriptor.h"

#define ENDPOINT0_SIZE 64
#define ENDPOINT1_SIZE 8
#define ENDPOINT2_SIZE 3
#define NUM_ENDPOINTS 3
#define BD_NUM (NUM_ENDPOINTS * 4)

enum EpBufferDirection
{
    RX,
    TX
};

enum EpBufferDestination
{
    EVEN,
    ODD
};

enum EpBufferData
{
    DATA0,
    DATA1
};

typedef struct
{
    uint8_t *rx_buffer_odd;
    uint8_t *rx_buffer_even;
    uint8_t *tx_buffer_odd;
    uint8_t *tx_buffer_even;

    enum EpBufferDestination tx_buffer;
    /*
    DATA0 and DATA1 PIDs are used in Low and Full speed links as part of an error-checking system. 
    When used, all data packets on a particular endpoint use an alternating DATA0 / DATA1 so that the endpoint knows if a received packet is the one it is expecting. 
    If it is not it will still acknowledge (ACK) the packet as it is correctly received,
    but will then discard the data, assuming that it has been re-sent because the host missed seeing the ACK the first time it sent the data packet.
    */
    enum EpBufferData tx_data01_field;

    uint8_t *tx_data;
    uint16_t tx_data_length;
    uint8_t buffer_size;

    void (*handler)(uint8_t endpoint_number, uint8_t token_pid, buffer_descriptor_t *bd);
} endpoint_t;

#endif
