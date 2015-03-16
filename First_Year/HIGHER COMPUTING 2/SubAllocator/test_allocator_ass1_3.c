//
//  test_allocator_ass1_3.c
//  SubAllocator
//
//  Created by Jiashu Chen on 25/08/12.
//  Copyright (c) 2012 Jiashu Chen. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define test_allocator_c
#include "allocator.h"

#define MEMORY_SIZE 1024
#define HEADER_SIZE 24
static void *malloc_ptr = NULL;
static int malloc_num = 0;
static size_t malloc_siz = 0;
static void *test_allocator_malloc(size_t, int);
static void *test_allocator_malloc_fail(size_t);

int main(void) {
    
    int size;
    void *ptr1;
    void *ptr2;
    void *ptr3;
    void *ptr4;
    void *ptr5;
    void *ptr6;
    void *ptr7;
    void *ptr8;
    void *ptrA;
    void *ptrB;
    void *ptrC;
    
    setbuf(stdout, NULL);
    
    printf("allocator_init(%d)\n\n", MEMORY_SIZE);
    
    allocator_init(MEMORY_SIZE);
    printf("check... malloc is called once\n");
    assert(malloc_num == 1);
    printf("passed!\n\n");
    
    printf("check... malloc size\n");
    assert(malloc_siz == MEMORY_SIZE);
    printf("passed!\n\n");
    
    printf("block of memory start at %p\n\n", malloc_ptr);
    
    size = 128 - HEADER_SIZE;
    
    printf("allocate_malloc... 7 x %d memory blocks\n", size);
    ptr1 = test_allocator_malloc(size, 1+'0');
    print_memory();
    ptr2 = test_allocator_malloc(size, 2+'0');
    print_memory();
    ptr3 = test_allocator_malloc(size, 3+'0');
    print_memory();
    ptr4 = test_allocator_malloc(size, 4+'0');
    print_memory();
    ptr5 = test_allocator_malloc(size, 5+'0');
    print_memory();
    ptr6 = test_allocator_malloc(size, 6+'0');
    print_memory();
    ptr7 = test_allocator_malloc(size, 7+'0');
    print_memory();
    printf("yay, memory blocks allocated without crashing\n\n");
    
    printf("check... last block remains free\n");
    ptr8 = test_allocator_malloc_fail(size);
    print_memory();
    printf("passed!\n\n");

    printf("allocator_free 4\n");
    allocator_free(ptr4);
    print_memory();
    printf("allocator_free 6\n");
    allocator_free(ptr6);
    print_memory();
    printf("check... 4 and 6 are not merged\n");
    ptrA = test_allocator_malloc_fail(size*2);
    print_memory();
    printf("passed!\n\n");
    
    printf("allocator_free 3\n");
    allocator_free(ptr3);
    print_memory();
    printf("check... 3 and 4 are merged\n");
    ptrA = test_allocator_malloc(2*size, 'A');
    print_memory();
    assert(ptrA == ptr3);
    printf("passed!\n\n");
    
    printf("allocator_free A\n");
    allocator_free(ptrA);
    print_memory();
    printf("allocator_free 1\n");
    allocator_free(ptr1);
    print_memory();
    printf("allocator_free 2\n");
    allocator_free(ptr2);
    print_memory();
    printf("check... 1, 2, A (was 3 & 4) are merged\n");
    print_memory();
    ptrB = test_allocator_malloc(size*4, 'B');
    assert(ptrB == ptr1);
    print_memory();
    printf("passed!\n\n");
    
    printf("check... 6, 7, last free block are merged\n");
    printf("allocator_free 7\n");
    allocator_free(ptr7);
    print_memory();
    printf("check... can't allocate last free block\n");
    ptrC = test_allocator_malloc_fail(size*3);
    print_memory();
    printf("passed!\n\n");
    printf("check... can allocate part of last free block\n");
    ptrC = test_allocator_malloc(size, 'C');
    print_memory();
    assert(ptrC != NULL);
    printf("passed!\n\n");
    
    allocator_free(ptr1);
    print_memory();
    ptr7 = test_allocator_malloc(32-24, '3');
    print_memory();
    ptr3 = test_allocator_malloc(256-24, '3');
    print_memory();
    ptr4 = test_allocator_malloc(256-24, '4');
    print_memory();
    allocator_free(ptr4);
    print_memory();
    allocator_free(ptr3);
    print_memory();
    allocator_free(ptr7);
    print_memory();
    allocator_free(ptr6);
    print_memory();
    allocator_free(ptr5);
    allocator_end();
    printf("check... free is called\n");
    assert(malloc_ptr == NULL);

    printf("passed!\n\n");
    
    return EXIT_SUCCESS;
}

static void *test_allocator_malloc(size_t size, int chr) {
    void * ptr;
    ptr = allocator_malloc((u_int32_t)size);
    printf("allocator_malloc size=%d ptr=%p %c\n", (u_int32_t) size, ptr, chr);
    assert(ptr != NULL);
    memset(ptr, chr, size);
    return ptr;
}

static void *test_allocator_malloc_fail(size_t size) {
    void * ptr;
    ptr = allocator_malloc((u_int32_t) size);
    printf("allocator_malloc size=%4d ptr=%p\n", (u_int32_t) size, ptr);
    assert(ptr == NULL);
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