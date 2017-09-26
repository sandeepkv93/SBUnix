#include <sys/ahci.h>
#include <sys/kprintf.h>
#include <sys/pci.h>
#define AHCI_BASE 0x400000
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX 0x35
#define AHCI_BUFF 0x410000
#define TRUE 1
#define FALSE 0
static int
ahci_get_signature(hba_port_t* port)
{
    // Check port status values
    uint8_t ipm = (port->ssts >> 4) & 0x0F;
    uint8_t spd = (port->ssts >> 8) & 0x0F;
    uint8_t det = port->ssts & 0x0F;

    if (!det || !spd || !ipm) {
        // Device not connected
        return 0;
    }

    return (port->sig);
}

void
ahci_setup(hba_mem_t* abar)
{
    abar->ghc |= HBA_GHC_HR;
    abar->ghc |= HBA_GHC_AE;
    abar->ghc |= HBA_GHC_IE;
}

void*
memset(void* s, int c, int n)
{
    unsigned char* p = s;
    while (n--)
        *p++ = (unsigned char)c;
    return s;
}

void
ahci_probe_port(hba_mem_t* abar)
{
    uint32_t pi = abar->pi;
    int i = 0;
    uint8_t* buff = (uint8_t*)(AHCI_BUFF);

    for (i = 0; i < 32; i++) {

        if (!(pi >> i & 0x1)) {
            // Port not implemented
            continue;
        }

        switch (ahci_get_signature(&abar->ports[i])) {
            case AHCI_DEV_SATA:
                kprintf("SATA drive found at port %d\n", i);
                ahci_setup(abar);
                port_rebase(&abar->ports[i], i);
                memset(buff, 8, 512);
                for (int i = 0; i < 10; i++) {
                    kprintf("%d ", buff[i]);
                }
                kprintf("\n");
                write_ahci(&abar->ports[i], 0, 0, 1, (uint16_t*)buff);
                memset(buff, 4, 512);
                for (int i = 0; i < 10; i++) {
                    kprintf("%d ", buff[i]);
                }
                kprintf("\n");
                read_ahci(&abar->ports[i], 0, 0, 1, (uint16_t*)buff);
                for (int i = 0; i < 10; i++) {
                    kprintf("%d ", buff[i]);
                }
                break;
            case AHCI_DEV_SATAPI:
                kprintf("SATAPI drive found at port %d\n", i);
                break;
            case AHCI_DEV_SEMB:
                kprintf("SEMB drive found at port %d\n", i);
                break;
            case AHCI_DEV_PM:
                kprintf("PM drive found at port %d\n", i);
                break;
        }
    }
}

void
ahci_discovery(void)
{
    uint8_t bus;
    uint8_t device;
    uint8_t func;
    uint64_t abar;

    for (bus = 0; bus < 255; bus++) {
        for (device = 0; device < 32; device++) {
            for (func = 0; func < 32; func++) {
                if (pci_class_check(bus, device, func, AHCI_PCI_CLASS)) {
                    // Let's relocate ABAR within 1GB
                    // ABAR is Bar[5] which is located at offset 0x24
                    pci_config_write_dw(bus, device, func, 0x24,
                                        AHCI_PCI_ABAR_LOCATION);
                    abar = pci_config_read_dw(bus, device, func, 0x24);
                    ahci_probe_port((hba_mem_t*)abar);
                }
            }
        }
    }
}

// Start command engine
void
start_cmd(hba_port_t* port)
{
    // Wait until CR (bit15) is cleared
    kprintf("start_cmd before waiting for bit15 clear!\n");
    while (port->cmd & HBA_PxCMD_CR)
        ;
    kprintf("bit15 cleared\n");
    // Set FRE (bit4) and ST (bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}

// Stop command engine
void
stop_cmd(hba_port_t* port)
{
    // Clear ST (bit0)
    kprintf("port->cmd: %d\n", port->cmd);
    port->cmd &= ~HBA_PxCMD_ST;

    // Wait until FR (bit14), CR (bit15) are cleared
    while (1) {
        if (port->cmd & HBA_PxCMD_FR)
            continue;
        if (port->cmd & HBA_PxCMD_CR)
            continue;
        break;
    }

    // Clear FRE (bit4)
    port->cmd &= ~HBA_PxCMD_FRE;
}

void
port_rebase(hba_port_t* port, int portno)
{
    kprintf("Command Engine Stop initiated\n");
    stop_cmd(port); // Stop command engine
    kprintf("Command Engine Stopped\n");
    // Command list offset: 1K*portno
    // Command list entry size = 32
    // Command list entry maxim count = 32
    // Command list maxim size = 32*32 = 1K per port
    port->clb = AHCI_BASE + (portno << 10);
    // port->clbu = 0;
    memset((void*)(port->clb), 0, 1024);

    // FIS offset: 32K+256*portno
    // FIS entry size = 256 bytes per port
    port->fb = AHCI_BASE + (32 << 10) + (portno << 8);
    // port->fbu = 0;
    memset((void*)(port->fb), 0, 256);

    // Command table offset: 40K + 8K*portno
    // Command table size = 256*32 = 8K per port
    hba_cmd_header_t* cmdheader = (hba_cmd_header_t*)(port->clb);
    for (int i = 0; i < 32; i++) {
        cmdheader[i].prdtl = 8; // 8 prdt entries per command table
                                // 256 bytes per command table, 64+16+48+16*8
        // Command table offset: 40K + 8K*portno + cmdheader_index*256
        cmdheader[i].ctba = AHCI_BASE + (40 << 10) + (portno << 13) + (i << 8);
        // cmdheader[i].ctbau = 0;
        memset((void*)cmdheader[i].ctba, 0, 256);
    }

    start_cmd(port); // Start command engine
}
// Find a free command list slot
int
find_cmdslot(hba_port_t* port)
{
    // If not set in SACT and CI, the slot is free
    uint32_t slots = (port->sact | port->ci);
    int i = 0;
    for (i = 0; i < 32; i++) {
        if ((slots & 1) == 0)
            return i;
        slots >>= 1;
    }
    kprintf("Cannot find free command list entry\n");
    return -1;
}

int
read_ahci(hba_port_t* port, uint32_t startl, uint32_t starth, uint32_t count,
          uint16_t* buf)
{
    port->is_rwc = 0xffff;
    int spin = 0; // Spin lock timeout counter
    int slot = find_cmdslot(port);
    int i = 0;
    if (slot == -1)
        return FALSE;

    hba_cmd_header_t* cmdheader = (hba_cmd_header_t*)port->clb;
    cmdheader += slot;
    cmdheader->cfl =
      sizeof(fis_reg_h2d_t) / sizeof(uint32_t);          // Command FIS size
    cmdheader->w = 0;                                    // Read from device
    cmdheader->prdtl = (uint16_t)((count - 1) >> 4) + 1; // PRDT entries count

    hba_cmd_tbl_t* cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
                        (cmdheader->prdtl - 1) * sizeof(hba_prdt_entry_t));

    // 8K bytes (16 sectors) per PRDT
    for (i = 0; i < cmdheader->prdtl - 1; i++) {
        kprintf("lkjdsf");
        cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
        cmdtbl->prdt_entry[i].dbc = 8 * 1024; // 8K bytes
        cmdtbl->prdt_entry[i].i = 1;
        buf += 4 * 1024; // 4K words
        count -= 16;     // 16 sectors
    }
    // Last entry
    cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
    cmdtbl->prdt_entry[i].dbc = count << 9; // 512 bytes per sector
    cmdtbl->prdt_entry[i].i = 1;

    // Setup command
    fis_reg_h2d_t* cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // Command
    cmdfis->command = ATA_CMD_READ_DMA_EX;

    cmdfis->lba0 = (uint8_t)startl;
    cmdfis->lba1 = (uint8_t)(startl >> 8);
    cmdfis->lba2 = (uint8_t)(startl >> 16);
    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->lba3 = (uint8_t)(startl >> 24);
    cmdfis->lba4 = (uint8_t)starth;
    cmdfis->lba5 = (uint8_t)(starth >> 8);

    cmdfis->count = count;

    // The below loop waits until the port is no longer busy before issuing a
    // new command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
        spin++;
    }
    if (spin == 1000000) {
        kprintf("Port is hung\n");
        return FALSE;
    }

    port->ci = 1 << slot; // Issue command

    // Wait for completion
    while (1) {
        // In some longer duration reads, it may be helpful to spin on the DPS
        // bit
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1 << slot)) == 0)
            break;
        if (port->is_rwc & HBA_PxIS_TFES) // Task file error
        {
            kprintf("Read disk error\n");
            return FALSE;
        }
    }

    // Check again
    if (port->is_rwc & HBA_PxIS_TFES) {
        kprintf("Read disk error\n");
        return FALSE;
    }

    return TRUE;
}

int
write_ahci(hba_port_t* port, uint32_t startl, uint32_t starth, uint32_t count,
           uint16_t* buf)
{
    port->is_rwc = (uint32_t)-1; // Clear pending interrupt bits
    int spin = 0;                // Spin lock timeout counter
    int slot = find_cmdslot(port);
    int i = 0;
    if (slot == -1)
        return FALSE;

    hba_cmd_header_t* cmdheader = (hba_cmd_header_t*)port->clb;
    cmdheader += slot;
    cmdheader->cfl =
      sizeof(fis_reg_h2d_t) / sizeof(uint32_t);          // Command FIS size
    cmdheader->w = 1;                                    // Read from device
    cmdheader->prdtl = (uint16_t)((count - 1) >> 4) + 1; // PRDT entries count

    hba_cmd_tbl_t* cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
                        (cmdheader->prdtl - 1) * sizeof(hba_prdt_entry_t));

    // 8K bytes (16 sectors) per PRDT
    for (i = 0; i < cmdheader->prdtl - 1; i++) {
        cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
        cmdtbl->prdt_entry[i].dbc = 8 * 1024; // 8K bytes
        cmdtbl->prdt_entry[i].i = 1;
        buf += 4 * 1024; // 4K words
        count -= 16;     // 16 sectors
    }
    // Last entry
    cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
    cmdtbl->prdt_entry[i].dbc = count << 9; // 512 bytes per sector
    cmdtbl->prdt_entry[i].i = 1;

    // Setup command
    fis_reg_h2d_t* cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // Command
    cmdfis->command = ATA_CMD_WRITE_DMA_EX;

    cmdfis->lba0 = (uint8_t)startl;
    cmdfis->lba1 = (uint8_t)(startl >> 8);
    cmdfis->lba2 = (uint8_t)(startl >> 16);
    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->lba3 = (uint8_t)(startl >> 24);
    cmdfis->lba4 = (uint8_t)starth;
    cmdfis->lba5 = (uint8_t)(starth >> 8);

    cmdfis->count = count;

    // The below loop waits until the port is no longer busy before issuing a
    // new command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
        spin++;
    }
    if (spin == 1000000) {
        kprintf("Port is hung\n");
        return FALSE;
    }

    port->ci = 1 << slot; // Issue command

    // Wait for completion
    while (1) {
        // In some longer duration reads, it may be helpful to spin on the DPS
        // bit
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1 << slot)) == 0)
            break;
        if (port->is_rwc & HBA_PxIS_TFES) // Task file error
        {
            kprintf("Write disk error\n");
            return FALSE;
        }
    }

    // Check again
    if (port->is_rwc & HBA_PxIS_TFES) {
        kprintf("Write disk error\n");
        return FALSE;
    }

    return TRUE;
}
