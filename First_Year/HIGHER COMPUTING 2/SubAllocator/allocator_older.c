//
//  allocator_older.c
//  SubAllocator
//
//  Created by Jiashu Chen on 17/08/12.
//  Copyright (c) 2012 Jiashu Chen. All rights reserved.
//

//
//  allocator.c
//  SubAllocator
//
//  Created by Jiashu Chen on 7/08/12.
//  Copyright (c) 2012 Jiashu Chen. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "allocator.h"

#define MAGIC_FREE 0xDEADBABE
#define MAGIC_IN_USE 0xB16B00B5
#define BASE 2
#define EMPTY 0
#define ADMIN_SIZE (int) (sizeof(admin))

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

#define NEED_SPLIT 1
#define NO_NEED_SPLIT 2

typedef struct _admin *Admin;

typedef struct _admin{
    u_int32_t magic;
    u_int32_t size;
    Admin next;
    Admin prev;
} admin;

//all typedef
typedef unsigned char byte;
static byte *memory = NULL;
static Admin free_list_Ptr;
static u_int32_t sizeLimit;
static u_int32_t sizeLeft;

//function declearation extra functions that I made mostlikely to be static functions
//calculate the size really needed
static u_int32_t sizeReq (u_int32_t size);
//split memory into the appropriate size
//sizeLimit whole memory
//sizeNeede is size required
static void splitMemory(Admin start);
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
    void *ptrReturned;
    short need = NEED_SPLIT;
    //calculating sizes
    //printf("admin size + n %d\n", (u_int32_t) (ADMIN_SIZE + n));
    allocaSize = sizeReq(ADMIN_SIZE + n);
    
    if (allocaSize >= sizeLeft) {
        ptrReturned = NULL;
    }else{
        // check for full list
        do {
            check = check->next;
        } while (check->magic != MAGIC_FREE && check != free_list_Ptr);
        
        if (check == free_list_Ptr && check->magic == MAGIC_IN_USE) {
            printf("MEMORY TOTAL OCCUPIED, NO MORE FREE SPACE\n");
            printf("SEGMENTATION FAULT\n");
            fprintf(stderr, "Memory Overwritten, Segmentation Fault\n");
            ptrReturned = NULL;
            //if not full list
        }else{
            //check for matching sizes
            check = free_list_Ptr;
            do {
                check = check->next;
                if (check->size == allocaSize && check->magic == MAGIC_FREE) {
                    need = NO_NEED_SPLIT;
                }
            } while (check != free_list_Ptr && need == NEED_SPLIT);
            //in no matching sizes, then do split
            if (need == NEED_SPLIT) {
                
                printf("split is called\n");
                check = free_list_Ptr->next;
                while ((check->size <= (BASE * allocaSize) && check->magic == MAGIC_IN_USE)){
                    check = check->next;
                }
                //used for testing
                //printf("checkSize before split called is %d\n",check->size);
                //printf("allocaSize before split called is %d\n", allocaSize);
                //reassure the node being used is free
                if (check->size <= allocaSize || (check->next == free_list_Ptr && check->size == allocaSize)) {
                    //print_memory();
                    printf("MEMORY OCCUPIED or NOT ENOUGH FREE SPACE\n");
                    printf("SEGMENTATION FAULT\n");
                    allocator_end();
                }
                //do split
                //reassure again it is not the size required
                if (check->size != allocaSize) {
                    //printf("Memory Block size is %d\n" "Actual needed size is %d\n", check->size, allocaSize);
                    while (check->size < allocaSize || check->size == MAGIC_IN_USE) {
                        check = check->next;
                    }
                    
                    while (check->size/2 >= allocaSize) {
                        splitMemory(check);
                        //printf("size is %d\n", check->size);
                    }
                }
                
            }
            pointer = check;
            pointer->magic = MAGIC_IN_USE;
            sizeLeft -= pointer->size;
            printf("size left is %d\n", sizeLeft);
            printf("pointer address is %p\n", pointer);
            ptrReturned = (void*) pointer + ADMIN_SIZE;
            printf("ptrReturned address is %p\n", ptrReturned);
            //printf("\x1b[31m""pointer is %p, ptrRet is %p\n",pointer, ptrReturned);
        }
    }
    return ptrReturned;
}

static void splitMemory(Admin start){
    Admin new;
    Admin check = start;
    void *ptr;
    ptr = (void*) start;
    //problem
    ptr = ptr + (check->size)/2;
    new = (Admin) ptr;
    //allocate function goes wrong
    //need to check with other people
    new->magic = MAGIC_FREE;
    new->size = (u_int32_t)((check->size) / 2);
    //printf("CheckSize before fragmentation %d\n", check->size);
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
    void *headPtr;
    
    headPtr = object - ADMIN_SIZE;
    check = (Admin) headPtr;
    printf("inside free function check Address is %p\n", check);
    
    if (check->magic == MAGIC_FREE) {
        fprintf(stderr, "Memory has already been freed\n");
        abort();
    }else if (check->magic == MAGIC_IN_USE){
        check->magic = MAGIC_FREE;
    }else {
        fprintf(stderr, "Memory Overwritten, Segmentation Fault\n");
        abort();
    }
    
    if (check->size != sizeLimit) {
        if (check->next->magic == MAGIC_FREE
            && check->next != free_list_Ptr
            && (check->size + check->next->size) == BASE*check->size) {
            check->size += check->next->size;
            check->next->next->prev = check;
            check->next = check->next->next;
        }
        if (check->prev->magic == MAGIC_FREE
            && check != free_list_Ptr
            && check->prev != free_list_Ptr
            && (check->size + check->next->size) == BASE*check->size) {
            check->prev->size += check->size;
            check->next->next->prev = check;
            check->next = check->next->next;
        }
        if (free_list_Ptr->size == sizeLimit/BASE
            && free_list_Ptr->next->magic == MAGIC_FREE
            && free_list_Ptr->magic == MAGIC_FREE) {
            free_list_Ptr->size = sizeLimit;
            free_list_Ptr->next = free_list_Ptr;
            free_list_Ptr->prev = free_list_Ptr;
        }
    }
    
}
void allocator_end(void){
    free(memory);
    memory = NULL;
    free_list_Ptr = NULL;
    assert(memory == NULL);
    printf("\nMemory has been Freed, Program Exit Normally\n");
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
    Admin check = free_list_Ptr;
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
    } while (check != free_list_Ptr);
    printf(ORIGINAL);
    printMemoBar();
}

static void printMemoBar (void){
    //print memory bar
    //int spacer = sizeLimit/(sizeLimit/12);
    int i;
    int j;
    int tmp;
    printf("Printing Memory Structure in a bar format\n");
    Admin check = free_list_Ptr;
    for (i = 0; i < 4; i++){
        do{
            printf(F_WHITE);
            if (check->magic == MAGIC_FREE) {
                printf(B_GREEN "%d", check->size);
                tmp = (check->size)/32;
                for (j=0; j<tmp; j++){
                    printf(B_BLUE "   ");
                }
            } else if (check->magic == MAGIC_IN_USE) {
                printf(B_RED "%d", check->size);
                tmp = (check->size)/32;
                for (j=0; j<tmp; j++){
                    printf(B_BLUE "   ");
                }
            }
            printf(B_CYAN " ");
            check = check->next;
        } while (check != free_list_Ptr);
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