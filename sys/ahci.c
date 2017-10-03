#include <sys/ahci.h>
#include <sys/debug.h>
#include <sys/kprintf.h>
#include <sys/pci.h>
#include <sys/string.h>
#include <sys/timer.h>
#define AHCI_BASE 0x400000
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX 0x35
#define AHCI_BUFF 0x410000
#define SET_DET 0x301
#define RESET_DET 0x300
#define READ 0
#define WRITE 1
#define STAG_SPINUP (HBA_PxCMD_SUD | HBA_PxCMD_POD | HBA_PxCMD_ICC)

hba_wrap_t g_ahci_disk;

bool
ahci_ready_to_go(hba_port_t* port)
{
    uint8_t spd = (port->ssts >> 4) & 0x0F;
    uint8_t ipm = (port->ssts >> 8) & 0x0F;
    uint8_t det = port->ssts & 0x0F;

    if (!det || !spd || !ipm) {
        return FALSE;
    }
    if (det == 3 && ipm == 1) {
        return TRUE;
    }
    return FALSE;
}

int
ahci_get_signature(hba_port_t* port)
{
    uint8_t spd = (port->ssts >> 4) & 0x0F;
    uint8_t ipm = (port->ssts >> 8) & 0x0F;
    uint8_t det = port->ssts & 0x0F;
    if (!det || !spd || !ipm) {
        debug_print("Port sig %x, spd %x, ipm %x, det %x.", port->sig, spd, ipm,
                    det);
        return 0;
    }
    debug_print("Port sig %x, spd %x, ipm %x, det %x.", port->sig, spd, ipm,
                det);
    return (port->sig);
}

void
ahci_fix_port(hba_port_t* port, int port_num, int support_staggered_spinup)
{
    if (port->cmd &
        (HBA_PxCMD_ST | HBA_PxCMD_CR | HBA_PxCMD_FRE | HBA_PxCMD_FR)) {
        port->cmd &= ~HBA_PxCMD_FRE;
        port->cmd &= ~HBA_PxCMD_ST;
    }
    port_rebase(port, port_num);
    port->sctl = SET_DET;
    sleep(10);
    port->sctl = RESET_DET;

    if (support_staggered_spinup) {
        debug_print("support_staggered_spinup.");
        port->cmd |= STAG_SPINUP;
        sleep(10);
    }
    port->serr_rwc = 0xFFFFFFFF;
    port->is_rwc = 0xFFFFFFFF;
    debug_print("After fix_port.");
    while (1) {
        sleep(10);
        if (ahci_ready_to_go(port)) {
            break;
        }
        sleep(10);
    }
    debug_print("Ready to go");
}

void
ahci_setup(hba_mem_t* abar)
{
    abar->ghc |= HBA_GHC_AE;
    abar->ghc |= HBA_GHC_IE;
    debug_print("Ahci setup complete.");
}

void
print_range(uint8_t* buff, int rangeA, int rangeB)
{
    for (int i = rangeA; i <= rangeB; i++) {
        kprintf("%d ", buff[i]);
    }
}

void
ahci_readwrite_test()
{
    uint8_t* buff = (uint8_t*)(AHCI_BUFF);
    int i, j;

    kprintf("Writing 100 4KB chuncks with corresponding byte..\n");
    for (i = 0; i < 100; i++) {
        for (j = 0; j < 8; j++) {
            memset(buff, i, 512);
            ahci_rw(g_ahci_disk.port, (i * 8 + j), 0, 1, (uint16_t*)buff,
                    WRITE);
        }
    }

    memset(buff, 23, 512); // clear buffer
    kprintf("Reading first byte from each 4KB chunk:\n");
    for (j = 0; j < 100; j++) {
        ahci_rw(g_ahci_disk.port, j * 8, 0, 1, (uint16_t*)buff, READ);
        print_range(buff, 0, 0);
    }
    kprintf("\n");
}

int
ahci_probe_port(hba_mem_t* abar)
{
    uint32_t pi = abar->pi;
    int i = 0;

    debug_print("Inside ahci_probe_port.");
    for (i = 0; i < 32; i++) {

        if (!(pi >> i & 0x1)) {
            continue;
        }

        switch (ahci_get_signature(&abar->ports[i])) {
            case AHCI_DEV_SATAPI:
                kprintf("SATAPI drive found at port %d\n", i);
                break;
            case AHCI_DEV_SEMB:
                kprintf("SEMB drive found at port %d\n", i);
                break;
            case AHCI_DEV_PM:
                kprintf("PM drive found at port %d\n", i);
                break;
            case 0:
                break;
            case AHCI_DEV_SATA:
                kprintf("SATA drive found at port %d.\n", i);
                debug_print("Cap is %x.", abar->cap);
                ahci_setup(abar);
                ahci_fix_port(&abar->ports[i], i, abar->cap & HBA_MEM_CAP_SSS);
                g_ahci_disk.abar = abar;
                g_ahci_disk.port = &(abar->ports[i]);
                return 0;
        }
    }
    return -1;
}

void
ahci_discovery(void)
{
    uint8_t bus;
    uint8_t device;
    uint8_t func;
    hba_mem_t* abar;

    for (bus = 0; bus < 255; bus++) {
        for (device = 0; device < 32; device++) {
            for (func = 0; func < 8; func++) {
                if (pci_class_check(bus, device, func, AHCI_PCI_CLASS)) {
                    kprintf("Found AHCI controller, looking for disks...\n");
                    pci_config_write_dw(bus, device, func, 0x24,
                                        AHCI_PCI_ABAR_LOCATION);
                    abar = (hba_mem_t*)((uint64_t)pci_config_read_dw(
                      bus, device, func, 0x24));

                    ahci_probe_port(abar);
                }
            }
        }
    }
}

// code reference : OSdev.org

void
start_cmd(hba_port_t* port)
{
    debug_print("In start_cmd.");
    while (port->cmd & HBA_PxCMD_CR)
        ;
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
    debug_print("Out of start_cmd.");
}

void
stop_cmd(hba_port_t* port)
{
    debug_print("In stop_cmd.");
    port->cmd &= ~HBA_PxCMD_ST;

    while (1) {
        if (port->cmd & HBA_PxCMD_FR)
            continue;
        if (port->cmd & HBA_PxCMD_CR)
            continue;
        break;
    }

    port->cmd &= ~HBA_PxCMD_FRE;
    debug_print("Out of stop_cmd");
}

void
port_rebase(hba_port_t* port, int portno)
{
    debug_print("Insisde port_rebase.");
    stop_cmd(port);
    port->clb = AHCI_BASE + (portno << 10);
    memset((void*)(port->clb), 0, 1024);

    port->fb = AHCI_BASE + (32 << 10) + (portno << 8);
    memset((void*)(port->fb), 0, 256);

    hba_cmd_header_t* cmdheader = (hba_cmd_header_t*)(port->clb);
    for (int i = 0; i < 32; i++) {
        cmdheader[i].prdtl = 8;
        cmdheader[i].ctba = AHCI_BASE + (40 << 10) + (portno << 13) + (i << 8);
        memset((void*)cmdheader[i].ctba, 0, 256);
    }

    start_cmd(port);
    debug_print("Port rebase exit.");
}
int
find_cmdslot(hba_port_t* port)
{
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
ahci_rw(hba_port_t* port, uint32_t startl, uint32_t starth, uint32_t count,
        uint16_t* buf, uint8_t op)
{
    char* op_str[] = { "READ", "WRITE" };
    port->is_rwc = 0xffff;
    int spin = 0;
    int slot = find_cmdslot(port);
    int i = 0;
    if (slot == -1)
        return FALSE;

    hba_cmd_header_t* cmdheader = (hba_cmd_header_t*)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(fis_reg_h2d_t) / sizeof(uint32_t);
    cmdheader->w = op;
    cmdheader->prdtl = (uint16_t)((count - 1) >> 4) + 1;

    hba_cmd_tbl_t* cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
                        (cmdheader->prdtl - 1) * sizeof(hba_prdt_entry_t));

    for (i = 0; i < cmdheader->prdtl - 1; i++) {
        cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
        cmdtbl->prdt_entry[i].dbc = 8 * 1024;
        cmdtbl->prdt_entry[i].i = 1;
        buf += 4 * 1024;
        count -= 16;
    }
    cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
    cmdtbl->prdt_entry[i].dbc = count << 9;
    cmdtbl->prdt_entry[i].i = 1;
    fis_reg_h2d_t* cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;
    cmdfis->command = (op == READ ? ATA_CMD_READ_DMA_EX : ATA_CMD_WRITE_DMA_EX);

    cmdfis->lba0 = (uint8_t)startl;
    cmdfis->lba1 = (uint8_t)(startl >> 8);
    cmdfis->lba2 = (uint8_t)(startl >> 16);
    cmdfis->device = 1 << 6;
    cmdfis->lba3 = (uint8_t)(startl >> 24);
    cmdfis->lba4 = (uint8_t)starth;
    cmdfis->lba5 = (uint8_t)(starth >> 8);

    cmdfis->count = count;

    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
        spin++;
    }
    if (spin == 1000000) {
        kprintf("Port is hung in %s\n", op_str[op]);
        return FALSE;
    }

    port->ci = 1 << slot;

    while (1) {
        if ((port->ci & (1 << slot)) == 0)
            break;
        if (port->is_rwc & HBA_PxIS_TFES) {
            kprintf("%s disk error\n", op_str[op]);
            return FALSE;
        }
    }

    if (port->is_rwc & HBA_PxIS_TFES) {
        kprintf("%s disk error\n", op_str[op]);
        return FALSE;
    }
    return TRUE;
}
