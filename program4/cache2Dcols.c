/* Name: Nick Klabjan
 * CS login: klabjan
 * Section(s): Lecture 1
 *
 * Filename: cache2Dcols.c
 */

int arr[3000][500]; // global 2D array of 3000 rows and 500 cols

int main() {
    // iterates through the columns
    for (int col = 0; col < 500; col++) {
        // iterates through the rows
        for (int row = 0; row < 3000; row++) {
            *(*(arr + row) + col) = row + col; // sets the value of each element in the array as the index
        }
    }
    return 0;
}

