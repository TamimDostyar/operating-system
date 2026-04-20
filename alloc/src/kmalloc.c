#include "heap.h"

void *kmalloc(size_t size){

    block_header_t *cur = (block_header_t*) HEAP_START;
    
    while (cur){
        if (cur->isFree && cur->size >= size){
            // for larger cases we need to still leave size
            if (cur->size >= size + sizeof(block_header_t) + 1){
                block_header_t *new_block = (block_header_t*)((char*)(cur + 1) + size);
                new_block->size  = cur->size - size - sizeof(block_header_t);
                new_block->isFree = 1;
                new_block->next  = cur->next;
                cur->size = size;
                cur->next = new_block;
            }
            cur->isFree = 0;
            return (void *) (cur +1);   
        }
        cur = cur->next;
    }
    return 0;
}


void kfree(void *ptr){
    if (!ptr) return;
    block_header_t *hdr = (block_header_t*) ptr-1;
    hdr->isFree=1;
}