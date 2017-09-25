#include <sys/ahci.h>
#include <sys/kprintf.h>
#include <sys/pci.h>
#define AHCI_BASE 0x400000

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

void
ahci_probe_port(hba_mem_t* abar)
{
    uint32_t pi = abar->pi;
    int i = 0;

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
    uint64_t abar;

    for (bus = 0; bus < 255; bus++) {
        for (device = 0; device < 32; device++) {
            if (pci_class_check(bus, device, AHCI_PCI_CLASS)) {
                // Let's relocate ABAR within 1GB
                // ABAR is Bar[5] which is located at offset 0x24
                pci_config_write_dw(bus, device, 0, 0x24,
                                    AHCI_PCI_ABAR_LOCATION);
                abar = pci_config_read_dw(bus, device, 0, 0x24);
                ahci_probe_port((hba_mem_t*)abar);
            }
        }
    }
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