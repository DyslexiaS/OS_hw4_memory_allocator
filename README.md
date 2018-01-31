# OS_hw4_memory_allocator
## Implement
Implement a memory allocator library for user application 
- alloc 16 
- free 0x00000028
- print bin[1]
- print bin[6]
## user application
Write a user application to test the memory allocator library 
- Should receive 3 kinds of commands 1) alloc N 
- Call hw_malloc(N) to allocate N bytes of data memory 
- Print relative data address (i.e., offset between start_brk and the address returned by hw_malloc()) 2) free ADDR 
- Call hw_free() to free the memory at (start_brk + ADDR) 
- Print either “success” or “fail” 3) print BIN 
- Print address and size information of a given bin • BIN can be bin[0], bin[1], bin[2], bin[3], bin[4], bin[5], or bin[6]
- "cat testfile.txt | ./hw4_mm_test > outputfile.txt"
- diff outputfile.txt solution.txt
