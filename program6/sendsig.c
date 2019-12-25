////////////////////////////////////////////////////////
//
// File name: sendsig.c
//
// Author: Nick Klabjan
// CS Login: klabjan
//
////////////////////////////////////////////////////////



#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * This program sends either a SIGUSR1 or SIGINT signal given PID value.
 *
 * @param argc, length of program arguments
 * @param argv,
 * @return 0, quits program
 */
int main (int argc, const char *argv[]) {

    // checks if correct number of arguments were passed in
    if (argc != 3) {
        printf("Usage: <signal type> <pid>\n");
        exit(1);
    }

    // intializes pid value to second argument
    int pid = atoi(argv[2]);

    // sees if first argument is "-1"
    if (strcmp(argv[1], "-i") == 0) {
        kill(pid, SIGINT);
        exit(0);
    }
    // checks to see if first argument is "-u"
    else if (strcmp(argv[1], "-u") == 0) {
        kill(pid, SIGUSR1);
        exit(0);
    }
    else
        printf("Usage: <signal type> <pid>\n");
    return 0;
}
