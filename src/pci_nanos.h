#ifndef __PCI_NANOS_H__
#define __PCI_NANOS_H__

#include "ll.h"

extern node_t *g_pci_devices;

void pci_init();
void pci_print_bus();

#endif
