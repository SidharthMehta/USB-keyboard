#ifndef USB_CONFIG_H
#define USB_CONFIG_H

#include <stdint.h>
#include <stddef.h>
#include "descriptor.h"
#include "endpoint.h"
#include "buffer_descriptor.h"

// Token
#define PID_OUT 0b0001
#define PID_IN 0b1001
#define PID_SOF 0b0101
#define PID_SETUP 0b1101

// Data
#define PID_DATA0 0b0011
#define PID_DATA1 0b1011
#define PID_DATA2 0b0111
#define PID_MDATA 0b1111

// Handshake
#define PID_ACK 0b0010
#define PID_NAK 0b1010
#define PID_STALL 0b1110
#define PID_NYET 0b0110

// Special
#define PID_PRE 0b1100
#define PID_ERR 0b1100
#define PID_SPLIT 0b1000
#define PID_PING 0b0100

enum setup_state {
	SEND,
	STALL
};

void usb_endpoint_prepare_transmit(uint8_t endpoint_number);
static enum setup_state usb_endp_setup_handler(uint8_t endpoint_number, usb_setup_packet_t *packet);
void usb_init(void);
static void init_buffer_descriptor(uint8_t endpoint_number);
static void usb0_endpoint0_handler(uint8_t endpoint_number, uint8_t token_pid, buffer_descriptor_t *bd);
void USB_SetEndpoint(uint8_t index, endpoint_t *endpoint);
void buffer_descriptor_rx_release(buffer_descriptor_t *bd, uint8_t bufferSize);
#endif
