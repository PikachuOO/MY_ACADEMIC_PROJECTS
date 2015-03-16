//
//  pixelColor.c
//  Mandelbrot
//
//  Created by Jiashu Chen and Shiyun (Patrick) Zhu on 29/03/12.
//  Task2 Group Task
//  Copyright (c) 2012 University of New South Wales. All rights reserved.
//

//Reason change Color Setting is because "%" takes a lot of times to calculate

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "pixelColor.h"

#define SIMPLE_SERVER_VERSION 1.0
#define REQUEST_BUFFER_SIZE 1000
#define DEFAULT_PORT 1585 //last 4 digit of my student number (zNumber)
#define NUMBER_OF_PAGES_TO_SERVE 10000 // after serving this many pages the server will halt
#define EXHIBITION_ITERATIONS 256
#define BMP_HEADER_SIZE 54
#define BMP_PIXEL_SIZE 3
#define HALVED_RESOLUTION 256
#define ORIGIN 0.00
#define TERMINATE_VALUE 4.00
#define POSITION_SHIFT 0.5 //mandelbrot looks better (make the set to be displayed in the centre of the bmp image)
#define CHANGE_MANUAL_SERVER_PORT 1
#define BLACK_FOR_POINTS_INSIDE 0
#define KEEP_RESULT_POSITIVE 1 // make the cosine function range from -1 < cos(steps) < 1 to 0 < cos(steps)+1 < 2
#define HALF_ALL_POSSIBLE_COLOR 128//this is all color possible exist form 00 to FF in hex (which is from 0 to 256)
#define INITIAL 0



unsigned char stepsToRed (int steps){

    int answer;

    if (steps == EXHIBITION_ITERATIONS){
        answer = BLACK_FOR_POINTS_INSIDE;
    }else {
        steps = ((cos (steps / 18)) + KEEP_RESULT_POSITIVE) * HALF_ALL_POSSIBLE_COLOR;
        answer = steps;
    }

    return answer;
}

unsigned char stepsToGreen (int steps){

    int answer;

    if (steps == EXHIBITION_ITERATIONS){
        answer = BLACK_FOR_POINTS_INSIDE;
    }else {
        steps = ((cos (steps / 17)) + KEEP_RESULT_POSITIVE) * HALF_ALL_POSSIBLE_COLOR;
        answer =  steps;
    }

    return answer;
}
unsigned char stepsToBlue (int steps){

    int answer;

    if (steps == EXHIBITION_ITERATIONS){
        answer = BLACK_FOR_POINTS_INSIDE;
    }else {
        steps = ((cos (steps / 15)) + KEEP_RESULT_POSITIVE) * HALF_ALL_POSSIBLE_COLOR;
        answer = steps;
    }

    return answer;
}