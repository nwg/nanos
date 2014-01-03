#ifndef __ATA_NANOS_H__
#define __ATA_NANOS_H__

// Based on 82371SB PIIX3 IDE (Function 1)
// http://download.intel.com/design/intarch/datashts/29055002.pdf

#include <stdint.h>
#include "file.h"
#include "process.h"
#include "ata.h"

#define PCI_BMIBA_MASK 0xFFF0

#define PCI_IDE_ATA_CLASS 0x01
#define PCI_IDE_ATA_SUBCLASS 0x01
#define PCI_IDE_ATA_PROG_IF_1 0x8A
#define PCI_IDE_ATA_PROG_IF_2 0x80

#define PCI_IDE_COMMAND_1 0x1F0
#define PCI_IDE_CONTROL_1 0x3F6

#define ATA_NANOS_TIMEOUT_US (TMR_TIME_OUT*1000000)

#define ATA_LBA28_MAX (1 << 28)

#define ATA_BUSMASTER_PROG_IF 0x80

typedef struct {
    uintmax_t offset;
    file_t *file;
} disktest_t;

typedef enum {
    ATA_DRIVE_PRIMARY_MASTER,
    ATA_DRIVE_PRIMARY_SLAVE,
    ATA_DRIVE_SECONDARY_MASTER,     // Not implemented
    ATA_DRIVE_SECONDARY_SLAVE,      // Not implemented
    NUM_ATA_DRIVES
} ata_drive_e;

typedef struct {
    process_t *process;
    bool got_irq;
    uint64_t start_time;
} ata_ata_cmd_status_t;

typedef enum {
    ATA_PCICMD_IOSE = 1 << 0,
    ATA_PCICMD_MSE = 1 << 1,
    ATA_PCICMD_BME = 1 << 2,
    ATA_PCICMD_SCE = 1 << 3,
    ATA_PCICMD_MWI = 1 << 4,
    ATA_PCICMD_FBE = 1 << 9,
} ata_pcicmd_e;

extern ata_ata_cmd_status_t cmd_statuses[NUM_ATA_DRIVES];

void ata_nanos_init();

int ata_read(ata_drive_e drive, uint64_t lba, void *buf, size_t nbyte);
void ata_nanos_handle_irq_14();

#endif
