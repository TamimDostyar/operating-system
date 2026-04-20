#include "heap.h"

void *kmalloc(size_t size){
    if (size == 0) return 0;
    // round the size
    // we need to carve so that without that we don't waste any
    // i am using the pattern idiom (x + N-1) & ~(N-1)
    size = (size + 7) & ~7;

    block_header_t *cur = (block_header_t*) HEAP_START;
    
    while (cur){
        KASSERT(cur->size > 0);
        // for first fit in 
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
            return (void *)(cur + 1);
        }
        cur = cur->next;
    }
    return 0;
}
//[A: free] -> [B: used] -> [C: free]
// this is the idea i am thinking with merging
//[A: free] -> [B+C: free]
// merging basically does next
// what I am thinking to do is every time we free the free function
// calls merge to merge back to heap but we need to get that from the main memory address

void merge() {
    block_header_t *hdr = (block_header_t*) HEAP_START;
    while (hdr && hdr->next) {
        if (hdr->isFree && hdr->next->isFree) {
            hdr->size = hdr->size + sizeof(block_header_t) + hdr->next->size;
            hdr->next = hdr->next->next;
        } else {
            hdr = hdr->next;
        }
    }
}

void kfree(void *ptr){
    if (!ptr) return;
    KASSERT((uintptr_t)ptr > HEAP_START);
    block_header_t *hdr = (block_header_t*) ptr-1;
    hdr->isFree=1;
    merge();
}

