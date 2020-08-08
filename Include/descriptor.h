#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include <stdint.h>
#include <stddef.h>

#define STRING_DESCRIPTORS 7
#define REPORT_DESCRIPTORS_KEYBOARD 63
#define REPORT_DESCRIPTORS_MOUSE 50

/* bmRequestType*/
#define USB_REQUEST_TYPE_DIR_MASK (0x80U)
#define USB_REQUEST_TYPE_DIR_SHIFT (7U)
#define USB_REQUEST_TYPE_DIR_OUT (0x00U) // Host to Device
#define USB_REQUEST_TYPE_DIR_IN (0x80U)  // Device to Host

#define USB_REQUEST_TYPE_TYPE_MASK (0x60U)
#define USB_REQUEST_TYPE_TYPE_SHIFT (5U)
#define USB_REQUEST_TYPE_TYPE_STANDARD (0U)
#define USB_REQUEST_TYPE_TYPE_CLASS (0x20U)
#define USB_REQUEST_TYPE_TYPE_VENDOR (0x40U)

#define USB_REQUEST_TYPE_RECIPIENT_MASK (0x1FU)
#define USB_REQUEST_TYPE_RECIPIENT_SHIFT (0U)
#define USB_REQUEST_TYPE_RECIPIENT_DEVICE (0x00U)
#define USB_REQUEST_TYPE_RECIPIENT_INTERFACE (0x01U)
#define USB_REQUEST_TYPE_RECIPIENT_ENDPOINT (0x02U)
#define USB_REQUEST_TYPE_RECIPIENT_OTHER (0x03U)

/* Standard Request Codes: bRequest */
#define USB_REQUEST_STANDARD_GET_STATUS (0U)
#define USB_REQUEST_STANDARD_CLEAR_FEATURE (1U)
#define USB_REQUEST_STANDARD_SET_FEATURE (3U)
#define USB_REQUEST_STANDARD_SET_ADDRESS (5U)
#define USB_REQUEST_STANDARD_GET_DESCRIPTOR (6U)
#define USB_REQUEST_STANDARD_SET_DESCRIPTOR (7U)
#define USB_REQUEST_STANDARD_GET_CONFIGURATION (8U)
#define USB_REQUEST_STANDARD_SET_CONFIGURATION (9U)
#define USB_REQUEST_STANDARD_GET_INTERFACE (10U)
#define USB_REQUEST_STANDARD_SET_INTERFACE (11U)
#define USB_REQUEST_STANDARD_SYNCH_FRAME (12U)

/* 
Class Descriptor Types: wValue's High byte 
The wValue field specifies the descriptor type in the high byte (refer to Table 9-5)
and the descriptor index in the low byte 
*/
#define USB_DESCRIPTOR_TYPE_DEVICE (0x01U)
#define USB_DESCRIPTOR_TYPE_CONFIGURATION (0x02U)
#define USB_DESCRIPTOR_TYPE_STRING (0x03U)
#define USB_DESCRIPTOR_TYPE_INTERFACE (0x04U)
#define USB_DESCRIPTOR_TYPE_ENDPOINT (0x05U)
#define USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER (0x06U)
#define USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION (0x07U)
#define USB_DESCRIPTOR_TYPE_INTERFACE_POWER (0x08U)

/* Standard Feature Selectors*/
#define USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT (0U)
#define USB_REQUEST_STANDARD_FEATURE_SELECTOR_DEVICE_REMOTE_WAKEUP (1U)
#define USB_REQUEST_STANDARD_FEATURE_SELECTOR_DEVICE_TEST_MODE (2U)

/* 
USB standard descriptor configure bmAttributes 
D7 is reserved and must be set to one for historical reasons.
*/
#define USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_D7_MASK (0x80U)
#define USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_D7_SHIFT (7U)

#define USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_SELF_POWERED_MASK (0x40U)
#define USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_SELF_POWERED_SHIFT (6U)

#define USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_REMOTE_WAKEUP_MASK (0x20U)
#define USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_REMOTE_WAKEUP_SHIFT (5U)

/* USB standard descriptor endpoint bEndpointAddress */
#define USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK (0x80U)
#define USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT (7U)
#define USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT (0U)
#define USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN (0x80U)

#define USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK (0x0FU)
#define USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_SHIFT (0U)

/* USB standard descriptor endpoint bmAttributes */
#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK (0x03U)
#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_NUMBER_SHIFT (0U)

#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_SYNC_TYPE_MASK (0x0CU)
#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_SYNC_TYPE_SHIFT (2U)
#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_SYNC_TYPE_NO_SYNC (0x00U)
#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_SYNC_TYPE_ASYNC (0x04U)
#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_SYNC_TYPE_ADAPTIVE (0x08U)
#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_SYNC_TYPE_SYNC (0x0CU)

#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_USAGE_TYPE_MASK (0x30U)
#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_USAGE_TYPE_SHIFT (4U)
#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_USAGE_TYPE_DATA_ENDPOINT (0x00U)
#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_USAGE_TYPE_FEEDBACK_ENDPOINT (0x10U)
#define USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_USAGE_TYPE_IMPLICIT_FEEDBACK_DATA_ENDPOINT (0x20U)

/* USB standard descriptor endpoint wMaxPacketSize */
#define USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK (0x07FFu)
#define USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK (0x1800u)
#define USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_SHIFT (11U)

#define USB_DESCRIPTOR_TYPE_HID (0x21U)
#define USB_DESCRIPTOR_TYPE_HID_REPORT (0x22U)
#define USB_DESCRIPTOR_TYPE_HID_PHYSICAL (0x23U)

typedef struct
{
  union {
    struct
    {
      uint8_t bmRequestType;
      uint8_t bRequest;
    } fields;
    uint16_t wRequestAndType;
  } request;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} usb_setup_packet_t;

/* Universal Serial Bus Specification Revision 2.0 Page 261 */
typedef __packed struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} usb_device_descriptor_t;

/* 
Universal Serial Bus Specification Revision 2.0 Page 264
If a full-speed only device (with a device descriptor version number equal to 0200H) receives a
GetDescriptor() request for a device_qualifier, it must respond with a request error
*/
typedef __packed struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint8_t bNumConfigurations;
    uint8_t bReserved;
} usb_device_qualifier_descriptor_t;

/* 
Universal Serial Bus Specification Revision 2.0 Page 267 
An interface descriptor is always returned as part of a configuration descriptor. Interface descriptors cannot
be directly accessed with a GetDescriptor() or SetDescriptor() request.
*/
typedef __packed struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} usb_interface_descriptor_t;

/* Universal Serial Bus Specification Revision 2.0 Page 264 */
typedef __packed struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} usb_configuration_descriptor_t;

/* 
Universal Serial Bus Specification Revision 2.0 Page 269 
An endpoint descriptor is always returned as part of a configuration descriptor. Endpoint descriptors cannot
be directly accessed with a GetDescriptor() or SetDescriptor() request.
*/
typedef __packed struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} usb_endpoint_descriptor_t;

/* Device Class Definition for HID Version 1.11 Page 22 */
typedef __packed struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdHID;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;
    uint8_t bChildDescriptorType;
    uint16_t wDescriptorLength;
} usb_hid_descriptor_t;

typedef __packed struct
{
    usb_configuration_descriptor_t configuration_descriptor;
    usb_interface_descriptor_t interface_descriptor_1;
    usb_hid_descriptor_t hid_descriptor_1;
    usb_endpoint_descriptor_t endpoint_descriptor_1;
	  usb_interface_descriptor_t interface_descriptor_2;
    usb_hid_descriptor_t hid_descriptor_2;
    usb_endpoint_descriptor_t endpoint_descriptor_2;
} usb_configuration_t;

typedef struct
{
    void *descriptor;
    uint16_t length;
} descriptor_entry_t;

extern const descriptor_entry_t string_descriptors[STRING_DESCRIPTORS];
extern const usb_device_descriptor_t device_descriptor;
extern const usb_configuration_t configuration;
extern const uint8_t report_descriptor_keyboard[REPORT_DESCRIPTORS_KEYBOARD];
extern const uint8_t report_descriptor_mouse[REPORT_DESCRIPTORS_MOUSE];
#endif
