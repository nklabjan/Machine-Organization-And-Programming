////////////////////////////////////////////////////////
//
// File name: intdate.c
//
// Author: Nick Klabjan
// CS Login: klabjan
//
////////////////////////////////////////////////////////
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// global variables for counter and alarm time
int counter = 0;
int alarm_time = 3;

/**
 * Method that's called when SIGALRM signal is sent.
 */
void sigalrm_handler() {
    time_t rtime;
    time(&rtime);
    printf("PID: %d | Current Time: %s", getpid(), ctime(&rtime));
    alarm(alarm_time);
}

/**
 * Method that's called when SIGUSR1 signal is sent.
 */
void sigusr1_handler() {
    counter++;
    printf("SIGUSR1 caught!\n");
}

/**
 * Method that's called when SIGINT signal is sent.
 */
void sigint_handler() {
    printf("\nSIGINT received.\n");
    printf("SIGUSR1 was received %d times. Exiting now.\n", counter);
    exit(0);
}

int main() {
    // creates and intiliazes SIGARLM
    struct sigaction sa;
    memset(&sa,0,sizeof(sa));
    sa.sa_handler = sigalrm_handler;
    sigaction(SIGALRM, &sa, NULL);
    sa.sa_flags = 0;

    // creates and intiliazes SIGUSR1
    struct sigaction sa1;
    memset(&sa1, 0, sizeof(sa1));
    sa1.sa_handler = sigusr1_handler; 
    sigaction(SIGUSR1, &sa1, NULL);
    sa1.sa_flags = 0;

    // creates and intiliazes SIGINT
    struct sigaction sa2;
    memset(&sa2, 0, sizeof(sa2));
    sa2.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa2, NULL);
    sa2.sa_flags = 0;

    // checks to make sure all signals are binding correctly
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
   	    printf("Error binding to SIG_ALRM Handler!\n");
   	    exit(1);
    }

    if (sigaction(SIGINT, &sa2, NULL) == -1) {
        printf("Error binding to SIGINT Handler!\n");
        exit(1);
    }

    if (sigaction(SIGUSR1, &sa1, NULL) == -1) {
        printf("Error binding to SIGUSR1 Handler!\n");
        exit(1);
    }

    printf("Pid and time will be printed every 3 seconds.\n");
    printf("Enter ^C to end the program.\n");
    // keeps repeating and SIGALRM is sent every 3 seconds
    alarm(alarm_time);
    while(1) {
    }
}
