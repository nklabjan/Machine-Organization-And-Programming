////////////////////////////////////////////////////////////////////////////////
// Main File:        verify_hetero.c
// This File:        verify_hetero.c
// Other Files:      N/A
// Semester:         CS 354 Spring 2018
//
// Author:           Nick Klabjan
// Email:            klabjan@wisc.edu
// CS Login:         klabjan
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
    int size;
    int **array;
} Square;

Square * construct_square(char *filename);
int verify_hetero(Square * square);
void insertion_sort(int* arr, int size);

/**
 * Takes in a file with a square and checks to see if that
 * square is hetero square.
 *
 * @param argc, number of arguments
 * @param argv, list of arguments
 * @return 0 to shut down program
 */
int main(int argc, char *argv[])                        
{
    //if there aren't two arguments, print error message
    if (argc !=2) {
        printf("Usage: ./verify_hetero <filename>\n");
        exit(1);
    }

    //constructs a new square based on input file
    Square *s = construct_square(*(argv+1));

    //if the square is a hetero square, print "true"
    if(verify_hetero(s) == 1)
        printf("true\n");
    else
        //else print false
        printf("false\n");

    //frees up the memory of square
    for (int i = 0; i < s->size; i++) {
        free(*(s->array + i));
    }
    free(s->array);
    free(s);

    return 0;
}

/**
 * Reads the input file to initialize a square struct
 * from the contents of the file and returns the square.
 * The format of the file is defined in the assignment specifications
 */
Square * construct_square(char *filename)                
{
    //opens the file
    FILE *file = fopen(filename, "r");

    //if the file is null, iw. doesn't exist
    if (file == NULL){
        //close the file
        fclose(file);
        printf("Cannot open file for reading.\n");
        //exit program
        exit(1);
    }

    //creates a new square structure
    Square *s = malloc(sizeof(Square));

    //sets aside memory for the buffer
    char buffer[512];

    //gets the size of the square, ie. number of rows
    s->size = atoi(fgets(buffer,512,file));

    //allocates memory for the square's 2D array
    s->array = (int**)malloc(sizeof(int*) * s->size);
    //traverses through the rows
    for(int i = 0; i < s->size; i++) {
        //allocates memory for each row since it's a 2D array
        *(s->array + i) = (int*)malloc(sizeof(int) * s->size);
    }

    //stores the row in the 2D array
    int i = 0;
    //stores the col in the 2D array
    int j = 0;
    //stores the first line of 2D array from the file
    char *line = fgets(buffer,512,file);
    //stores a character from a line
    char *ch = NULL;


    //while there is a line in the file
    while(line != NULL) {
        //stores a character from that line
        ch = strtok(line, ",");
        //while the character isn't NULL
        while (ch != NULL) {
            //add character to 2D array
            *(*(s->array+i)+j) = atoi(ch);
            //goes to the next character in line
            ch = strtok(NULL, ",");
            //increments column
            j++;
            //if max column reached set it to 0
            if (j == s->size)
                j = 0;
        }
        //stores a line from the file
        line = fgets(buffer,512,file);
        //increment row
        i++;
    }
    //returns the square
    return s;
}

/* verify_hetero verifies if the square is a heterosquare
 * 
 * returns 1(true) or 0(false)
 */
int verify_hetero(Square * square)               
{
    //if the size of the square is 1, return 0
    if (square->size == 1) {
        return 0;
    }

    //size of the array to store the sums
    int sizeOfArraySums = (square->size * 2) + 2;

    //allocated memory for the array that will store the sums
    int *arraySums = (int*)malloc(sizeof(int) * sizeOfArraySums);

    //stores the sum of each row in square
    int rowSum = 0;
    //keeps track of position in arraySums
    int i = 0;

    //for each row in square
    for (int row = 0; row < square->size; row++) {
        //initial rowSum to 0
        rowSum = 0;
        //for each col in square
        for (int col = 0; col < square->size; col++) {
            //calculates the sum of all the values in the same row
            rowSum += *(*(square->array + row) + col);
        }
        //adds the rowSum to the array arraySum
        *(arraySums+i) = rowSum;
        //increments position in arraySums
        i++;
    }

    //stores the sum of each col in squares
    int colSum = 0;
    //keeps track of position in arraySums
    i = square->size;
    //for each col in square
    for (int col = 0; col < square->size; col++) {
        //sets columSum to 0
        colSum = 0;
        //for each row in square
        for (int row = 0; row < square->size; row++) {
            //calculates the sum of all the values in the same row
            colSum += *(*(square->array + row) + col);
        }
        //adds the rowSum to the array arraySums
        *(arraySums+i) = colSum;
        //increments position in arraySums
        i++;
    }

    //stores the sum of the main Diagonal
    int mainDiagonalSum = 0;
    //increments row and col at same time
    for (int j = 0; j < square->size; j++) {
        //adds each element in the main diagonal to the sum
        mainDiagonalSum += *(*(square->array+j)+j);
    }
    //adds mainDiagnolSum to arraySums
    *(arraySums+(2*square->size)) = mainDiagonalSum;

    //stores the sum of the main Diagonal
    int secondDiagonalSum = 0;
    //stores the column we want to access in array
    int col = 0;
    //increments row and col at same time
    for (int row = 0; row < square->size; row++) {
        //the column we want to check in the square
        col = square->size-row-1;
        //adds each element in the main diagonal to the sum
        secondDiagonalSum += *(*(square->array+row)+col);
    }
    //adds mainDiagnolSum to arraySums
    *(arraySums+(2*square->size+1)) = secondDiagonalSum;

    //sorts the array arraySums in increasing order
    insertion_sort(arraySums, sizeOfArraySums);

    //for each element n arraySums
    for (int i = 0; i < sizeOfArraySums-1; i++) {
        //if two of elements are the same
        if ( *(arraySums + i) == *(arraySums + i + 1)) {
            //free up arraySums memory
            free(arraySums);
            //return 0
            return 0;
        }
    }

    //free up arraySums memory
    free(arraySums);
    //return 1
    return 1;
}

/* insertion_sort sorts the arr in ascending order
 *
 */
void insertion_sort(int* arr, int size)                  
{
    //stores a value from the unsorted part of arr
    int value = 0;
    //stores which element in arr to analyze
    int j = 0;
    //for each element in arr starting with the second element
    for (int i = 1; i < size; i++)
    {
        //stores unsorted element to value
        value = *(arr+i);
        //sets j to point to a sorted element in arr
        j = i-1;
        //while j is greater than 0 and element in sorted array is bigger than value
        while (j >= 0 && *(arr+j) > value)
        {
            //assigns the smaller value to be in front of bigger value
            *(arr+j+1) = *(arr+j);
            //decrements j
            j--;
        }
        //sets value to that position in they array
        *(arr+j+1) = value;
    }
}
