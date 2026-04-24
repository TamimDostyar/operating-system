
#ifndef HEAP_P
#define HEAP_P

#include <stdint.h>
#include <stddef.h>


#define HEAP_SIZE (1024 * 1024)
#ifdef TEST
extern char heap_buffer[HEAP_SIZE];
#define HEAP_START ((uintptr_t)heap_buffer)
#else
#define HEAP_START 0x200000
#endif

typedef struct block_header{
    struct block_header *next;
    size_t size;
    int isFree;
} block_header_t;


void  heap_init(void);
void *kmalloc(size_t size);
void  kfree(void *ptr);


#ifdef DEBUG
#define KASSERT(cond) if (!(cond)) { __asm__ volatile("hlt"); }
#else
#define KASSERT(cond)
#endif


#endif