#ifndef __PCI_NANOS_H__
#define __PCI_NANOS_H__

#include "ll.h"
#include "pci.h"

extern node_t *g_pci_devices;

void pci_init();
void pci_print_bus();

typedef bool (*pci_device_predicate)(pci_device_t *device, void *ctx);
pci_device_t *pci_find_device(pci_device_predicate p, void *ctx);

#endif
