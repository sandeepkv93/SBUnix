#define PCI_CONFIG_DATA 0xCFC
#define PCI_CONFIG_ADDRESS 0xCF8
uint32_t pci_config_read_dw(uint8_t bus, uint8_t device, uint8_t func,
                            uint8_t offset);
void pci_config_write_dw(uint8_t bus, uint8_t device, uint8_t func,
                         uint8_t offset, uint32_t value);
bool pci_class_check(uint8_t bus, uint8_t device, uint8_t func,
                     uint32_t deviceClass);
uint16_t pci_config_read_word(uint8_t bus, uint8_t device, uint8_t func,
                              uint8_t offset);
