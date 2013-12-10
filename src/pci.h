#ifndef __PCI_H__
#define __PCI_H__

#include <stdint.h>
#include "binary.h"
#include "endian.h"

#define PCI_ADDRESS_ENABLE 0x80000000
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_VENDOR_INVALID 0xFFFF

enum {
    PCI_HEADER_TYPE_DEVICE = 0x00,
    PCI_HEADER_TYPE_BRIDGE_PCI = 0x01,
    PCI_HEADER_TYPE_BRIDGE_CARDBUS = 0x02,

    PCI_HEADER_IS_MULTIFUNCTION_BIT = 0x80,
};
typedef uint8_t pci_header_type_e;

#define PCI_HEADER_TYPE_DEVICE 0x00
#define PCI_HEADER_TYPE_BRIDGE_PCI 0x01
#define PCI_HEADER_TYPE_BRIDGE_CARDBUS 0x02

#define PCI_HEADER_BAR0 0x10
#define PCI_HEADER_BAR1 0x14
#define PCI_HEADER_BAR2 0x18
#define PCI_HEADER_BAR3 0x1C
#define PCI_HEADER_BAR4 0x20
#define PCI_HEADER_BAR5 0x24

// #define PCI_HEADER_IS_MULTIFUNCTION_BIT ((uint8_t)0x80)

void pci_config_write(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t data);
uint32_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
#define pci_config_read_bar0(bus, device, function) pci_config_read(bus, device, function, PCI_HEADER_BAR0)
#define pci_config_read_bar1(bus, device, function) pci_config_read(bus, device, function, PCI_HEADER_BAR1)
#define pci_config_read_bar2(bus, device, function) pci_config_read(bus, device, function, PCI_HEADER_BAR2)
#define pci_config_read_bar3(bus, device, function) pci_config_read(bus, device, function, PCI_HEADER_BAR3)
#define pci_config_read_bar4(bus, device, function) pci_config_read(bus, device, function, PCI_HEADER_BAR4)
#define pci_config_read_bar5(bus, device, function) pci_config_read(bus, device, function, PCI_HEADER_BAR5)

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    pci_header_type_e header_type;
    uint8_t BIST;
} pci_config_header_t;

typedef struct {
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    pci_config_header_t header;
} pci_device_t;

static inline void swap_pci_config_header(pci_config_header_t *header) {
    inplace_swap16(&header->vendor_id);
    inplace_swap16(&header->device_id);
    inplace_swap16(&header->command);
    inplace_swap16(&header->status);
}

static inline void tofromlittle_pci_config_header(pci_config_header_t *header) {
    if (BIG_ENDIAN) swap_pci_config_header(header);
}

#define PCI_CONFIG_ADDRESS_REG(bus, device, function, offset) \
    (PCI_ADDRESS_ENABLE | (bus << 16) | (device << 11) | (function << 8) | offset)

typedef void (*pci_scan_handler)(pci_device_t *device);
void pci_scan(pci_scan_handler f);

#endif
