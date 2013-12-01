#include "pci_nanos.h"
#include "kmem.h"
#include <string.h>
#include "kernel.h"
#include "pci.h"

void add_devices();

node_t *g_pci_devices = NULL;

void pci_init() {
    g_pci_devices = ll_alloc_a(kalloc);
    add_devices();

    pci_print_bus();
}

void add_device(pci_device_t *device) {
    pci_device_t *data = kalloc(sizeof(pci_device_t));
    memcpy(data, device, sizeof(pci_device_t));
    ll_append_data_a(kalloc, g_pci_devices, data);
}

void add_devices() {
    pci_scan(add_device);
}

void print_device(node_t *node) {
    pci_device_t *device = (pci_device_t*)node->data;
    kprintf("%x %x %x %x %x\n", device->bus, device->device, device->function, device->header.vendor_id, device->header.device_id);
}

void pci_print_bus() {
    ll_foreach(g_pci_devices, print_device);
}
