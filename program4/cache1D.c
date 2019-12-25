/* Name: Nick Klabjan
 * CS login: klabjan
 * Section(s): Lecture 1
 *
 * Filename: cache1D.c
 */

int arr[100000]; // global array of integers of size 100,000

int main() {
    // iterates through the array
    for (int i = 0; i < 100000; i++)
        *(arr+i) = i; // sets the value of each element in the array as the index
    return 0;
}