////////////////////////////////////////////////////////////////////////////////
// Main File:        generate_magic.c
// This File:        generate_magic.c
// Other Files:      N/A
// Semester:         CS 354 Spring 2018
//
// Author:           Nick Klabjan
// Email:            klabjan@wisc.edu
// CS Login:         klabjan
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
    int size;
    int **array;
} Square;

int get_square_size();
Square * generate_magic(int size);
void write_to_file(char *filename, Square * square);

/**
 * Creates a square structure then writes it to a file
 *
 * @param argc the number of arguments
 * @param argv the list of arguments
 * @return 0 to quit the program
 */
int main(int argc, char *argv[])                 
{
    //if there aren't two arguments
    if (argc !=2) {
        printf("Usage: ./generate_magic <filename>\n");
        exit(1);
    }

    //creates the square with the size given by user
    Square *s = generate_magic(get_square_size());

    //writes the square to the file specified
    write_to_file(*(argv + 1),s);

    //frees up the memory of square
    for (int i = 0; i < s->size; i++) {
        free(*(s->array + i));
    }
    free(s->array);
    free(s);

    return 0;
}

/**
 * Prompts the user for the magic square size
 * checks if it is an odd number >= 3 and returns the number
 *
 * @return the size of the square
 */
int get_square_size()            
{
    //int variable to store the size of the square
    int size = 0;
    //prompts user to enter a size value
    printf("Enter size of magic square, must be odd\n");
    //sets the user's value to size
    scanf("%d", &size);

    //if the size is even and not greater than or equal to 3 print out message
    if (size < 3 || size % 2 == 0) {
        printf("Size must be an odd number >= 3.\n");
        //exists program
        exit(1);
    }

    else {
        return size;
    }
}

/**
 * Constructs a magic square of size n
 * using the Siamese algorithm.
 *
 * @param n, the size of the magic square
 * @return the magic square
 */
Square * generate_magic(int n)           
{
    //creates a new square structure
    Square *s = malloc(sizeof(Square));
    //sets n to the square size
    s->size = n;

    //initializes the rows in the Square's 2D array
    s->array = (int**)malloc(sizeof(int*)*(s->size));
    //for each row in square
    for(int i = 0; i < s->size; i++) {
        *(s->array+i) = (int*)malloc(sizeof(int) * s->size);
        //for each col in the square
        for(int col = 0; col < s->size; col++) {
            //set every element to be -1
            *(*(s->array+i)+col) = -1;
        }
    }

    //stores the row in array
    int row = 0;
    //stores the column in array
    int col = s->size/2;
    //sets the middle of the top most row to 1
    *(*(s->array+0)+(s->size/2)) = 1;
    //the number to be filled into the square
    int numToBeFilled = 2;

    //while all the elements in array haven't been filled
    while(numToBeFilled <= (s->size*s->size)) {

        //set the new position for the new number
        int newRow = row - 1;
        int newCol = col + 1;

        //if the newRow isn't valid
        if (newRow < 0)
            //set newRow to be the last row
            newRow = s->size - 1;
        //if the newCol isn't valid
        if (newCol == s->size)
            //set newCol to the first column
            newCol = 0;

        // if there is a -1 in that spot already
        if (*(*(s->array+newRow)+newCol) == -1) {
            //set the new number to that location
            *(*(s->array + newRow) + newCol) = numToBeFilled;
            //update row and col
            row = newRow;
            col = newCol;
        }
        //if there is a value there already
        else {
            //increment row
            row++;
            //set the new number to the row and col in Square
            *(*(s->array+row)+col) = numToBeFilled;
        }
        //increments to the new number to be added to the square
        numToBeFilled++;
    }
    //returns the square
    return s;
}

/**
 * Opens up a new file(or overwrites the existing file)
 * and writes out the square in the format expected by verify_hetero.c.
 *
 * @param filename, the file to have output printed to
 * @param square, the square to be printed to the output file
 */
void write_to_file(char *filename, Square * square)              
{
    //opens file to be written to
    FILE *file = fopen(filename, "w");

    //if the file is null, then print that it can't open the file
    if (file == NULL) {
        printf("Cannot open file for writing.\n");
        exit(1);
    }

    //prints the size of square to the file
    fprintf(file,"%d\n",square->size);

    //for each row in square
    for(int row = 0;row < square->size; row++){
        //for each column in square
        for(int col = 0;col < square->size; col++){
            //if it is the last number in the row
            if (col == square->size-1)
                //print out the number to the file without a comma
                fprintf(file, "%d",*(*(square->array+row)+col));
            else
                //print out the number to the file with a comma
                fprintf(file, "%d,",*(*(square->array+row)+col));
        }
        //new line in the output file
        fprintf(file, "\n");
    }
    //close the file
    fclose(file);
}
