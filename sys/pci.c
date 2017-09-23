#include <sys/ahci.h>
#include <sys/debug.h>
#include <sys/defs.h>
#include <sys/kprintf.h>

uint32_t
sysInLong(uint16_t port)
{
    uint32_t value;
    __asm__("inl %1,%0;" : "=a"(value) : "d"(port));
    return value;
}

void
sysOutLong(uint16_t port, uint32_t value)
{
    __asm__("outl %0, %1;" : : "a"(value), "d"(port));
}

uint32_t
pciConfigReadByte(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;

    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (ldevice << 11) | (lfunc << 8) |
                         (offset & 0xfc) | ((uint32_t)0x80000000));

    /* write out the address */
    sysOutLong(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register
     */
    tmp = (sysInLong(0xCFC));
    return (tmp);
}

uint16_t
pciConfigReadWord(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
    uint32_t value = 0;
    value = pciConfigReadByte(bus, device, func, offset);
    return (uint16_t)((value >> ((offset & 2) * 8)) & 0xffff);
}

void
pciCheckVendor(uint8_t bus, uint8_t device)
{
    uint16_t vendor_id, device_id;
    uint32_t class_id;
    uint64_t bar_5;
    /* vendors that == 0xFFFF, it must be a non-existent device. */
    if ((vendor_id = pciConfigReadWord(bus, device, 0, 0)) != 0xFFFF) {
        device_id = pciConfigReadWord(bus, device, 0, 2);
        class_id = pciConfigReadByte(bus, device, 0, 0x08);
        if (((class_id & 0xffffff00) == 0x01060100)) {
            kprintf("%d %d %x %x %x*\n", bus, device, vendor_id, device_id,
                    class_id);
            bar_5 = pciConfigReadByte(bus, device, 0, 0x24) & 0x4fffffff;
            kprintf("Bar is %x", bar_5);
            kprintf("Val at bar is ");
            debugDumpData((void*)bar_5, 0x2b);
        }
    }
}

void
checkAllBuses(void)
{
    uint8_t bus;
    uint8_t device;
    for (bus = 0; bus < 255; bus++) {
        for (device = 0; device < 32; device++) {
            pciCheckVendor(bus, device);
        }
    }
}
