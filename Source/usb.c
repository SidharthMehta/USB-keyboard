#include "MKL25Z4.h"
#include "usb_config.h"
#include "buffer_descriptor.h"
#include "descriptor.h"
#include "endpoint.h"
#include "LEDs.h"
#include <string.h>

// USB doesn't have a dedicted memory block as a result we have to provide it. Pg. 616 of reference manual for more details
__attribute((aligned(512))) buffer_descriptor_t buffer_descriptor_table[BD_NUM];

static uint8_t endpoint_0_rx_buf[2][ENDPOINT0_SIZE];

/*
Each USB device has a number of endpoints. Each endpoint is a source or sink of data. A device can have up to 16 OUT and 16 IN endpoints.
OUT always means from host to device.
IN always means from device to host.
Endpoint 0 is a special case which is a combination of endpoint 0 OUT and endpoint 0 IN, and is used for controlling the device. (Control Transfers)
*/
static endpoint_t endpoint0 =
	{
		.rx_buffer_even = endpoint_0_rx_buf[0],
		.rx_buffer_odd = endpoint_0_rx_buf[1],
		.tx_buffer_even = NULL,
		.tx_buffer_odd = NULL,
		.tx_buffer = EVEN,
		.tx_data01_field = DATA0,
		.tx_data = NULL,
		.buffer_size = ENDPOINT0_SIZE,
		.handler = usb0_endpoint0_handler};

// Endpoint is a source or sink of data
endpoint_t *endpoints[NUM_ENDPOINTS] =
	{
		&endpoint0,
		NULL};

void USB_SetEndpoint(uint8_t index, endpoint_t *endpoint)
{
	if (index > 0 && index < NUM_ENDPOINTS)
	{
		endpoints[index] = endpoint;
		init_buffer_descriptor(index);
	}
}

void buffer_descriptor_rx_release(buffer_descriptor_t *bd, uint8_t bufferSize)
{
	uint8_t data = (bd->field & BD_DATA1_MASK) ? 1 : 0;
	// This method releases control of USB
	bd->field = BD_SET_FIELD(bufferSize, data);
}

void USB0_IRQHandler(void)
{
	uint8_t status;

	status = USB0->ISTAT;
	/*
	When the host wants to start communicating with a device it will start by applying a 'Reset' condition which sets the device to its default unconfigured state.
	The Reset condition involves the host pulling down both data lines to low levels (SE0) for at least 10 ms. The device may recognise the reset condition after 2.5 us.
	This 'Reset' should not be confused with a micro-controller power-on type reset. It is a USB protocol reset to ensure that the device USB signaling starts from a known state.
	*/
	if (status & USB_ISTAT_USBRST_MASK)
	{
		// Read KQRUG flowchart
		// Reset all endpoints
		USB0->CTL |= USB_CTL_ODDRST_MASK;

		// Configure endpoint 0
		init_buffer_descriptor(0);

		// After reset, we are address 0, per USB spec
		USB0->ADDR = 0;

		// Clear interrupts
		USB0->ISTAT = 0xff;
		USB0->ERRSTAT = 0xff;

		// all necessary interrupts are now active
		USB0->ERREN = 0xFF;
		USB0->INTEN = USB_INTEN_USBRSTEN_MASK | USB_INTEN_ERROREN_MASK |
					  USB_INTEN_SOFTOKEN_MASK | USB_INTEN_TOKDNEEN_MASK | USB_INTEN_SLEEPEN_MASK | USB_INTEN_STALLEN_MASK;
		return;
	}
	if (status & USB_ISTAT_ERROR_MASK)
	{
		//handle error
		uint8_t error_src = USB0->ERRSTAT;
		USB0->ERRSTAT = error_src;
		USB0->ISTAT = USB_ISTAT_ERROR_MASK;
	}
	if (status & USB_ISTAT_SOFTOK_MASK)
	{
		//handle start of frame token
		USB0->ISTAT = USB_ISTAT_SOFTOK_MASK;
	}
	/*
	This bit is set when the current token being processed has completed. 
	The processor must immediately read the STATUS (STAT) register to determine the EndPoint and BD used for this token. 
	Clearing this bit (by writing a one) causes STAT to be cleared or the STAT holding register to be loaded into the STAT register.
	*/
	if (status & USB_ISTAT_TOKDNE_MASK)
	{
		// Status register contains the endpoint number, direction and buffer
		uint8_t stat = USB0->STAT;
		uint8_t endpoint_number = stat >> 4;
		uint8_t direction = (stat & USB_STAT_TX_MASK) >> USB_STAT_TX_SHIFT;
		uint8_t destination = (stat & USB_STAT_ODD_MASK) >> USB_STAT_ODD_SHIFT;

		// Find buffer descriptor corresponding to endpoint number, direction and buffer
		buffer_descriptor_t *bd = &buffer_descriptor_table[BDT_INDEX(endpoint_number, direction, destination)];
		uint8_t token_pid = BD_GET_TOKEN(bd->field);
		endpoint_t *ep = endpoints[endpoint_number];

		// Call handler corresponding to endpoint
		if (ep->handler != NULL)
		{
			ep->handler(endpoint_number, token_pid, bd);
		}

		if (direction == RX)
		{
			buffer_descriptor_rx_release(bd, ep->buffer_size);
		}

		USB0->ISTAT = USB_ISTAT_TOKDNE_MASK;
	}
	if (status & USB_ISTAT_SLEEP_MASK)
	{
		//handle USB sleep
		USB0->ISTAT = USB_ISTAT_SLEEP_MASK;
	}
	if (status & USB_ISTAT_STALL_MASK)
	{
		//handle usb stall
		USB0->ISTAT = USB_ISTAT_STALL_MASK;
	}
}

void usb_endpoint_prepare_transmit(uint8_t endpoint_number)
{
	uint32_t tx_length;
	endpoint_t *ep = endpoints[endpoint_number];
	buffer_descriptor_t *bd = &buffer_descriptor_table[BDT_INDEX(endpoint_number, TX, ep->tx_buffer)];
	tx_length = ep->tx_data_length;

	if (tx_length > ep->buffer_size)
	{
		tx_length = ep->buffer_size;
	}

	bd->address = ep->tx_data;
	bd->field = BD_SET_FIELD(tx_length, ep->tx_data01_field);

	ep->tx_data += tx_length;
	ep->tx_data_length -= tx_length;

	//toggle the odd and data bits
	ep->tx_buffer ^= 1;
	ep->tx_data01_field ^= 1;
}

static void usb0_endpoint0_handler(uint8_t endpoint_number, uint8_t token_pid, buffer_descriptor_t *bd)
{
	// See USB_20 Spec pg. 248 for contents of setup packet
	static usb_setup_packet_t setup;
	endpoint_t *ep = endpoints[endpoint_number];

	// Endpoint 0 handles control transfers SETUP STAGE uses DATA0.
	switch (token_pid)
	{
	case PID_SETUP:
		// Extract the setup packet. This defines the request, and specifies whether and how much data should be transferred in the DATA stage.
		memcpy((void *)&setup, bd->address, sizeof(usb_setup_packet_t));

		//clear any pending IN stuff
		buffer_descriptor_table[BDT_INDEX(0, TX, EVEN)].field = 0;
		buffer_descriptor_table[BDT_INDEX(0, TX, ODD)].field = 0;
		// Immediately following a SETUP token, the next byte of stage transmitted must be a DATA1.
		ep->tx_data01_field = DATA1;

		// SEND and STALL are handshake signals
		if (usb_endp_setup_handler(0, &setup) == SEND)
		{
			/* truncate data length */
			if (ep->tx_data_length > setup.wLength)
			{
				ep->tx_data_length = setup.wLength;
			}
			/*prepare two buffers with the answer*/
			// This facilitates DATA STAGE of control read
			usb_endpoint_prepare_transmit(0);
			if (ep->tx_data_length > 0)
			{
				usb_endpoint_prepare_transmit(0);
			}
		}
		else
		{
			// Status response STALL
			USB0_ENDPT0 = USB_ENDPT_EPSTALL_MASK | USB_ENDPT_EPRXEN_MASK |
						  USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPHSHK_MASK;
		}
		USB0->CTL = USB_CTL_USBENSOFEN_MASK;
		break;
	/*
	The DATA stage is optional. If present, it always starts with a transaction containing a DATA1. 
	The type of transaction then alternates between DATA0 and DATA1 until all the required data has been transferred.
	*/
	/*
	The STATUS stage is a transaction containing a zero-length DATA1 packet. If the DATA stage was IN then the STATUS stage is OUT, and vice versa.
	*/
	case PID_IN:
		if (ep->tx_data_length > 0)
		{
			usb_endpoint_prepare_transmit(0);
		}
		if (setup.request.wRequestAndType == 0x0500)
		{
			// SET_ADDRESS is a simple, outward direction request in a control transfer with no data stage. The only useful information carried in the SETUP packet is the required address.
			USB0->ADDR = setup.wValue;
		}
		break;
	case PID_OUT:
		break;
	case PID_SOF:
		break;
	}
}

static enum setup_state usb_endp_setup_handler(uint8_t endpoint_number, usb_setup_packet_t *packet)
{
	endpoint_t *ep = endpoints[endpoint_number];

	switch (packet->request.wRequestAndType)
	{
	case 0xA101: // GET_REPORT
		break;
	case 0xA102: // GET_IDLE
		break;
	case 0xA103: // GET_PROTOCOL
		break;
	case 0x2109: // SET_REPORT
		break;
	case 0x210A: // SET_IDLE
		break;
	case 0x210B: //SET_PROTOCOL
		break;
	case 0x0500: //set address (wait for IN packet)
		break;
	case 0x0900: //set configuration
		//we only have one configuration at this time
		break;
	case 0x0680: //get descriptor
	case 0x0681:
		if ((packet->wValue & (USB_DESCRIPTOR_TYPE_STRING << 8)) == (USB_DESCRIPTOR_TYPE_STRING << 8))
		{
			/*
			There are several strings which a host may request. The strings defined in the device descriptor are:
			Manufacturer String
			Product String
			Serial Number String
			These strings are optional. If not supported, the corresponding index in the device descriptor will be 0. Otherwise the host may use the specified index in a Get Descriptor (String) request to fetch the descriptor.
			*/
			uint8_t index = packet->wValue & 0x00FF;
			if (index < STRING_DESCRIPTORS)
			{
				ep->tx_data = (uint8_t *)string_descriptors[index].descriptor;
				ep->tx_data_length = string_descriptors[index].length;
			}
		}
		else if (packet->wValue == (USB_DESCRIPTOR_TYPE_DEVICE << 8))
		{
			ep->tx_data = (uint8_t *)&device_descriptor;
			ep->tx_data_length = sizeof(device_descriptor);
		}
		else if (packet->wValue == (USB_DESCRIPTOR_TYPE_CONFIGURATION << 8))
		{
			// The Get Descriptor (Configuration) warrants special explanation, because the request results in not just a Configuration Descriptor being returned, but also some or all of a number of other descriptors
			ep->tx_data = (uint8_t *)&configuration;
			ep->tx_data_length = sizeof(configuration);
		}
		else if (packet->wValue == (USB_DESCRIPTOR_TYPE_HID_REPORT << 8))
		{
			ep->tx_data = (uint8_t *)&report_descriptor;
			ep->tx_data_length = sizeof(report_descriptor);
		}
		else
		{
			return STALL;
		}
		break;
	default:
		return STALL;
	}
	return SEND;
}

static void init_buffer_descriptor(uint8_t endpoint_number)
{
	endpoint_t *ep = endpoints[endpoint_number];
	uint8_t buffer_size = ep->buffer_size;
	ep->tx_buffer = EVEN;
	ep->tx_data01_field = DATA0;
	buffer_descriptor_table[BDT_INDEX(endpoint_number, RX, EVEN)].field = BD_SET_FIELD(buffer_size, DATA0);
	buffer_descriptor_table[BDT_INDEX(endpoint_number, RX, EVEN)].address = (void *)ep->rx_buffer_even;
	buffer_descriptor_table[BDT_INDEX(endpoint_number, RX, ODD)].field = BD_SET_FIELD(buffer_size, DATA1);
	buffer_descriptor_table[BDT_INDEX(endpoint_number, RX, ODD)].address = (void *)ep->rx_buffer_odd;
	buffer_descriptor_table[BDT_INDEX(endpoint_number, TX, EVEN)].field = 0;
	buffer_descriptor_table[BDT_INDEX(endpoint_number, TX, ODD)].field = 0;
	USB0->ENDPOINT[endpoint_number].ENDPT = USB_ENDPT_EPRXEN_MASK | USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPHSHK_MASK;
}

void usb_init()
{
	// Sequence to initialize USB module as a device as mentioned in Kinetis quick reference guide pg. 134

	// Clock setup - USB module requires reference clock of 48 MHz
	// Clock source for USB - MCGPLLCLK clock with fixed divide by two
	SIM->SOPT2 |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL_MASK;
	// Clock gating
	SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;

	//Reset USB module (software)
	USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
	// Wait two USB clock cycles after setting this bit. This bit is always read as zero.
	while (USB0->USBTRC0 & USB_USBTRC0_USBRESET_MASK)
		;

	// Set BDT base registers KL25Z refernce manual
	USB0->BDTPAGE1 = ((uint32_t)buffer_descriptor_table) >> 8;
	USB0->BDTPAGE2 = ((uint32_t)buffer_descriptor_table) >> 16;
	USB0->BDTPAGE3 = ((uint32_t)buffer_descriptor_table) >> 24;

	// Clear all ISR flags and enable weak pull downs
	USB0->ISTAT = 0xFF;
	USB0->ERRSTAT = 0xFF;
	USB0->OTGISTAT = 0xFF;
	USB0->USBTRC0 |= 0x40; //a hint was given that this is an undocumented interrupt bit

	// Enable USB reset interrupt
	USB0->CTL = USB_CTL_USBENSOFEN_MASK;
	USB0->USBCTRL = USB_USBCTRL_PDE_MASK; // weak pull down
	USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;
	//NVIC_SET_PRIORITY(IRQ(INT_USB0), 112);
	NVIC_EnableIRQ(USB0_IRQn);

	// Enable pull-up resistor on D+ (Full speed, 12Mbit/s)
	USB0->CONTROL = USB_CONTROL_DPPULLUPNONOTG_MASK;
}
