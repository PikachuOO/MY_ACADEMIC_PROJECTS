//
//  main.c
//  NextThursday
//  Created by Jiashu Chen on 08/03/12.
//  Copyright (c) 2012 University of New South Wales. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NextThursday.h"

#define THURSDAY 0
#define FRIDAY 1
#define SATURDAY 2
#define SUNDAY 3
#define MONDAY 4
#define TUESDAY 5
#define WEDNESDAY 6

void assertCases(int year, int month, int day);

int main(int argc, char * argv[]){

    int day;
    int month;
    int year;

    printf("Enter a Date in format of DD/MM/YYYY\n");
    scanf("%d/%d/%d", &day, &month, &year);
    assertCases(year, month, day);
    printf("year is %d\n",year);
    printf("day is %d\n",  day);
    printf("month is %d\n", month);
    assert(year >= START_GREGORIAN_CALENDAR);
    printf("You year is %d\n", year);
    printf("Year you entered is %d\n", isLeapYear(year));
    printf("Doomsday of the year is %d\n", yearToDoomsday(year));
    printf("Day of Week is %d\n", dayOfWeek(yearToDoomsday(year), isLeapYear(year), month, day));
    printf("Day until next Thursday is %d\n", daysToNextThursday(year, month, day));

    return EXIT_SUCCESS;
}

void assertCases(int year, int month, int day){

    assert(daysToNextThursday(2012,1,1)==7-3);
    assert(daysToNextThursday(2012,2,1)==7-6);
    assert(daysToNextThursday(2012,3,1)==7-0);
    assert(daysToNextThursday(2012,4,1)==7-3);
    assert(daysToNextThursday(2012,5,1)==7-5);
    assert(daysToNextThursday(2012,6,1)==7-1);
    assert(daysToNextThursday(2012,7,1)==7-3);
    assert(daysToNextThursday(2012,8,1)==7-6);
    assert(daysToNextThursday(2012,9,1)==7-2);
    assert(daysToNextThursday(2012,10,1)==7-4);
    assert(daysToNextThursday(2012,11,1)==7-0);
    assert(daysToNextThursday(2012,12,1)==7-2);
    assert(daysToNextThursday(2011,1,1)==7-2);
    assert(daysToNextThursday(2011,2,1)==7-5);
    assert(daysToNextThursday(2011,3,1)==7-5);
    assert(daysToNextThursday(2011,4,1)==7-1);
    assert(daysToNextThursday(2011,5,1)==7-3);
    assert(daysToNextThursday(2011,6,1)==7-6);
    assert(daysToNextThursday(2011,7,1)==7-1);
    assert(daysToNextThursday(2011,8,1)==7-4);
    assert(daysToNextThursday(2011,9,1)==7-0);
    assert(daysToNextThursday(2011,10,1)==7-2);
    assert(daysToNextThursday(2011,11,1)==7-5);
    assert(daysToNextThursday(2011,12,1)==7-0);
}