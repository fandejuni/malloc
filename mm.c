/*    
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "SuperCoet",
    /* First member's full name */
    "Thibault Dardinier",
    /* First member's email address */
    "thibault.dardinier@polytechnique.edu",
    /* Second member's full name (leave blank if none) */
    "Paul Naert",
    /* Second member's email address (leave blank if none) */
    "paul.naert@polytechnique.edu"
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define SIZE_CELL 4

#define INT_POINTER_SIZE(pointeur) (((int *) pointeur) - 1)

#define EVENIZE(x) ((x >> 1) << 1)

#define ACTUAL_SIZE(pointeur) (EVENIZE(*(INT_POINTER_SIZE(pointeur))))

#define INT_POINTER_SIZE_LAST(pointeur) (((int *) pointeur) + (ACTUAL_SIZE(pointeur) - 2))

#define NEXT_BLOCK(pointeur) (void *) ((char *) pointeur + (ACTUAL_SIZE(pointeur) * SIZE_CELL)

#define PREV_BLOCK(pointeur) (void *) ((char *) pointeur - (EVENIZE(*(INT_POINTER_SIZE(pointeur) - 1)) * SIZE_CELL))

#define IS_FREE(pointeur) (*(INT_POINTER_SIZE(pointeur)) & 1)

#define OCCUPIED(value) ((value >> 1) << 1)

#define FREE(value) (OCCUPIED(value) + 1)

#define ADD(pointeur, n) ((void *) (((char *) pointeur) + n))

void* current_block;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    int* p = mem_sbrk(12);
    p++;
    *p = 2;
    p++;
    *p = 2;
    return 0;
}

void set_size(void* pointeur, int n) {
    int* p = INT_POINTER_SIZE(pointeur);
    *p = n;
    p = INT_POINTER_SIZE_LAST(pointeur);
    *p = n;
}

void set_free(void* pointeur) {
    int* p = INT_POINTER_SIZE(pointeur);
    *p = FREE(*p);
}

void set_occupied(void* pointeur) {
    int* p = INT_POINTER_SIZE(pointeur);
    *p = OCCUPIED(*p);
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *increase_heap_size(size_t size){
	int *heap_end=	((int*) mem_heap_hi()) +1;
	void *p = mem_sbrk(size);
	int end_size=0;
	if (*heap_end & 1){ //last block is free
		end_size = *heap_end;
		heap_end -= *heap_end +1;	//
		set_size(heap_end, end_size+size); //modifies the size of block at pos -1 and original_size + new size
	}
	return p;
}

void *mm_malloc(size_t size)
{		
		int prev_size=0;
		void* block_0 = current_block;
    int newsize = ALIGN(size + SIZE_T_SIZE) + 8; // 8: internal fragmentation to keep track of size
		while((char*) current_block + newsize < mem_heap_hi() ){ // loop from current to end
			if (IS_FREE(current_block) && ACTUAL_SIZE(current_block)> newsize){ // check if block fits
				set_occupied(current_block);
				prev_size= ACTUAL_SIZE(current_block);
				set_size(current_block,newsize);													// actualize size of block
				set_size(NEXT_BLOCK(current_block),prev_size - newsize); // actualize size of next block
				return current_block;
			}
		current_block = (void*) ( ( (char*) current_block) + newsize);
		}
		
		current_block=mem_heap_lo();
		while((char*) current_block + newsize < block_0 ){ // loop from begin to block_0
			if (IS_FREE(current_block) && ACTUAL_SIZE(current_block)> newsize){ // check if block fits
				set_occupied(current_block);
				prev_size= ACTUAL_SIZE(current_block);
				set_size(current_block,newsize);													// actualize size of block
				set_size(NEXT_BLOCK(current_block),prev_size - new_size); // actualize size of next block
				return current_block;
			}
		current_block = (void*) ( ( (char*) current_block) + newsize);
		}
		
		// Extend heap, and check if last portion is used or not
		
		current_block = mem_heap_hi();
		if(IS_FREE(current_block)){
			ADD(current_block, 3- (*current_block)); //go to start of last block
			increase_heap_size ( newsize - ACTUAL_SIZE(current_block) );
		}
		else{
			ADD(current_block,2);			
			increase_heap_size (newsize);
		}
		set_occupied(current_block);		
		return current_block;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}
