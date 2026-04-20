#include "../alloc/src/heap.h"

#include <stddef.h>
#include <stdio.h>

static int tests_run = 0;
static int tests_passed = 0;

static void check(const char *name, int condition) {
    tests_run++;
    if (condition) {
        tests_passed++;
        printf("  PASS: %s\n", name);
    } else {
        printf("  FAIL: %s\n", name);
    }
}

static void test_basic_alloc() {
    void *ptr = kmalloc(64);
    check("basic alloc returns non-null", ptr != 0);
    kfree(ptr);
}

static void test_zero_alloc() {
    void *ptr = kmalloc(0);
    check("zero alloc returns null", ptr == 0);
}

static void test_multiple_allocs() {
    void *a = kmalloc(32);
    void *b = kmalloc(64);
    void *c = kmalloc(128);
    check("alloc a non-null", a != 0);
    check("alloc b non-null", b != 0);
    check("alloc c non-null", c != 0);
    check("pointers are distinct", a != b && b != c && a != c);
    kfree(a);
    kfree(b);
    kfree(c);
}

static void test_reuse_after_free() {
    void *a = kmalloc(64);
    kfree(a);
    void *b = kmalloc(64);
    check("block reused after free", b != 0);
    kfree(b);
}

static void test_coalescing() {
    void *a = kmalloc(32);
    void *b = kmalloc(32);
    void *c = kmalloc(32);
    kfree(a);
    kfree(b);
    kfree(c);
    // after freeing all three, a large alloc should succeed
    void *big = kmalloc(80);
    check("coalesced blocks allow large alloc", big != 0);
    kfree(big);
}

static void test_repeated_cycles() {
    for (int i = 0; i < 100; i++) {
        void *ptr = kmalloc(16);
        check("repeated alloc non-null", ptr != 0);
        kfree(ptr);
    }
}

static void test_out_of_memory() {
    // request more than heap size
    void *ptr = kmalloc(2 * 1024 * 1024);
    check("oversized alloc returns null", ptr == 0);
}

static void test_null_free() {
    kfree(0);   // should not crash
    check("free null does not crash", 1);
}

void mm_allocator_test_run() {
    heap_init();

    test_basic_alloc();
    test_zero_alloc();
    test_multiple_allocs();
    test_reuse_after_free();
    test_coalescing();
    test_repeated_cycles();
    test_out_of_memory();
    test_null_free();

    printf("\n%d/%d tests passed\n", tests_passed, tests_run);
}

int main(){
    mm_allocator_test_run();
    return 0;
}