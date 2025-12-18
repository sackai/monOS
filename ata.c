#include "ata.h"
#include "port_io.h"
#include "console.h"

#define ATA_PRIMARY_IO     0x1F0
#define ATA_PRIMARY_CTRL   0x3F6

static int ata_wait_bsy() {
    while (inb(ATA_PRIMARY_IO + 7) & 0x80);
    return 1;
}

static int ata_wait_drq() {
    while (!(inb(ATA_PRIMARY_IO + 7) & 0x08));
    return 1;
}

void ata_init() {
    kprint("[ATA] Initialized.\n");
}

int ata_read_sector(uint32_t lba, uint8_t* buffer) {

    outb(ATA_PRIMARY_CTRL, 0);

    ata_wait_bsy();

    outb(ATA_PRIMARY_IO + 6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_IO + 2, 1);
    outb(ATA_PRIMARY_IO + 3, (uint8_t) lba);
    outb(ATA_PRIMARY_IO + 4, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_IO + 5, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_IO + 7, 0x20);

    ata_wait_bsy();
    ata_wait_drq();

    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_PRIMARY_IO);
        buffer[i * 2]     = data & 0xFF;
        buffer[i * 2 + 1] = data >> 8;
    }

    return 1;
}

int ata_write_sector(uint32_t lba, uint8_t* buffer) {

    outb(ATA_PRIMARY_CTRL, 0);

    ata_wait_bsy();

    outb(ATA_PRIMARY_IO + 6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_IO + 2, 1);
    outb(ATA_PRIMARY_IO + 3, (uint8_t) lba);
    outb(ATA_PRIMARY_IO + 4, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_IO + 5, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_IO + 7, 0x30);

    ata_wait_bsy();
    ata_wait_drq();

    for (int i = 0; i < 256; i++) {
        uint16_t data = buffer[i * 2] | (buffer[i * 2 + 1] << 8);
        outw(ATA_PRIMARY_IO, data);
    }

    outb(ATA_PRIMARY_IO + 7, 0xE7);

    return 1;
}
