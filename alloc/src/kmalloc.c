#include "heap.h"

void *kmalloc(size_t size){

    block_header_t *cur = (block_header_t*) HEAP_START;
    
    while (cur){
        if (cur->isFree && cur->size >= size){
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