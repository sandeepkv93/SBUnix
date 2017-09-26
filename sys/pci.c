#include <sys/debug.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/pci.h>

uint32_t
in_dw(uint16_t port)
{
    uint32_t value;
    __asm__("inl %1,%0;" : "=a"(value) : "d"(port));
    return value;
}

void
out_dw(uint16_t port, uint32_t value)
{
    __asm__("outl %0, %1;" : : "a"(value), "d"(port));
}

uint32_t
pci_get_config_address(uint32_t bus, uint32_t device, uint32_t func,
                       uint8_t offset)
{
    return (uint32_t)((bus << 16) | (device << 11) | (func << 8) |
                      (offset & 0xfc) | ((uint32_t)0x80000000));
}

void
pci_config_write_dw(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset,
                    uint32_t value)
{
    uint32_t address;
    address = pci_get_config_address(bus, device, func, offset);
    out_dw(0xCF8, address);
    out_dw(0xCFC, value);
}

uint32_t
pci_config_read_dw(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
    uint32_t address;
    address = pci_get_config_address(bus, device, func, offset);
    out_dw(0xCF8, address);
    return in_dw(0xCFC);
}

uint16_t
pci_config_read_word(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
    uint32_t value = 0;
    value = pci_config_read_dw(bus, device, func, offset) & 0xFFFFFFFF;
    return (uint16_t)((value >> ((offset & 2) * 8)) & 0xFFFF);
}

// TODO: Make a function that returns a link list of structs that describe PCI
// config space instead of checking each pair of bus,device
bool
pci_class_check(uint8_t bus, uint8_t device, uint8_t func, uint32_t deviceClass)
{
    uint16_t vendor_id;
    uint32_t class_id;

    vendor_id = pci_config_read_word(bus, device, func, 0);

    if (vendor_id == 0xFFFF) {
        // Device doesn't exist
        return FALSE;
    }

    class_id = pci_config_read_dw(bus, device, func, 8);
    /*
     * Lower 8 bits of class ID are just revID, we'll ignore it
     * We expect class argument to only have higher 24bit info
     */
    return ((class_id >> 8) == deviceClass);
}
