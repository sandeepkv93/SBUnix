#include <sys/vma.h>

#define PAGELIST_ENTRIES (1024 * 1024)
#define PAGE_SIZE 4096
struct pagelist_t pages[PAGELIST_ENTRIES];
struct pagelist_t* freepage_head;

void
vma_pagelist_add_addresses(uint64_t start, uint64_t end)
{
    for (int i = start / PAGE_SIZE; i < end / PAGE_SIZE; i++) {
        pages[i].present = TRUE;
    }
}

void
vma_pagelist_create(uint64_t physfree)
{
    int i = physfree / PAGE_SIZE;
    bool isLastPage = FALSE;
    while (!isLastPage) {
        isLastPage = TRUE;
        for (int j = i + 1; j < PAGELIST_ENTRIES; j++) {
            if (pages[j].present) {
                pages[i].next = &pages[j];
                isLastPage = FALSE;
                i = j;
                break;
            }
        }
    }
    freepage_head = &pages[physfree / PAGE_SIZE];
}

void*
vma_pagelist_getpage()
{
    uint64_t page =
      (freepage_head - pages) / (sizeof(struct pagelist_t)) * PAGE_SIZE;
    if (freepage_head == NULL)
        return NULL;
    freepage_head = freepage_head->next;
    return (void*)page;
}
