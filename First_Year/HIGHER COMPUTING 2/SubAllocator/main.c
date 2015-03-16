//
//  main.c
//  suballocator
//
//  Created by Jiashu Chen on 7/08/12.
//  Copyright (c) 2012 Jiashu Chen. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "allocator.h"


int main(int argc, const char * argv[]){
    char *case1;
    char *case2;
    char *case3;
    char *case4;
    char *case5;
    char *pointer;
    
    
    allocator_init(512);
    printf("\nAllocate case1 32 bytes\n\n");
    case1 = (char*) allocator_malloc(32);
    print_memory();
    
    printf("\nAllocate case3 8 bytes\n\n");
    case2 = (char*) allocator_malloc(8);
    print_memory();
    
    printf("\nAllocate case3 64 bytes\n\n");
    case3 = (char*) allocator_malloc(64);
    print_memory();
    
    printf("\nAllocate case4 64 bytes\n\n");
    case4 = (char*) allocator_malloc(64);
    print_memory();
    
    if (case4 == NULL) {
        printf("Yes, Allocating case4 returned NULL\n");
    }
    
    printf("\nAllocate case5 64 bytes\n\n");
    case5 = (char*) allocator_malloc(64);
    print_memory();
    
    printf("\nfreeing memory now\n\n");
    
    printf("\nfreeing case1\n");
    allocator_free(case1);
    print_memory();
    
    printf("\nAllocate pointer 32 bytes\n\n");
    pointer = (char*) allocator_malloc(32);
    
    if (pointer == NULL){
        printf ("program fucked\n");
        abort();
    }
    print_memory();
    
    printf("\nfreeing case2\n");
    allocator_free(case2);
    print_memory();
    
    printf("\nfreeing case3\n");
    allocator_free(case3);
    print_memory();
    
    printf("\nfreeing case4\n");
    allocator_free(case4);
    print_memory();
    
    printf("\nfreeing case5\n");
    allocator_free(case5);
    print_memory();
    
    printf("\nfreeing pointer\n");
    allocator_free(pointer);
    print_memory();
    
    allocator_end();
    return EXIT_SUCCESS;
}
