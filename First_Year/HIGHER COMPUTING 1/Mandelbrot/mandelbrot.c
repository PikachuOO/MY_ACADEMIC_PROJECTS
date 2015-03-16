//
//  mandelbrot.c
//  Mandelbrot
//
//  Created by Jiashu Chen and Shiyun (Patrick) Zhu on 29/03/12.
//  Task2 Group Task
//  Copyright (c) 2012 University of New South Wales. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
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

typedef struct _color{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
}color;

int iterationOfPoint (double hx, double hy, double startX, double startY, int zoomLevel);
int waitForConnection (int serverSocket);
int makeServerSocket (int portno);
void serveBMP (int socket, int zoomLevel, double startX, double startY);
void serveHtml(int socket);


int main (int argc, char *argv[]) {

    double startX, startY;
    int zoomLevel;
    int port = DEFAULT_PORT;

    printf ("************************************\n");
    printf ("Starting simple server %f\n", SIMPLE_SERVER_VERSION);
    printf ("Serving bmps since 2012\n");

    if (argc > CHANGE_MANUAL_SERVER_PORT) {
        port = atoi (argv[CHANGE_MANUAL_SERVER_PORT]);
    }

    // use to change port name if the port is already occupied, so multiple compilation will no be required

    int serverSocket = makeServerSocket (port);
    printf ("Access this server at http://localhost:%d/\n", port);
    printf ("************************************\n");

    char request[REQUEST_BUFFER_SIZE];
    int numberServed = 0;
    while (numberServed < NUMBER_OF_PAGES_TO_SERVE) {

        printf ("*** So far served %d pages ***\n", numberServed);

        int connectionSocket = waitForConnection (serverSocket);
        // wait for a request to be sent from a web browser, open a new
        // connection for this conversation

        // read the first line of the request sent by the browser

        int bytesRead;
        bytesRead = read (connectionSocket, request, (sizeof request) - 1);
        assert (bytesRead >= 0);

        printf("%s",request);

        // were we able to read any data from the connection?

        // print entire request to the console
        printf (" *** Received http request ***\n %s\n", request);
        //send the browser a simple html page using http

        printf (" *** Sending http response ***\n");
        if (request[5] == 'X' && request[6] == '-') {
            //double check to ensure if the input is just "lcoalhost:1585/X" the program will still use the java script
            sscanf(request, "GET /X-%lf-%lf-%d.bmp HTTP/1.1",&startX, &startY, &zoomLevel);
            printf("startX is %lf, startY is %lf, zoomLevel is %d\n", startX, startY, zoomLevel);
            printf("%c", request[5]);
            serveBMP(connectionSocket, zoomLevel, startX, startY);

        }else {
            serveHtml(connectionSocket);
        }

        // close the connection after sending the page- keep aust beautiful
        close(connectionSocket);


        numberServed++;
    }

    // close the server connection after we are done- keep aust beautiful
    printf ("** shutting down the server **\n");
    close (serverSocket);

    return EXIT_SUCCESS;
}

void serveHtml(int socket){

    char* message;
    message = "HTTP/1.0 200 OK\r\n"
              "Content-Type: text/html\r\n"
              "\r\n";
    printf ("about to send=> %s\n", message);
    write (socket, message, strlen (message));

    message = "<html> <script src= \"https://openlearning.cse.unsw.edu.au/site_media/viewer/tile.min.js\"></script> </html>";
    printf ("about to send=> %s\n", message);
    write (socket, message, strlen (message));
}



void serveBMP (int socket, int zoomLevel, double startX, double startY) {

    char * message;
    color colorWillOutput;
    double hx, hy;
    int steps;


    // first send the http response header

    // (if you write stings one after another like this on separate
    // lines the c compiler kindly joins them togther for you into
    // one long string)

    message = "HTTP/1.0 200 OK\r\n"
    "Content-Type: image/bmp\r\n"
    "\r\n";
    printf ("about to send=> %s\n", message);
    write (socket, message, strlen (message));

    // now send the BMP
    char bmp[BMP_HEADER_SIZE] = {
        0x42,0x4D,0x36,0x00,
        0x0C,0x00,0x00,0x00,
        0x00,0x00,0x36,0x00,
        0x00,0x00,0x28,0x00,
        0x00,0x00,0x00,0x02,
        0x00,0x00,0x00,0x02,
        0x00,0x00,0x01,0x00,
        0x18,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x04,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x01,
        0x00,0x00,0x00,0x00,
        0x00,0x00,
        };

    write (socket, bmp, BMP_HEADER_SIZE);

    for (hy = -HALVED_RESOLUTION; hy < HALVED_RESOLUTION; hy++)  {
        for (hx = -HALVED_RESOLUTION; hx < HALVED_RESOLUTION; hx++)  {

            steps = iterationOfPoint(hx, hy, startX, startY, zoomLevel);
            colorWillOutput = (color) {stepsToBlue(steps), stepsToGreen(steps), stepsToRed(steps)};

            unsigned char bmpPixels[BMP_PIXEL_SIZE] = {
                colorWillOutput.blue,
                colorWillOutput.green,
                colorWillOutput.red,
            };

            write(socket, bmpPixels, sizeof(bmpPixels));
        }
    }
}
// calculating the iteration of Points
int iterationOfPoint (double hx, double hy, double startX, double startY, int zoomLevel){
    double z, cx, cy, x, y;
    int iteration;

    cx = hx * (pow(2, -zoomLevel)) + startX - POSITION_SHIFT;//no #define so the formular is clearer here
    cy = hy * (pow(2, -zoomLevel)) + startY;//no #define so the formular is clearer here

    x = ORIGIN;
    y = ORIGIN;
    iteration = INITIAL;
        while (x * x + y * y < TERMINATE_VALUE && iteration < EXHIBITION_ITERATIONS) {
            z = x * x - y * y + cx; //complex number calculation (x+iy)^2 = x^2 - y^2 +2xyi
            y = 2.0 * x * y + cy; //no #define so the formular is clearer here
            x = z;
            iteration ++;
        }

    return iteration;
}


// start the server listening on the specified port number
int makeServerSocket (int portNumber) {

    // create socket
    int serverSocket = socket (AF_INET, SOCK_STREAM, 0);
    assert (serverSocket >= 0);
    // error opening socket

    // bind socket to listening port
    struct sockaddr_in serverAddress;
    bzero ((char *) &serverAddress, sizeof (serverAddress));

    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port        = htons (portNumber);

    // let the server start immediately after a previous shutdown
    int optionValue = 1;
    setsockopt (
                serverSocket,
                SOL_SOCKET,
                SO_REUSEADDR,
                &optionValue,
                sizeof(int)
                );

    int bindSuccess =
    bind (
          serverSocket,
          (struct sockaddr *) &serverAddress,
          sizeof (serverAddress)
          );

    assert (bindSuccess >= 0);
    // if this assert fails wait a short while to let the operating
    // system clear the port before trying again

    return serverSocket;
}

// wait for a browser to request a connection,
// returns the socket on which the conversation will take place
int waitForConnection (int serverSocket) {
    // listen for a connection
    const int serverMaxBacklog = 10;
    listen (serverSocket, serverMaxBacklog);

    // accept the connection
    struct sockaddr_in clientAddress;
    socklen_t clientLen = sizeof (clientAddress);
    int connectionSocket =
    accept (
            serverSocket,
            (struct sockaddr *) &clientAddress,
            &clientLen
            );

    assert (connectionSocket >= 0);
    // error on accept

    return (connectionSocket);
}