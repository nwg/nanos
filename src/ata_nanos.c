#include "ata_nanos.h"
#include "ata.h"
#include "timer.h"
#include <stdio.h>
#include "kernel.h"
#include "pci_nanos.h"
#include "kmem.h"
#include "syscall.h"
#include "interrupt.h"
#include "sizes.h"
#include "asm.h"
#include "schedule.h"
#include <string.h>

ata_ata_cmd_status_t cmd_statuses[NUM_ATA_DRIVES];

static pci_device_t *dev1 = NULL;
static bool use_dma = false;

static bool is_ide_ata_device(pci_device_t *dev, void *ctx);
void configure_mindrvr_ports(pci_device_t *device);
void enable_dma(pci_device_t *device);
bool dma_enabled(pci_device_t *device);

void _MINDRVR_OUTBYTE(uintptr_t addr, uint8_t data) { outb(addr, data); }
void _MINDRVR_OUTWORD(uintptr_t addr, uint16_t data) { outw(addr, data); }
void _MINDRVR_OUTDWORD(uintptr_t addr, uint32_t data) { outl(addr, data); }
uint8_t _MINDRVR_INBYTE(uintptr_t addr) { return inb(addr); }
uint16_t _MINDRVR_INWORD(uintptr_t addr) { return inw(addr); }
uint32_t _MINDRVR_INDWORD(uintptr_t addr) { return inl(addr); }

void ata_nanos_init() {
    enable_irq(14);
    irq_handlers[IRQ14] = ata_nanos_handle_irq_14;

    MINDRVR_OUTBYTE = _MINDRVR_OUTBYTE;
    MINDRVR_OUTWORD = _MINDRVR_OUTWORD;
    MINDRVR_OUTDWORD = _MINDRVR_OUTDWORD;
    MINDRVR_INBYTE = _MINDRVR_INBYTE;
    MINDRVR_INWORD = _MINDRVR_INWORD;
    MINDRVR_INDWORD = _MINDRVR_INDWORD;

    MINDRVR_SYSTEM_TIMER_TICKS_PER_SECOND = TIMER_TPS;

    dev1 = pci_find_device(is_ide_ata_device, NULL);

    if (!dev1) {
        kprintf("ATA: No PCI Devices found\n");
        return;
    }

    enable_dma(dev1);
    if (dma_enabled(dev1)) {
        use_dma = true;
        kprintf("ATA: Busmaster enabled\n");
    }

    configure_mindrvr_ports(dev1);

    if (reg_config_info[0] != REG_CONFIG_TYPE_ATA) {
        kprintf("ATA: No Connected Devices\n");
        return;
    }

    if (reg_reset(0)) {
        kprintf("Error resetting reg\n");
    }

}

void enable_dma(pci_device_t *device) {
    if (! (device->header.prog_if == ATA_BUSMASTER_PROG_IF))
        return;

    uint16_t cmd = pci_config_read_command(device->bus, device->device, device->function);
    cmd |= ATA_PCICMD_BME;
    pci_config_write_command(device->bus, device->device, device->function, cmd);
}

bool dma_enabled(pci_device_t *device) {
    uint16_t cmd = pci_config_read_command(device->bus, device->device, device->function);
    return cmd & ATA_PCICMD_BME;
}

void ata_nanos_handle_irq_14() {
    ata_ata_cmd_status_t *status = &cmd_statuses[ATA_DRIVE_PRIMARY_MASTER];
    if (!status->process) return;

    status->got_irq = true;
    switch_to_process(status->process);
}

int ata_read(ata_drive_e drive, uint64_t lba, void *buf, size_t nbyte) {

    uint32_t lba_hi = lba >> 32;
    uint32_t lba_lo = lba & 0xFFFFFFFF;
    size_t nsector = nbyte >> 9;
    bool fits28 = lba + nsector < ATA_LBA28_MAX;

    kprintf("ATA: Begin read\n");

    if (use_dma) {
        if (fits28) {
            return dma_pci_lba28(0, CMD_READ_DMA, 0, nsector, lba_lo, buf, nsector);
        } else {
            return dma_pci_lba48(0, CMD_READ_DMA, 0, nsector, lba_hi, lba_lo, buf, nsector);
        }
    } else {
        if (fits28) {
            return reg_pio_data_in_lba28(0, CMD_READ_SECTORS, 0, nsector, lba_lo, buf, nsector, 0);
        } else {
            return reg_pio_data_in_lba48(0, CMD_READ_SECTORS, 0, nsector, lba_hi, lba_lo, buf, nsector, 0);
        }
    }
}

int MINDRVR_SYSTEM_WAIT_INTR_OR_TIMEOUT(int device) {
    kprintf("Driver called wait\n");

    process_t *process = current_process();
    ata_drive_e drive = (ata_drive_e)device;
    ata_ata_cmd_status_t *status = &cmd_statuses[drive];
    status->process = process;
    status->got_irq = false;
    status->start_time = g_timer_ticks;

    YIELD();

    bool was_timeout = !status->got_irq;
    memset(status, 0, sizeof(ata_ata_cmd_status_t));

    kprintf("Driver done wait\n");
    return was_timeout;
}

uintmax_t MINDRVR_SYSTEM_READ_TIMER( void ) {
    return g_timer_ticks;
}

static bool is_ide_ata_device(pci_device_t *dev, void *ctx) {
    return dev->header.header_type == PCI_HEADER_TYPE_DEVICE
        && dev->header.class_code == PCI_IDE_ATA_CLASS
        && dev->header.subclass == PCI_IDE_ATA_SUBCLASS
        && ( dev->header.prog_if == PCI_IDE_ATA_PROG_IF_1 || dev->header.prog_if == PCI_IDE_ATA_PROG_IF_2);
}

void configure_mindrvr_ports(pci_device_t *device) {
    uint32_t bmiba = pci_config_read_bar4(device->bus, device->device, device->function);
    bmiba &= PCI_BMIBA_MASK;
    MINDRVR_init(PCI_IDE_COMMAND_1, PCI_IDE_CONTROL_1, bmiba);
}
