
#ifndef HEAP_P
#define HEAP_P

#include <stdint.h>
#include <stddef.h>


#define HEAP_SIZE (1024 * 1024)

//below needs to get commented and uncomment line 13-18 for testing purpose!!!
#define HEAP_START 0x200000

// #ifdef TEST
// extern char heap_buffer[HEAP_SIZE];
// #define HEAP_START ((uintptr_t)heap_buffer)
// #else
// #define HEAP_START 0x200000
// #endif

typedef struct block_header{
    struct block_header *next;
    size_t size;
    int isFree;
} block_header_t;


void  heap_init(void);
void *kmalloc(size_t size);
void  kfree(void *ptr);

#endif