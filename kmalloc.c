#include "kmalloc.h"
#include <stdlib.h>

struct MCB_t;

typedef struct MCB_t{
    size_t padding;
    size_t size;
    struct MCB_t* next;
} MCB;

static MCB* mcb_head = NULL;
static size_t address_align = 1;

int kmalloc_init(void* mem, size_t size, size_t align){
    if(mcb_head == NULL && mem != NULL && size > sizeof(MCB)){
        address_align = align;
        mcb_head = (MCB*)(mem);
        mcb_head->size = size - sizeof(MCB);
        mcb_head->padding = 0;
        mcb_head->next = NULL;
        return 0;

    }
    return -1;
}

void* kmalloc_align(size_t size, size_t align){
    MCB* ptr = mcb_head;
    void* result = NULL;
    MCB* pre_ptr = NULL;
    while(ptr){
        size_t offset = (size_t)((char*)ptr + sizeof(MCB)) % align;
        if(offset > 0){
            offset = align - offset;
        }
        size_t aligned_size = ptr->size - offset;
        if(ptr->size > offset && aligned_size >= size){
            void *next = ptr->next;
            ptr = (MCB*)((char*)ptr + offset);
            ptr->size = aligned_size;
            ptr->padding = offset;
            ptr->next = next;
            result = (char*)ptr + sizeof(MCB);
            break;
        }else{
            pre_ptr = ptr;
            ptr = ptr->next;
        }
    }
    if(result){
        size_t remain = ptr->size - size;
        if(remain > sizeof(MCB)){
            ptr->size = size;
            MCB* next = (MCB*)((char*)result + size);
            next->next = ptr->next;
            next->padding = 0;
            next->size = remain - sizeof(MCB);
            if(pre_ptr){
                pre_ptr->next = next;
            }else{
                mcb_head = next;
            }
        }else{
            if(pre_ptr){
                pre_ptr->next = ptr->next;
            }else{
                mcb_head = ptr->next;
            }
        }
    }
    return result;
}

static void merge(MCB* pre, MCB* next){
    if(pre && next && next > pre){
        if((char*)pre + pre->size + sizeof(MCB) == (char*)next){
            pre->next = next->next;
            pre->size += sizeof(MCB) + next->size;
        }
    }
}

void* kmalloc(size_t size){
    return kmalloc_align(size, address_align);
}

void kfree(void* ptr){
    if(ptr){
        MCB* mcb = (MCB*)((char*)ptr - sizeof(MCB));
        if(mcb->padding > 0){
            size_t size = mcb->size + mcb->padding;
            mcb = (MCB*)((char*)mcb - mcb->padding);
            mcb->size = size;
            mcb->padding = 0;
        }
        if(mcb_head){
            if(mcb_head > mcb){
                mcb->next = mcb_head;
                mcb_head = mcb;
                merge(mcb_head,mcb_head->next);
            }else{
                MCB* mcb_ptr = mcb_head;
                while(mcb_ptr){
                    if(mcb_ptr->next){
                        if(mcb_ptr->next > mcb){
                            mcb->next = mcb_ptr->next;
                            mcb_ptr->next = mcb;
                            merge(mcb, mcb->next);
                            merge(mcb_ptr, mcb_ptr->next);
                            break;
                        }
                    }else{
                        mcb_ptr->next = mcb;
                        mcb->next = NULL;
                        merge(mcb_ptr, mcb_ptr->next);
                    }
                    mcb_ptr = mcb_ptr->next;
                }
            }
        }else{
            mcb_head = mcb;
            mcb_head->next = NULL;
        }
    }
}

void kmalloc_merit(MemMerit* merit){
    if(merit){
        merit->free_bytes = 0;
        merit->free_chunks = 0;
        if(mcb_head){
            MCB* ptr = mcb_head;
            while(ptr){
                merit->free_bytes += ptr->size;
                merit->free_chunks += 1;
                ptr = ptr->next;
            }
        }
    }
}
