#include <sys/ahci.h>
#include <sys/kprintf.h>
#include <sys/pci.h>
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
                pci_config_write_dw(bus, device, 0, 0x24, AHCI_PCI_ABAR_LOCATION);
                abar = pci_config_read_dw(bus, device, 0, 0x24);
                ahci_probe_port((hba_mem_t*)abar);
            }
        }
    }
}
