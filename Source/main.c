#include "MKL25Z4.h"
#include "usb_config.h"
#include "LEDs.h"
#include "endpoint.h"
#include "buffer_descriptor.h"

extern __attribute((aligned(512))) buffer_descriptor_t buffer_descriptor_table[BD_NUM];
static void endpoint1_handler(uint8_t endpoint_number, uint8_t token_pid, buffer_descriptor_t *bd);
extern endpoint_t *endpoints[NUM_ENDPOINTS];
static uint8_t endpoint_1_rx_buf[2][ENDPOINT1_SIZE];
static uint8_t endpoint_1_tx_buf[2][ENDPOINT1_SIZE];
volatile uint8_t wait = 1;

uint8_t report[8][8] = {
	{0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00}};

static endpoint_t endpoint1 =
	{
		.rx_buffer_even = endpoint_1_rx_buf[0],
		.rx_buffer_odd = endpoint_1_rx_buf[1],
		.tx_buffer_even = endpoint_1_tx_buf[0],
		.tx_buffer_odd = endpoint_1_tx_buf[1],
		.tx_buffer = EVEN,
		.tx_data01_field = DATA0,
		.tx_data = NULL,
		.buffer_size = ENDPOINT1_SIZE,
		.handler = endpoint1_handler};

int main()
{
	uint16_t delay = 0xfF;
	Init_RGB_LEDs();
	Control_RGB_LEDs(0, 0, 0);
	usb_init();
	USB_SetEndpoint(1, &endpoint1);
	
	while(delay--);
	
	while (1)
	{
		int i=0;
		for(i=0; i<8; i++)
		{
			wait = 1;
			endpoint1.tx_data = report[i];
			endpoint1.tx_data_length = ENDPOINT1_SIZE;
			usb_endpoint_prepare_transmit(1);
			while(wait);
		}
	}
	return 1;
}

static void endpoint1_handler(uint8_t endpoint_number, uint8_t token_pid, buffer_descriptor_t *bd)
{
	endpoint_t *ep = endpoints[endpoint_number];
	switch (token_pid)
	{
	case PID_SETUP:
		Control_RGB_LEDs(0, 0, 1);
		break;
	case PID_IN:
		if(ep->tx_data_length > 0)
				usb_endpoint_prepare_transmit(1);
		wait = 0;
		Control_RGB_LEDs(0, 1, 0);
		break;
	case PID_OUT:
		Control_RGB_LEDs(0, 1, 1);
		break;
	case PID_SOF:
		Control_RGB_LEDs(1, 0, 0);
		break;
	default:
		Control_RGB_LEDs(1, 0, 1);
		break;
	}
}
