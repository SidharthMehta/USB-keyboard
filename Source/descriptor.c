#include "descriptor.h"
#include "endpoint.h"
#define CHAR_TO_UTF16(x) (x), 0x00

const uint8_t language_descriptor[] = {
    4,                          //blength
    USB_DESCRIPTOR_TYPE_STRING, //bDescriptorType
    0x09,                       //wLANGID[0] Lang ID for 0x0409 US English
    0x04};

const uint8_t manufacturer_string_descriptor[] = {
    14,
    USB_DESCRIPTOR_TYPE_STRING,
    CHAR_TO_UTF16('s'),
    CHAR_TO_UTF16('m'),
    CHAR_TO_UTF16('e'),
    CHAR_TO_UTF16('h'),
    CHAR_TO_UTF16('t'),
    CHAR_TO_UTF16('a')};

const uint8_t product_string_descriptor[] = {
    18,
    USB_DESCRIPTOR_TYPE_STRING,
    CHAR_TO_UTF16('k'),
    CHAR_TO_UTF16('e'),
    CHAR_TO_UTF16('y'),
    CHAR_TO_UTF16('m'),
    CHAR_TO_UTF16('o'),
    CHAR_TO_UTF16('u'),
    CHAR_TO_UTF16('s'),
    CHAR_TO_UTF16('e')};

const uint8_t serial_string_descriptor[] = {
    18,
    USB_DESCRIPTOR_TYPE_STRING,
    CHAR_TO_UTF16('0'),
    CHAR_TO_UTF16('0'),
    CHAR_TO_UTF16('0'),
    CHAR_TO_UTF16('0'),
    CHAR_TO_UTF16('0'),
    CHAR_TO_UTF16('0'),
    CHAR_TO_UTF16('1')};

const uint8_t interface_string_descriptor_keyboard[] = {
    18,
    USB_DESCRIPTOR_TYPE_STRING,
    CHAR_TO_UTF16('k'),
    CHAR_TO_UTF16('e'),
    CHAR_TO_UTF16('y'),
    CHAR_TO_UTF16('b'),
    CHAR_TO_UTF16('o'),
    CHAR_TO_UTF16('a'),
    CHAR_TO_UTF16('r'),
    CHAR_TO_UTF16('d')};

const uint8_t interface_string_descriptor_mouse[] = {
    12,
    USB_DESCRIPTOR_TYPE_STRING,
    CHAR_TO_UTF16('m'),
    CHAR_TO_UTF16('o'),
    CHAR_TO_UTF16('u'),
    CHAR_TO_UTF16('s'),
    CHAR_TO_UTF16('e')};

const uint8_t configuration_string_descriptor[] = {
    16,
    USB_DESCRIPTOR_TYPE_STRING,
    CHAR_TO_UTF16('d'),
    CHAR_TO_UTF16('e'),
    CHAR_TO_UTF16('f'),
    CHAR_TO_UTF16('a'),
    CHAR_TO_UTF16('u'),
    CHAR_TO_UTF16('l'),
    CHAR_TO_UTF16('t')};

const usb_device_descriptor_t device_descriptor = {
    .bLength = 18,
    .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
    .bcdUSB = 0x0101,
    .bDeviceClass = 0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = ENDPOINT0_SIZE,
    .idVendor = 0xFFFF,
    .idProduct = 0x0001,
    .bcdDevice = 0x0100,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1};

const uint8_t report_descriptor_mouse[REPORT_DESCRIPTORS_MOUSE] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
		};

const uint8_t report_descriptor_keyboard[REPORT_DESCRIPTORS_KEYBOARD] = {
    0x05, 0x01,
    0x09, 0x06,
    0xA1, 0x01,
    0x05, 0x07,
    0x19, 0xE0,
    0x29, 0xE7,
    0x15, 0x00,
    0x25, 0x01,
    0x75, 0x01,
    0x95, 0x08,
    0x81, 0x02,
    0x95, 0x01,
    0x75, 0x08,
    0x81, 0x01,
    0x95, 0x05,
    0x75, 0x01,
    0x05, 0x08,
    0x19, 0x01,
    0x29, 0x05,
    0x91, 0x02,
    0x95, 0x01,
    0x75, 0x03,
    0x91, 0x01,
    0x95, 0x06,
    0x75, 0x08,
    0x15, 0x00,
    0x25, 0x65,
    0x05, 0x07,
    0x19, 0x00,
    0x29, 0x65,
    0x81, 0x00,
    0xC0};

const usb_configuration_t configuration = {
    .configuration_descriptor = {
        .bLength = 9,
        .bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION,
        .wTotalLength = sizeof(usb_configuration_t),
        .bNumInterfaces = 2,
        .bConfigurationValue = 1,
        .iConfiguration = 6,
        .bmAttributes = 0xA0,
        .bMaxPower = 0xFA},
    .interface_descriptor_1 = {
        .bLength = 9, 
        .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE, 
        .bInterfaceNumber = 0, 
        .bAlternateSetting = 0, 
        .bNumEndpoints = 1, 
        .bInterfaceClass = 3, 
        .bInterfaceSubClass = 0, 
        .bInterfaceProtocol = 0, 
        .iInterface = 4},
    .hid_descriptor_1 = {
        .bLength = 9, 
        .bDescriptorType = USB_DESCRIPTOR_TYPE_HID, 
        .bcdHID = 0x0101, 
        .bCountryCode = 0, 
        .bNumDescriptors = 1, 
        .bChildDescriptorType = USB_DESCRIPTOR_TYPE_HID_REPORT, 
        .wDescriptorLength = sizeof(report_descriptor_keyboard)},
    .endpoint_descriptor_1 = {
        .bLength = 7,
        .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
        // Endpoint 1 - In
        .bEndpointAddress = 0x81,
        // Interrupt
        .bmAttributes = 0x03,
        .wMaxPacketSize = ENDPOINT1_SIZE,
        .bInterval = 0x0A},
		.interface_descriptor_2 = {
        .bLength = 9, 
        .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE, 
        .bInterfaceNumber = 1, 
        .bAlternateSetting = 0, 
        .bNumEndpoints = 1, 
        .bInterfaceClass = 3, 
        .bInterfaceSubClass = 0, 
        .bInterfaceProtocol = 0, 
        .iInterface = 5},
		.hid_descriptor_2 = {
        .bLength = 9, 
        .bDescriptorType = USB_DESCRIPTOR_TYPE_HID, 
        .bcdHID = 0x0101, 
        .bCountryCode = 0, 
        .bNumDescriptors = 1, 
				.bChildDescriptorType = USB_DESCRIPTOR_TYPE_HID_REPORT,
				.wDescriptorLength = sizeof(report_descriptor_mouse)},
    .endpoint_descriptor_2 = {
        .bLength = 7,
        .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
        // Endpoint 2 - In
        .bEndpointAddress = 0x82,
        // Interrupt
        .bmAttributes = 0x03,
        .wMaxPacketSize = ENDPOINT2_SIZE,
        .bInterval = 0x04
    }};

const descriptor_entry_t string_descriptors[STRING_DESCRIPTORS] = {
    {(void *)&language_descriptor, sizeof(language_descriptor)},
    {(void *)&manufacturer_string_descriptor, sizeof(manufacturer_string_descriptor)},
    {(void *)&product_string_descriptor, sizeof(product_string_descriptor)},
    {(void *)&serial_string_descriptor, sizeof(serial_string_descriptor)},
    {(void *)&interface_string_descriptor_keyboard, sizeof(interface_string_descriptor_keyboard)},
		{(void *)&interface_string_descriptor_mouse, sizeof(interface_string_descriptor_mouse)},
    {(void *)&configuration_string_descriptor, sizeof(configuration_string_descriptor)},
};
