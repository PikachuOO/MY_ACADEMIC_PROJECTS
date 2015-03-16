//
//  test_allocator.c
//  SubAllocator
//
//  Created by Jiashu Chen on 16/08/12.
//  Copyright (c) 2012 Jiashu Chen. All rights reserved.
//

#include <stdio.h>
#include <assert.h>

#define test_allocator_c
#include "allocator.h"

#define MEMORY_SIZE 1024
#define HEADER_SIZE 24
static void *malloc_ptr = NULL;
static int malloc_num = 0;
static size_t malloc_siz = 0;

int main(void) {
    
    int size;
    void *ptr;
    void *free_region_ptr;
    
    setbuf(stdout, NULL);
    
    printf("allocator_init(%d)\n\n", MEMORY_SIZE);
    allocator_init(MEMORY_SIZE);
    printf("check... malloc is called once\n");
    assert(malloc_num == 1);
    printf("passed!\n\n");
    
    printf("check... malloc size\n");
    assert(malloc_siz == MEMORY_SIZE);
    printf("passed!\n\n");
    
    free_region_ptr = malloc_ptr;
    printf("block of memory start at %p\n\n", free_region_ptr);
    
    size = MEMORY_SIZE - HEADER_SIZE ;
    printf("check... allocator_malloc(%d)\n", size);
    assert(allocator_malloc(size) == NULL);
    printf("return NULL - passed!\n\n");
    
    size = MEMORY_SIZE / 2 - HEADER_SIZE;
    printf("check... allocator_malloc(%d) first\n", size);
    ptr = allocator_malloc(size);
    assert(ptr != NULL
           && ( ptr == free_region_ptr + HEADER_SIZE
               || ptr == free_region_ptr + (MEMORY_SIZE/2) + HEADER_SIZE)
           );
    printf("return %p - passed!\n", ptr);
    printf("check... allocator_malloc(%d) second\n", size);
    assert(allocator_malloc(size) == NULL);
    printf("return NULL - passed!\n\n");
    
    if (ptr == free_region_ptr + HEADER_SIZE) free_region_ptr += MEMORY_SIZE / 2;\
    size = MEMORY_SIZE / 4 - HEADER_SIZE;
    printf("check... allocator_malloc(%d) first\n", size);
    ptr = allocator_malloc(size);
    assert(ptr != NULL
           && ( ptr == free_region_ptr + HEADER_SIZE
               || ptr == free_region_ptr + (MEMORY_SIZE/4) + HEADER_SIZE)
           );
    printf("return %p - passed!\n", ptr);
    printf("check... allocator_malloc(%d) second\n", size);
    assert(allocator_malloc(size) == NULL);
    printf("return NULL - passed!\n\n");
    
    if (ptr == free_region_ptr + HEADER_SIZE) free_region_ptr += MEMORY_SIZE / 4;
    
    size = MEMORY_SIZE / 8 - HEADER_SIZE;
    printf("check... allocator_malloc(%d) first\n", size);
    ptr = allocator_malloc(size);
    assert(ptr != NULL
           && ( ptr == free_region_ptr + HEADER_SIZE
               || ptr == free_region_ptr + (MEMORY_SIZE/8) + HEADER_SIZE)
           );
    printf("return %p - passed!\n", ptr);
    printf("check... allocator_malloc(%d) second\n", size);
    assert(allocator_malloc(size) == NULL);
    printf("return NULL - passed!\n\n");
    
    if (ptr == free_region_ptr + HEADER_SIZE) free_region_ptr += MEMORY_SIZE / 8;

    size = MEMORY_SIZE / 16 - HEADER_SIZE;
    printf("check... allocator_malloc(%d) first\n", size);
    ptr = allocator_malloc(size);
    assert(ptr != NULL
           && ( ptr == free_region_ptr + HEADER_SIZE
               || ptr == free_region_ptr + (MEMORY_SIZE/16) + HEADER_SIZE)
           );
    printf("return %p - passed!\n", ptr);
    printf("check... allocator_malloc(%d) second\n", size);
    assert(allocator_malloc(size) == NULL);
    printf("return NULL - passed!\n\n");
    
    if (ptr == free_region_ptr + HEADER_SIZE) free_region_ptr += MEMORY_SIZE/16;
    
    size = MEMORY_SIZE / 32 - HEADER_SIZE;
    printf("check... allocator_malloc(%d) first\n", size);
    ptr = allocator_malloc(size);
    assert(ptr != NULL
           && ( ptr == free_region_ptr + HEADER_SIZE
               || ptr == free_region_ptr + (MEMORY_SIZE/32) + HEADER_SIZE)
           );
    printf("return %p - passed!\n", ptr);
    printf("check... allocator_malloc(%d) second\n", size);
    assert(allocator_malloc(size) == NULL);
    printf("return NULL - passed!\n\n");
    
    size = 1;
    printf("check... allocator_malloc(%d)\n", size);
    assert(allocator_malloc(size) == NULL);
    printf("return NULL - passed!\n\n");
    
    allocator_end();
    printf("check... free is called\n");
    assert(malloc_ptr == NULL);
    printf("passed!\n\n");
    
    return EXIT_SUCCESS;
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