#include "heap.h"

#ifdef TEST
char heap_buffer[HEAP_SIZE];
#endif

void heap_init(void){
    block_header_t *start = (block_header_t*) HEAP_START;
    start->size = HEAP_SIZE - sizeof(block_header_t);
    start->isFree = 1;
    start->next = NULL;
}