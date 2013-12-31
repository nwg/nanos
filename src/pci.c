#include "pci.h"
#include "asm.h"
#include "endian.h"

void pci_config_write_noswap(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t data) {
    uint32_t address = PCI_CONFIG_ADDRESS_REG(bus, device, function, offset);
    outl(PCI_CONFIG_ADDRESS, tofromlittle32(address));
    outl(PCI_CONFIG_DATA, data);
}

void pci_config_write(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t data) {
    pci_config_write_noswap(bus, device, function, offset, data);
}

uint32_t pci_config_read_noswap(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = PCI_CONFIG_ADDRESS_REG(bus, device, function, offset);
    outl(PCI_CONFIG_ADDRESS, tofromlittle32(address));
    uint32_t result = inl(PCI_CONFIG_DATA);
    return result;
}

uint32_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    return tofromlittle32(pci_config_read_noswap(bus, device, function, offset));
}

void pci_config_read_header(uint8_t bus, uint8_t device, uint8_t function, pci_config_header_t *header) {
    uint32_t *header32 = (uint32_t*)header;

    for (int i = 0; i < 4; i++) {
        header32[i] = pci_config_read_noswap(bus, device, function, i*sizeof(uint32_t));
    }

    tofromlittle_pci_config_header(header);
}

uint16_t pci_config_read_vendor(uint8_t bus, uint8_t device, uint8_t function) {
    uint32_t reg = pci_config_read(bus, device, function, 0x00);
    return reg & 0xFFFF;
}

uint16_t pci_config_read_command(uint8_t bus, uint8_t device, uint8_t function) {
    uint32_t reg = pci_config_read(bus, device, function, 0x04);
    return reg & 0xFFFF;
}

void pci_config_write_command(uint8_t bus, uint8_t device, uint8_t function, uint16_t value) {
    uint32_t reg = pci_config_read(bus, device, function, 0x04);
    reg |= value;
    pci_config_write(bus, device, function, 0x04, reg);
}

void pci_scan_device(uint8_t bus, uint8_t device, pci_scan_handler handler) {
    uint16_t vendor;
    pci_device_t dev;
    dev.bus = bus;
    dev.device = device;

    uint8_t function = 0;

    vendor = pci_config_read_vendor(bus, device, function);
    if (vendor == PCI_VENDOR_INVALID) return;

    pci_config_read_header(bus, device, function, &dev.header);

    dev.function = function;
    handler(&dev);

    if (!(dev.header.header_type & PCI_HEADER_IS_MULTIFUNCTION_BIT))
        return;

    for (function = 1; function < 8; function++) {
        vendor = pci_config_read_vendor(bus, device, function);
        if (vendor == PCI_VENDOR_INVALID) continue;
        pci_config_read_header(bus, device, function, &dev.header);
        dev.function = function;
        handler(&dev);
    }
}

void pci_scan(pci_scan_handler f) {
    for (int bus = 0; bus < 256; bus++) {
        for (int device = 0; device < 32; device++) {
            pci_scan_device(bus, device, f);
        }
    }
}
