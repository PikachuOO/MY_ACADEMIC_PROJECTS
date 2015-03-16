//
//  test_allocator_ass1_2.c
//  SubAllocator
//
//  Created by Jiashu Chen on 24/08/12.
//  Copyright (c) 2012 Jiashu Chen. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define test_allocator_c
#include "allocator.h"

#define MEMORY_SIZE 4096
#define HEADER_SIZE 24
static void *malloc_ptr = NULL;
static int malloc_num = 0;
static size_t malloc_siz = 0;
static void *test_allocator_malloc(size_t, int);

int main(void) {
    
    int size1;
    int size2;
    int size3;
    int size4;
    int size5;
    int size6;
    int size7;
    int sizeA;
    int sizeB;
    int sizeC;
    void *ptr1;
    void *ptr2;
    void *ptr3;
    void *ptr4;
    void *ptr5;
    void *ptr6;
    void *ptr7;
    void *ptrA;
    void *ptrB;
    void *ptrC;
    
    setbuf(stdout, NULL);
    
    printf("allocator_init(%d)\n\n", MEMORY_SIZE);
    
    allocator_init(MEMORY_SIZE);
    printf("check... malloc is called once\n"); assert(malloc_num == 1);
    printf("passed!\n\n");
    
    printf("check... malloc size\n");
    assert(malloc_siz == MEMORY_SIZE);
    printf("passed!\n\n");
    
    printf("block of memory start at %p\n\n", malloc_ptr);
    
    printf("check... allocator_malloc can malloc on different sizes\n");
    printf("check...        and memory can be used without crashing\n");
    /*
     1=128|2=32|free|3=64|4=256|5=512|6=1024|7=2048
     */
    
    size1 = 128 - HEADER_SIZE - 15;
    ptr1 = test_allocator_malloc(size1, 1+'0');
    print_memory();

    size2 = 32 - HEADER_SIZE - 8;
    ptr2 = test_allocator_malloc(size2, 2+'0');
    print_memory();

    size3 = 64 - HEADER_SIZE;
    ptr3 = test_allocator_malloc(size3, 3+'0');
    print_memory();

    size7 = 2048 - HEADER_SIZE - 1;
    ptr7 = test_allocator_malloc(size7, 7+'0');
    print_memory();

    size6 = 1024 - HEADER_SIZE - 2;
    ptr6 = test_allocator_malloc(size6, 6+'0');
    print_memory();

    size5 = 512 - HEADER_SIZE - 3;
    ptr5 = test_allocator_malloc(size5, 5+'0');
    print_memory();

    size4 = 256 - HEADER_SIZE - 4;
    ptr4 = test_allocator_malloc(size4, 4+'0');
    print_memory();
    printf("passed!\n\n");
    print_memory();

    printf("check... allocator_free is releasing memory\n");
    printf("allocator_free 4 ptr=%p\n", ptr4);
    allocator_free(ptr4);
    print_memory();
        
    sizeA = 128 - HEADER_SIZE - 10;
    ptrA = test_allocator_malloc(sizeA, 'A');
    print_memory();
    assert(ptrA == ptr4 || ptrA == (char *)ptr4 + 128);

    sizeB = 128 - HEADER_SIZE;
    ptrB = test_allocator_malloc(sizeB, 'B');
    assert(ptrB == ptr4 || ptrB == (char *)ptr4 + 128);
    
    printf("allocator_free 1 ptr=%p\n", ptr1);
    allocator_free(ptr1);
    
    sizeC = 128 - HEADER_SIZE;
    ptrC = test_allocator_malloc(sizeC, 'C');
    assert(ptrC == ptr1);
    
    allocator_end();
    printf("check... free is called\n");
    assert(malloc_ptr == NULL);
    printf("passed!\n\n");
    
    return EXIT_SUCCESS;
}




static void *test_allocator_malloc(size_t size, int chr) {
    void * ptr;
    ptr = allocator_malloc((u_int32_t) size);
    printf("allocator_malloc size=%zd ptr=%p %c\n", size, ptr, chr);
    assert(ptr != NULL);
    memset(ptr, chr, size);
    return ptr;
}

void *test_malloc (size_t size) {
    malloc_ptr = malloc(size);
    malloc_num++;
    malloc_siz = size;
    return malloc_ptr;
}

void test_free (void *ptr) {
    if (malloc_ptr == ptr) {
        malloc_ptr = NULL;
    }
    free(ptr);
}
