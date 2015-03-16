//
//  allocator.c
//  SubAllocator
//
//  Created by Jiashu Chen on 7/08/12.
//  Copyright (c) 2012 Jiashu Chen. All rights reserved.
//

//this is the final version with more comments and explanination added

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "allocator.h"

//Macros, and magic numbers
#define MAGIC_FREE 0xDEADBABE
#define MAGIC_IN_USE 0xB16B00B5
#define BASE 2
#define EMPTY 0
#define ADMIN_SIZE (int) (sizeof(admin))
//ANSI ESCAPE SEQUENCE
//used for printing
#define B_RED "\x1b[41m"
#define B_BLUE "\x1b[44m"
#define B_GREEN "\x1b[42m"
#define B_MAGNETA "\x1b[45m"
#define B_YELLOW "\x1b[43m"
#define B_CYAN "\x1b[46m"
#define F_RED "\x1b[31m"
#define F_MAGNETA "\x1b[35m"
#define F_BLUE "\x1b[34m"
#define F_WHITE "\x1b[37m"
#define ORIGINAL "\x1b[0m"
//Boolean Operator with meaning explained
//for split
#define NEED_SPLIT (1 == 1)
#define NO_NEED_SPLIT (1 == 0)
//for merge
#define MERGE (1 == 1)
#define ALL_CLEAR (1 == 0)
//for general validation
#define YES (1 == 1)
#define NO (1 == 0)

//Header Structure
typedef struct _admin *Admin;

typedef struct _admin{
    u_int32_t magic;
    u_int32_t size;
    Admin next;
    Admin prev;
} admin;

//Global variables
typedef unsigned char byte;
static byte *memory = NULL;
static Admin free_list_Ptr;
//sizeLimit whole memory
static short sizeLimit = EMPTY;
//space left that can be use
static short sizeLeft = EMPTY;
//function declearation extra functions that I made mostlikely to be static functions
//calculate the size really needed
static u_int32_t sizeReq (u_int32_t size);
//split memory into the appropriate size
static void splitMemory(Admin start);
//print memory in a bar format
static void printMemoBar(void);

void allocator_init(u_int32_t size){
    u_int32_t realSize;
    realSize = sizeReq(size);
    while (realSize <= ADMIN_SIZE) {
        printf("Put in a valid argument\n");
        scanf("%d\n", &size);
        realSize = sizeReq(size);
    }
    printf("Initialising the Memory Slot\n");
    printf("Required Size: %d bytes\n", size);
    printf("Actual malloced size %d bytes\n", realSize);
    memory = malloc(realSize);
    free_list_Ptr = (Admin) memory;
    sizeLimit = realSize;
    sizeLeft = realSize;
    printf("\nAddress of memory is %p\n", memory);
    printf("Address of free_list_Ptr is %p\n", free_list_Ptr);
    free_list_Ptr->magic = MAGIC_FREE;
    free_list_Ptr->size = sizeLimit;
    free_list_Ptr->next = free_list_Ptr;
    free_list_Ptr->prev = free_list_Ptr;
    /*
     printf("\x1B[34m"
     "free_list_Ptr = %p,\nfree_list_PtrNext = %p\n"
     "\x1B[0m"
     "\n\n\n", free_list_Ptr, free_list_Ptr->next);
     */
}

void *allocator_malloc(u_int32_t n){
    //size need to alloc
    u_int32_t allocaSize;
    Admin pointer;
    Admin check = free_list_Ptr;
    void *ptrReturned = NULL;
    short need = NEED_SPLIT;
    short efficient = NO;
    //calculating sizes
    allocaSize = sizeReq(ADMIN_SIZE + n);
    //check if enought space left
    if (allocaSize < sizeLeft) {
        //check if there is an efficient partial block left
        //the do while loop avoid case like the following trying to alloca 256
        // ||128 free ||256 occupied || 128 free||
        check = free_list_Ptr;
        do {
            check = check->next;
            if (check->size >= allocaSize && check->magic == MAGIC_FREE) {
                efficient = YES;
            }
        } while (check != free_list_Ptr && need == NEED_SPLIT && efficient == NO);
        
        if (efficient == YES) {
            //check for matching sizes (finding algorithm)
            check = free_list_Ptr;
            do {
                check = check->next;
                if (check->size == allocaSize && check->magic == MAGIC_FREE) {
                    need = NO_NEED_SPLIT;
                }
            } while (check != free_list_Ptr && need == NEED_SPLIT);
            //in no matching sizes, then do split (splitting algorithm) (by logic at this step there exist a efficient block)
            if (need == NEED_SPLIT) {
                //used for testing
                //printf("split is called\n");
                check = free_list_Ptr->next;
                while ((check->size < (BASE * allocaSize) || check->magic == MAGIC_IN_USE)
                       && check != free_list_Ptr){
                    check = check->next;
                }
                //used for testing
                //printf("checkSize before split called is %d\n",check->size);
                //printf("allocaSize before split called is %d\n", allocaSize);
                
                //reassure the node being used is free
                // also as spec required, this also detect whether is valid or not
                if ((check->size <= allocaSize || (check->next == free_list_Ptr && check->size == allocaSize))
                    || (check->magic != MAGIC_FREE && check->magic != MAGIC_IN_USE)) {
                 fprintf(stderr, "MEMORY OCCUPIED or NOT ENOUGH FREE SPACE\n");
                 fprintf(stderr, "SEGMENTATION FAULT\n");
                 allocator_end();
                }
                 
                //do split
                //reassure again it is not the size required
                //if statement not really needed but put down for logic clarity
                //however while loop inside is required
                if (check->size != allocaSize) {
                    //printf("Memory Block size is %d\n" "Actual needed size is %d\n", check->size, allocaSize);
                    while ((check->size < allocaSize || check->size == MAGIC_IN_USE)) {
                        check = check->next;
                    }
                    while (check->size/2 >= allocaSize
                           && check != NULL) {
                        splitMemory(check);
                        //printf("size is %d\n", check->size);
                    }
                }
            }
            pointer = check;
            pointer->magic = MAGIC_IN_USE;
            sizeLeft -= pointer->size;
            ptrReturned = (void*) pointer + ADMIN_SIZE;
            //used for testing
            //printf("size left is %d\n", sizeLeft);
            //printf("pointer address is %p\n", pointer);
            //printf("ptrReturned address is %p\n", ptrReturned);
            free_list_Ptr = pointer;
        }else if (efficient == NO){
            //this else if is not really needed (ptrReturned was initialised to NULL), but put down logic clarity
            ptrReturned = NULL;
        }
    }
    return ptrReturned;
}

static void splitMemory(Admin start){
    Admin new;
    Admin check = start;
    void *ptr;
    ptr = (char*) start;
    ptr = ptr + (check->size)/2;
    new = (Admin) ptr;
    new->magic = MAGIC_FREE;
    new->size = (u_int32_t)((check->size) / 2);
    check->size /= 2;
    if (check->prev == check && check->next == check) {
        check->prev = new;
        new->next = check;
        check->next = new;
        new->prev = check;
    }else{
        check->next->prev = new;
        new->prev = check;
        new->next = check->next;
        check->next = new;
    }
    //used for testing
    /*
     printf("newSize = %d\n"
     "CheckSize after fragmentation %d\n"
     "newAddress = %p\n"
     "checkAddress = %p\n"
     "checkNext = %p\n"
     "newPrev = %p\n"
     "checkPrev = %p\n"
     "newNext = %p\n\n\n", new->size,check->size, new, check, check->next, new->prev,check->prev, new->next);
     */
}

void allocator_free(void *object){
    Admin check;
    //treating the start of memory and the end of the memory case
    //free//occupied//free//occupied//free//
    Admin top = (Admin) memory;
    void *headPtr;
    short merge = MERGE;
    headPtr = object - ADMIN_SIZE;
    check = (Admin) headPtr;
    printf("Header being freed is %p\n", check);
    if (check->magic == MAGIC_FREE) {
        fprintf(stderr, "Memory has already been freed\n");
        abort();
    }else if (check->magic == MAGIC_IN_USE){
        sizeLeft += check->size;
        check->magic = MAGIC_FREE;
        free_list_Ptr = check;
    }else {
        //case left out for any broken or overwritten memory
        fprintf(stderr, "Memory Overwritten, Segmentation Fault\n");
        abort();
    }
    //check if there is a free node next the the current pointer
    while ((check->prev->magic == MAGIC_FREE
           || check->next->magic == MAGIC_FREE
            || check->magic == MAGIC_FREE)
           && merge == MERGE) {
            merge = ALL_CLEAR;
        if (check->size != sizeLimit) {
            //reset the whole structure if only two node
            if ((check->size == sizeLimit/BASE
                && check->next->size == sizeLimit/BASE
                && check->next->magic == MAGIC_FREE
                && check->magic == MAGIC_FREE)
                ) {
                top->size = sizeLimit;
                top->next = top;
                top->prev = top;
                free_list_Ptr = top;
                merge = MERGE;
            }
            if (check->next->magic == MAGIC_FREE
                && check->next != top
                && (check->size + check->next->size) == BASE*check->size) {
                check->size += check->next->size;
                check->next->next->prev = check;
                check->next = check->next->next;
                merge = MERGE;
                free_list_Ptr = check;
            }
            if (check->prev->magic == MAGIC_FREE
                && check != top
                && (check->size + check->prev->size) == BASE*check->size) {
                free_list_Ptr = check->prev;
                check->prev->size += check->size;
                check->next->prev = check->prev;
                check->prev->next = check->next;
                merge = MERGE;
                check = free_list_Ptr;
            }
        }
    }
}

void allocator_end(void){
    //printf("\n\n\nStart Freeing Memory, Memory Structure Before freed\n");
    //printMemoBar();
    free(memory);
    memory = NULL;
    free_list_Ptr = NULL;
    assert(memory == NULL);
    //printf("\nMemory has been Freed, Program Exit Normally\n");
}

static u_int32_t sizeReq (u_int32_t size){
    u_int32_t tmp = size-1;
    u_int32_t answer = 0;
    if (size == 1) {
        answer = 1;
    }else {
        while (tmp != 1 && tmp != 0) {
            tmp = tmp >> 1;
            answer++;
        }
        if (tmp == 1) {
            answer++;
        }
    }
    return pow(BASE, answer);
}


void print_memory (void){
    if (memory == NULL) {
        printf("Memory allocator not running.\n");
    }
    printf("Mem print:\n");
    int i = 0;
    int j;
    int k = 0;
    byte *data;
    Admin check = (Admin) memory;
    do {
        if (k % BASE == 0) {
            printf(F_BLUE);
        } else {
            printf(F_MAGNETA);
        }
        k++;
        printf("%d: %d bytes: (%x)\n\t", i, check->size, check->magic);
        printf("prev %p\tnext %p\n\t",check->prev, check->next);
        printf("%ld back.\t%ld forward.\n\t",
               (long)check - (long)check->prev,
               (long)check->next - (long) check);
        if (check->magic == MAGIC_FREE) {
            printf("(free)\n\n");
        } else if (check->magic == MAGIC_IN_USE) {
            data = ((byte*)check) + ADMIN_SIZE;
            for (j = 0; j < check->size; j++) {
                printf("%2x  ", data[j]);
                if (j % 8 == 7) {
                    printf("\n\t");
                }
            }
            printf("\n");
        } else {
            printf(ORIGINAL);
            printf("Seg Fault\n");
            abort();
        }
        //used for testing
        //printf("\x1b[32m""check address beforeis %p\n", check);
        check = check->next;
        //printf("check size afteris %d\n", check->size);
        //printf("check address afteris %p\n""\x1b[0m", check);
        i++;
    } while (check != (Admin) memory);
    printf(ORIGINAL);
    printMemoBar();
}

static void printMemoBar (void){
    //print memory bar
    int i;
    int j;
    int tmp;
    printf("Printing Memory Structure in a bar format\n");
    Admin check = (Admin) memory;
    for (i = 0; i < 4; i++){
        printf(F_WHITE);
        do{
            if (check->magic == MAGIC_FREE && check != free_list_Ptr) {
                printf(B_GREEN "%d", check->size);
            } else if (check->magic == MAGIC_IN_USE && check != free_list_Ptr) {
                printf(B_RED "%d", check->size);
            } else if (check == free_list_Ptr && check->magic == MAGIC_FREE){
                printf(F_MAGNETA B_GREEN "%d", check->size);
            }else if (check == free_list_Ptr && check->magic == MAGIC_IN_USE){
                printf(F_MAGNETA B_RED "%d", check->size);
            }
            tmp = (check->size)/64;
            for (j=0; j<=tmp; j++){
                printf(B_BLUE "  ");
            }
            printf(B_CYAN " " F_WHITE);
            check = check->next;
        } while (check != (Admin) memory);
        printf(ORIGINAL);
        printf("\n");
    }
     
}

/*
static int sizeReq (u_int32_t size){
    u_int32_t tmp,answer = 0;
    for (tmp=size-1; tmp >1; tmp>>=1, answer++) {}
    return tmp == 1 ? answer+1: answer;
}
*/