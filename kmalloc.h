#ifndef _KMALLOC_H
#define _KMALLOC_H
#include <stddef.h>

typedef struct MemMerit{
    size_t free_bytes;
    size_t free_chunks;
}MemMerit;

int kmalloc_init(void* mem, size_t size, size_t align);

void* kmalloc_align(size_t size, size_t align);

void* kmalloc(size_t size);

void kfree(void* ptr);

void kmalloc_merit(MemMerit* merit);

#endif
