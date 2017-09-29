#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H
#include <sys/defs.h>
#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)
#define PIC_EOI 0x20 /* End-of-interrupt command code */

typedef struct
{
    uint16_t offset_1; // offset bits 0..15
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t
      ist; // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t type_attr; // type and attributes
    uint16_t offset_2; // offset bits 16..31
    uint32_t offset_3; // offset bits 32..63
    uint32_t zero;     // reserved
} __attribute__((packed)) struct_idt_entry;

struct_idt_entry idt[256];

void pic_init();
void register_idt();
void enable_interrupts(bool);

#endif
