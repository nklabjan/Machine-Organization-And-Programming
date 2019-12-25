//////////////////////////////////////////////////////////
//
// File Name: mem.c
//
// Author: Nick Klabjan
// CS Email: klabjan@cs.wisc.edu
// ID#: 9074842692
//
// Description: Implements the functoin malloc and free.
//
//////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"
#include <stdbool.h>

/*
 * This structure serves as the header for each allocated and free block
 * It also serves as the footer for each free block
 * The blocks are ordered in the increasing order of addresses 
 */
typedef struct blk_hdr {                         
        int size_status;

    /*
    * Size of the block is always a multiple of 8
    * => last two bits are always zero - can be used to store other information
    *
    * LSB -> Least Significant Bit (Last Bit)
    * SLB -> Second Last Bit 
    * LSB = 0 => free block
    * LSB = 1 => allocated/busy block
    * SLB = 0 => previous block is free
    * SLB = 1 => previous block is allocated/busy
    * 
    * When used as the footer the last two bits should be zero
    */

    /*
    * Examples:
    * 
    * For a busy block with a payload of 20 bytes (i.e. 20 bytes data + an additional 4 bytes for header)
    * Header:
    * If the previous block is allocated, size_status should be set to 27
    * If the previous block is free, size_status should be set to 25
    * 
    * For a free block of size 24 bytes (including 4 bytes for header + 4 bytes for footer)
    * Header:
    * If the previous block is allocated, size_status should be set to 26
    * If the previous block is free, size_status should be set to 24
    * Footer:
    * size_status should be 24
    * 
    */
} blk_hdr;

/* Global variable - This will always point to the first block
 * i.e. the block with the lowest address */
blk_hdr *first_blk = NULL;

/*
 * Note: 
 *  The end of the available memory can be determined using end_mark
 *  The size_status of end_mark has a value of 1
 *
 */

/* 
 * Function for allocating 'size' bytes
 * Returns address of allocated block on success 
 * Returns NULL on failure 
 * Here is what this function should accomplish 
 * - Check for sanity of size - Return NULL when appropriate 
 * - Round up size to a multiple of 8 
 * - Traverse the list of blocks and allocate the best free block which can accommodate the requested size 
 * - Also, when allocating a block - split it into two blocks
 * Tips: Be careful with pointer arithmetic 
 */
void* Mem_Alloc(int size) {
    // if size is bigger than what's possible and if size is less than 1
    if (size > 131070 || size < 1) {
        // return null
        return NULL;
    }

    // updates the size of allocation to include the header
    int size_Of_Allocation = size + 4;

    // size of padding
    int size_Of_Padding = 0;
    // if the size of allocation is not a multiple of 8
    if (size_Of_Allocation % 8 != 0) {
        // calculate the current padding needed
        size_Of_Padding = 8 - (size_Of_Allocation % 8);
        // add that padding to the size of allocation
        size_Of_Allocation += size_Of_Padding;
    }

    // block header pointer
    blk_hdr *ptr = NULL;
    // true if end of heap is reached
    bool is_End_Of_Heap = false;
    // block header pointer that points to the next
    // header after smallest free block
    blk_hdr *next_Blk_After_Free = NULL;
    // points to the next block header
    blk_hdr *next_Ptr = NULL;
    // block header pointer that points to smallest free block
    blk_hdr *smallest_Free_Blk = NULL;
    // block header pointer that points to the footer
    blk_hdr *footer_Ptr = NULL;
    // size of smallest free block
    int size_Of_Prev_Free = 0;
    // size of the next smallest free block
    int size_Of_Next_Free = 0;
    // the right most bit of block_hdr
    int right_Most_Bit = 0;
    // size of Block
    int size_Of_Block = 0;
    // next block's right most bit
    int next_Right_Most_Bit = 0;

    // sets ptr to point to first block in heap
    ptr = first_blk;

    // while the ptr's size status isn't equal to 
    // one, which means end of heap
    while (ptr->size_status != 1) {
        // sets size of block to be the ptr's size status
        size_Of_Block = ptr->size_status;

        // calculates right most bit
        right_Most_Bit = ptr->size_status % 2;

        // if the right most bit is 1
        if (right_Most_Bit == 1) {
            // subtract one from size of block
            size_Of_Block--;
	}

        // if the size of block isn't multiple of 8
        if (size_Of_Block % 8 != 0) {
            // subtract two from size of block
            size_Of_Block -= 2;
	}

        // the right most bit is 0, therefore isn't allocated
        if (right_Most_Bit == 0) {
            // if the size of block is bigger than size of allocation
            if (size_Of_Block >= size_Of_Allocation) {
                // the size of block is the size of allocation
                if (size_Of_Block == size_Of_Allocation) {
                    // increases pointer's size status by one
                    ptr->size_status++;
                    // set the nextPtr to point to the next header block
                    next_Ptr = ptr + size_Of_Block/4;
                    // checks if nextPtr is pointing to end of heap
                    if (next_Ptr->size_status == 1) {
                        // set is_End_Of_Heap to true
                        is_End_Of_Heap = true;
                        // break out of while loop
                        break;
                    }
                    // updates the size status of the next header_block
                    next_Ptr->size_status += 2;
                    // returns the ptr to the payload to the mem block
                    return (void*) (ptr + 1);
                // if block is big enough and smaller than previous min
                } else if (smallest_Free_Blk == NULL || 
			size_Of_Block < size_Of_Prev_Free) {
                    // sets ptr to the pointer pointing to the smallest block
                    smallest_Free_Blk = ptr;
                    // sets size of block to the size of the previous free one 
                    size_Of_Prev_Free = size_Of_Block;
		    // set ptr size status to the size status of the smallest block pointer
                    smallest_Free_Blk->size_status = ptr->size_status;
                    // sets the pointer of the smallest next block
                    next_Blk_After_Free = smallest_Free_Blk +
			 size_Of_Prev_Free/4;
                    // checks to see if the end of the heap is reached
                    if (next_Blk_After_Free->size_status == 1) {
			// set that the end of the heap is true
                        is_End_Of_Heap = true;
			// breaks out of loop
                        break;
                    }
                    // sets size status of blk after 
		    // the free one to size of next free
                    size_Of_Next_Free = next_Blk_After_Free->size_status;
                    // calculates the right most it
                    next_Right_Most_Bit = next_Blk_After_Free->size_status % 2;
                    // if the second from the right most bit is 1
                    if (next_Right_Most_Bit == 1) {
                        // subtract one from the size
                        size_Of_Next_Free--;
                    }
                    // if the size is a multiple of 8
                    if (size_Of_Next_Free % 8 != 0) {
                        // subtracts 2 from the size
                        size_Of_Next_Free -= 2;
                    }
                }
            }
        }
        // updates ptr to point to next block header
        ptr += size_Of_Block/4;
    }

    // if there is not a big enough block
    if (smallest_Free_Blk == NULL) {
        // returns NULL
        return NULL;
    // if there is a big enough block but it is bigger than what we want
    } else {
        // updates the size of the smallest Free block
        smallest_Free_Blk->size_status++;
        // size difference of the two splitted free blocks
        int size_Of_Split_Diff = 0;
        // pointer to the block being coalesced
        blk_hdr *coalesce_Blk = NULL;
        // stores the difference of the previous block size and size we want
        size_Of_Split_Diff = size_Of_Prev_Free - size_Of_Allocation;
        // updates size status of smallest free block
        smallest_Free_Blk->size_status -= size_Of_Split_Diff;
        // updtaes size of previous free block
        size_Of_Prev_Free -= size_Of_Split_Diff;

        // if it isn't end of heap and the right most bit is 0
        if (is_End_Of_Heap == false && next_Right_Most_Bit == 0) {
            // sets the footer pointer to point to the footer
            footer_Ptr = next_Blk_After_Free + size_Of_Next_Free/4 - 1;
            // updates the coalesce_Blk pointer
            coalesce_Blk = smallest_Free_Blk + size_Of_Prev_Free;
            // updates size status of the coalesce block
            coalesce_Blk->size_status = size_Of_Next_Free + size_Of_Split_Diff;
            // updates size status of footer pointer
            footer_Ptr->size_status = size_Of_Next_Free + size_Of_Split_Diff;
            // updates size status of coalesce block by 2
            coalesce_Blk->size_status += 2;
            // return pointer to the smallest free block's payload
            return (void *) (smallest_Free_Blk + 1);
        // if it isnt end of heap and the next block's right most bit is 1
        } else if (is_End_Of_Heap == false && next_Right_Most_Bit == 1) {
            // set the footer pointer to point to footer
            footer_Ptr = next_Blk_After_Free - 1;
            // update size status of footer
            footer_Ptr->size_status = size_Of_Split_Diff;
            // set the pointer to the coalesce block
            coalesce_Blk = smallest_Free_Blk + size_Of_Prev_Free/4;
            // update size status of coalesce block
            coalesce_Blk->size_status = size_Of_Split_Diff + 2;
            // return pointer to the smallest free block's payload
            return (void *) (smallest_Free_Blk + 1);
        // if it is the end of the heap
        } else {
            // set footer pointer of split block
            footer_Ptr = smallest_Free_Blk + ((size_Of_Prev_Free 
			+ size_Of_Split_Diff)/4 - 1);
            // updates size status of the footer
            footer_Ptr->size_status = size_Of_Split_Diff;
            // updates the pointer of the next block after free
            next_Blk_After_Free = smallest_Free_Blk + size_Of_Prev_Free/4;
            // set size staus of the next block after the free one
            next_Blk_After_Free->size_status = size_Of_Split_Diff + 2;
            // return pointer to the smallest free block
            return (void *) (smallest_Free_Blk + 1);
        }
    }
}

/* 
 * Function for freeing up a previously allocated block 
 * Argument - ptr: Address of the block to be freed up 
 * Returns 0 on success 
 * Returns -1 on failure 
 * Here is what this function should accomplish 
 * - Return -1 if ptr is NULL
 * - Return -1 if ptr is not 8 byte aligned or if the block is already freed
 * - Mark the block as free 
 * - Coalesce if one or both of the immediate neighbours are free 
 */
int Mem_Free(void *ptr) {
    // if the ptr is null
    if (ptr == NULL) {
        // return -1
        return -1;
    }
    // pointer that points to next block header
    blk_hdr *next_Blk_Hdr = NULL;
    // size of the next block
    int size_Of_Next_Blk = 0;
    // pointer that points to previous block header
    blk_hdr *prev_Blk_Hdr = NULL;
    // size of the block that pointer points to
    int size_Of_Blk = 0;
    // size of the previous block
    int size_Of_Prev_Blk = 0;
    // pointer that points to footer
    blk_hdr *footer_Ptr = NULL;
    // second right most bit
    int second_Right_Most_Bit = 0;
    // right most bit of next block
    int right_Most_Bit_Of_Next_Blk = 0;

    // sets pointer to point to the block header
    blk_hdr *pointer = ptr - 4;
    // if the block is already freed
    if (pointer->size_status % 2 == 0) {
        // return -1
        return -1;
    }

    // subtract one from pointer's size status
    pointer->size_status--;
    // if size isn't multiple of 8
    if (pointer->size_status % 8 != 0) {
        // that means second right most bit is 1
        second_Right_Most_Bit = 1;
    }

    // how much we must adjust the size of the block
    int i = (second_Right_Most_Bit * 2);
    // sets size of block to pointer's size status - i
    size_Of_Blk = pointer->size_status - i;
    // if the size of block isn't a mulitple of 8
    if (size_Of_Blk % 8 != 0) {
        // return -1
        return -1;
    }

    // makes the footer pointer to point to foot
    footer_Ptr = pointer + size_Of_Blk/4 - 1;
    // sets size status of footer pointer to be size of block
    footer_Ptr->size_status = size_Of_Blk;
    // makes next block header pointer to point to the next block header
    next_Blk_Hdr = pointer + size_Of_Blk/4;

    // if the size status isn't 1, which means not the end of heap
    if (next_Blk_Hdr->size_status != 1) {
	// updates sizes status of the block header for next block
	next_Blk_Hdr->size_status -= 2;
        // calculates the right most bit of next block
        right_Most_Bit_Of_Next_Blk = next_Blk_Hdr->size_status % 2;
        // if the next block is free
        if (right_Most_Bit_Of_Next_Blk != 1) {  
            // calculates size of this block
            size_Of_Next_Blk = next_Blk_Hdr->size_status;
            // adjusts to where the footer pointer points to
            footer_Ptr = next_Blk_Hdr + size_Of_Next_Blk/4 - 1;
            // calculates size status of the footer pointer
            footer_Ptr->size_status = size_Of_Next_Blk + size_Of_Blk;
            // adjusts the pointer's size status as well
            pointer->size_status = (second_Right_Most_Bit * 2) + 
		size_Of_Blk + size_Of_Next_Blk;
        }
    }
    // if the block in front of pointer's block is free
    if (second_Right_Most_Bit == 0) {
        // adjust to where prev block header pointer points to
        prev_Blk_Hdr = pointer - 1;
        // updates size of the previous block
        size_Of_Prev_Blk = prev_Blk_Hdr->size_status;
        // sets the pointer for the previous block header
        prev_Blk_Hdr = pointer - size_Of_Prev_Blk/4;
        // updates size status or the previous block header
        prev_Blk_Hdr->size_status += size_Of_Blk + size_Of_Next_Blk;
        // updates size status of footer pointer
        footer_Ptr->size_status += size_Of_Prev_Blk;
    }
    return 0;
}

/*
 * Function used to initialize the memory allocator
 * Not intended to be called more than once by a program
 * Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated
 * Returns 0 on success and -1 on failure 
 */
int Mem_Init(int sizeOfRegion) {                         
    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    blk_hdr* end_mark;
    static int allocated_once = 0;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: Mem_Init has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, 
                    fd, 0);
    if (MAP_FAILED == space_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
     allocated_once = 1;

    // for double word alignement and end mark
    alloc_size -= 8;

    // To begin with there is only one big free block
    // initialize heap so that first block meets 
    // double word alignement requirement
    first_blk = (blk_hdr*) space_ptr + 1;
    end_mark = (blk_hdr*)((void*)first_blk + alloc_size);
  
    // Setting up the header
    first_blk->size_status = alloc_size;

    // Marking the previous block as busy
    first_blk->size_status += 2;

    // Setting up the end mark and marking it as busy
    end_mark->size_status = 1;

    // Setting up the footer
    blk_hdr *footer = (blk_hdr*) ((char*)first_blk + alloc_size - 4);
    footer->size_status = alloc_size;
  
    return 0;
}

/* 
 * Function to be used for debugging 
 * Prints out a list of all the blocks along with the following information i
 * for each block 
 * No.      : serial number of the block 
 * Status   : free/busy 
 * Prev     : status of previous block free/busy
 * t_Begin  : address of the first byte in the block (this is where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block (as stored in the block header) (including the header/footer)
 */ 
void Mem_Dump() {                        
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    blk_hdr *current = first_blk;
    counter = 1;

    int busy_size = 0;
    int free_size = 0;
    int is_busy = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => busy block
            strcpy(status, "Busy");
            is_busy = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_busy = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "Busy");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_busy) 
            busy_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blk_hdr*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total busy size = %d\n", busy_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", busy_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;
}
