//
//  NextThursday.c
//  NextThursday
//
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

#define JANUARY 1
#define FEBRUARY 2
#define MARCH 3
#define APRIL 4
#define MAY 5
#define JUNE 6
#define JULY 7
#define AUGUST 8
#define SEPTEMBER 9
#define OCTOBER 10
#define NOVEMBER 11
#define DECEMBER 12


//Determing leap year or not
int isLeapYear(int year) {

    int yesOrNo;

    assert(year >= START_GREGORIAN_CALENDAR);

    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {

        yesOrNo = 1;

    } else {

        yesOrNo = 0;
    }
    return yesOrNo;
}

//Determining the doomsdsay of a year
int yearToDoomsday (int year){

    int doomsday;

    doomsday = ((TUESDAY + year + (year / 4) - (year / 100) + (year / 400)) % 7);

    return doomsday;

}
//Determining the day of week
int dayOfWeek (int doomsday, int leapYear, int month, int day){

    int whatDay;

    if (month < MARCH) {
        if (leapYear == 1 && (month == JANUARY)) {
            whatDay = (doomsday + ((day - 4 + 7) % 7)) % 7;
        } else if (leapYear == 0 && (month == JANUARY)){
            whatDay = (doomsday + ((day - 3 + 7) % 7)) % 7;
        } else if (leapYear == 1 && month == FEBRUARY) {
            whatDay = (doomsday + ((day - 1 + 7) % 7)) % 7;
            //only thing left here is February within non-leap year.
        } else {
            whatDay = (doomsday + ((day + 7) % 7)) % 7;
        }

        //To eliminate chance of mod7 with a negative number "e.g. ((day=1)-7)%7 = -6%7", add a positive multple of 7 after (day - (?) + 7)
        //Deal with March now.
    } else if(month == MARCH) {
        whatDay = (doomsday + ((day + 7) % 7)) % 7;
        //Now get rid of the even months
    } else if (month > MARCH && month % 2 == 0) {
        whatDay = (doomsday + ((day - month + 14) % 7)) % 7;
        // Only odd months left
    } else if (month == MAY) {
        whatDay = (doomsday + ((day - 9 +14) % 7)) % 7;
    } else {
        if (month == SEPTEMBER) {
            whatDay = (doomsday + ((day - 5 + 7) % 7)) % 7;
        } else if (month == JULY) {
            whatDay = (doomsday + ((day - 11 + 14) % 7)) % 7;
        }else {
            //November is the only month left
            whatDay = (doomsday + ((day - 7 + 7) % 7)) % 7;
        }
    }
    return whatDay;
}


// given a date after START_GREGORIAN_CALENDAR this function returns the number of days until the next thursday.

int daysToNextThursday (int year, int month, int day){

    int numberOfDays;

    numberOfDays = (7 - dayOfWeek(yearToDoomsday(year), isLeapYear(year), month, day));

    return numberOfDays;

}