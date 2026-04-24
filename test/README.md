RUN BELOW TO TEST:

- clang -DTEST -I../alloc/src -o memory mm_allecatorTest.c ../alloc/src/kmalloc.c ../alloc/src/heap.c
- clang -DTEST -I../scheduling/src -I../alloc/src -o scheduling ../scheduling/src/process.c ../scheduling/src/scheduler.c shedulingTest.c ../alloc/src/kmalloc.c ../alloc/src/heap.c