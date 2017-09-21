#include <sys/defs.h>
#include <sys/kprintf.h>

/*
void checkDevice(uint8_t bus, uint8_t device) {
     uint8_t function = 0;

     vendorID = getVendorID(bus, device, function);
     if(vendorID == 0xFFFF) return;
        // Device doesn't exist
     kprintf("%x ",vendorID);
     //checkFunction(bus, device, function);
     headerType = getHeaderType(bus, device, function);
     if( (headerType & 0x80) != 0) {
         // It is a multi-function device, so check remaining functions
         for(function = 1; function < 8; function++) {
             if(getVendorID(bus, device, function) != 0xFFFF) {
                 checkFunction(bus, device, function);
             }
         }
     }
 }
*/

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

uint16_t
pciConfigReadWord(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;

    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (ldevice << 11) | (lfunc << 8) |
                         (offset & 0xfc) | ((uint32_t)0x80000000));

    /* write out the address */
    sysOutLong(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register
     */
    tmp = (uint16_t)((sysInLong(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}
uint32_t
pciConfigReadWord1(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
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
// long counter = 0;
void
pciCheckVendor(uint8_t bus, uint8_t device)
{
    uint16_t vendor_id, device_id;
    // uint32_t class_id;
    uint32_t class_id;
    /* try and read the first configuration register. Since there are no */
    /* vendors that == 0xFFFF, it must be a non-existent device. */
    if ((vendor_id = pciConfigReadWord(bus, device, 0, 0)) != 0xFFFF) {
        // if ((vendor_id = pciConfigReadWord(bus, device, 0, 0)) != 0xFFFF) {
        // kprintf("%x ",vendor_id);
        device_id = pciConfigReadWord(bus, device, 0, 2);
        // kprintf("%x ",device_id);
        class_id = pciConfigReadWord1(bus, device, 0, 0x08);
        /*if ((class_id == 0x010601)) {
            // if ((class_id & 0xff000000) == 0x01000000)
            // if (vendor_id == 0x8086)
            kprintf("%x %x %x %x\n", vendor_id, device_id, class_id);
        }
        */
        if ((vendor_id == 0x8086) && ((class_id & 0xffffff00) == 0x01060100)) {
            kprintf("%d %d %x %x %x*",bus, device,  vendor_id, device_id, class_id);
        };
    }
}

void
checkAllBuses(void)
{
    uint8_t bus;
    uint8_t device;
    //bus = 0;
    //device = 5;
    for (bus = 0; bus < 255; bus++) {
        for (device = 0; device < 32; device++) {
            pciCheckVendor(bus, device);
        }
    }
}
