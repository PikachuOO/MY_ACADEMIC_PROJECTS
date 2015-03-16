#include "allocator.h"
#include <assert.h>
#include <stdio.h>

#define INITIAL    1024
#define FIRST_VAL  239
#define SECOND_VAL 111
#define THIRD_VAL  15

int main(int argc, char **argv) {
    char *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8;
    int i;
    // assert(allocator_init only calls malloc once with the correct size);
    allocator_init(INITIAL);
    p1 = allocator_malloc(FIRST_VAL);
    assert (p1 !=  NULL);
    for (int i = 0; i < FIRST_VAL; i++) p1[i] = 1;
    p2 = allocator_malloc(SECOND_VAL);
    assert (p2 !=  NULL);
    for (int i = 0; i < SECOND_VAL; i++) p2[i] = 2;
    p3 = allocator_malloc(SECOND_VAL);
    assert (p3 !=  NULL);
    for (int i = 0; i < SECOND_VAL; i++) p3[i] = 3;
    p4 = allocator_malloc(SECOND_VAL);
    assert (p4 !=  NULL);
    for (int i = 0; i < SECOND_VAL; i++) p4[i] = 4;
    p5 = allocator_malloc(THIRD_VAL);
    assert (p5 !=  NULL);
    for (int i = 0; i < THIRD_VAL; i++) p5[i] = 5;
    p6 = allocator_malloc(THIRD_VAL);
    assert (p6 !=  NULL);
    for (int i = 0; i < THIRD_VAL; i++) p6[i] = 6;
    p7 = allocator_malloc(THIRD_VAL);
    assert (p7 !=  NULL);
    for (int i = 0; i < THIRD_VAL; i++) p7[i] = 7;
    p8 = allocator_malloc(THIRD_VAL);
    assert (p8 !=  NULL);
    for ( i = 0; i < THIRD_VAL; i++) p8[i] = 8;
    for ( i = 0; i < FIRST_VAL; i++) assert(p1[i] == 1);
    for ( i = 0; i < SECOND_VAL; i++) assert(p2[i] == 2);
    for ( i = 0; i < SECOND_VAL; i++) assert(p3[i] == 3);
    for ( i = 0; i < SECOND_VAL; i++) assert(p4[i] == 4);
    for ( i = 0; i < THIRD_VAL; i++) assert(p5[i] == 5);
    for ( i = 0; i < THIRD_VAL; i++) assert(p6[i] == 6);
    for ( i = 0; i < THIRD_VAL; i++) assert(p7[i] == 7);
    for ( i = 0; i < THIRD_VAL; i++) assert(p8[i] == 8);
    // assert (the pointers returned make sense)
    assert(allocator_malloc(FIRST_VAL) == NULL);
    allocator_free(p5);
    allocator_free(p4);
    allocator_free(p1);
    allocator_free(p3);
    allocator_free(p6);
    allocator_free(p2);
    allocator_free(p7);
    allocator_free(p8);
    p1 = allocator_malloc(FIRST_VAL);
    assert (p1 !=  NULL);
    for (int i = 0; i < FIRST_VAL; i++) p1[i] = 1;
    p2 = allocator_malloc(SECOND_VAL);
    assert (p2 !=  NULL);
    for (int i = 0; i < SECOND_VAL; i++) p2[i] = 2;
    p3 = allocator_malloc(SECOND_VAL);
    assert (p3 !=  NULL);
    for (int i = 0; i < SECOND_VAL; i++) p3[i] = 3;
    p4 = allocator_malloc(SECOND_VAL);
    assert (p4 !=  NULL);
    for (int i = 0; i < SECOND_VAL; i++) p4[i] = 4;
    p5 = allocator_malloc(THIRD_VAL);
    assert (p5 !=  NULL);
    for (int i = 0; i < THIRD_VAL; i++) p5[i] = 5;
    p6 = allocator_malloc(THIRD_VAL);
    assert (p6 !=  NULL);
    for (int i = 0; i < THIRD_VAL; i++) p6[i] = 6;
    p7 = allocator_malloc(THIRD_VAL);
    assert (p7 !=  NULL);
    for (int i = 0; i < THIRD_VAL; i++) p7[i] = 7;
    p8 = allocator_malloc(THIRD_VAL);
    assert (p8 !=  NULL);
    for (int i = 0; i < THIRD_VAL; i++) p8[i] = 8;
    for (int i = 0; i < FIRST_VAL; i++) assert(p1[i] == 1);
    for (int i = 0; i < SECOND_VAL; i++) assert(p2[i] == 2);
    for (int i = 0; i < SECOND_VAL; i++) assert(p3[i] == 3);
    for (int i = 0; i < SECOND_VAL; i++) assert(p4[i] == 4);
    for (int i = 0; i < THIRD_VAL; i++) assert(p5[i] == 5);
    for (int i = 0; i < THIRD_VAL; i++) assert(p6[i] == 6);
    for (int i = 0; i < THIRD_VAL; i++) assert(p7[i] == 7);
    for (int i = 0; i < THIRD_VAL; i++) assert(p8[i] == 8);
    // assert (the pointers returned make sense)

    printf("ALL TESTS PASSED\n");
    return 0;
}
