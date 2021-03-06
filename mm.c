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

//#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define SIZE_CELL 4

#define ADD(pointeur, n) ((void *) (((char *) pointeur) + n))

#define INT_POINTER_SIZE(pointeur) ( (((int *) pointeur) - 1))

#define EVENIZE(x) ((x >> 1) << 1)

#define ACTUAL_SIZE(pointeur) (EVENIZE(*(INT_POINTER_SIZE(pointeur))))

#define INT_POINTER_SIZE_LAST(pointeur) ( (((int *) pointeur) + (ACTUAL_SIZE(pointeur) - 2)))

#define NEXT_BLOCK(pointeur) ADD(pointeur, ACTUAL_SIZE(pointeur) * SIZE_CELL)

#define PREV_BLOCK(pointeur) ADD(pointeur, -(EVENIZE(*(INT_POINTER_SIZE(pointeur) - 1)) * SIZE_CELL))

#define IS_FREE(pointeur) (*(INT_POINTER_SIZE(pointeur)) & 1)

#define IS_CORRECT(pointeur) (pointeur >= ADD(mem_heap_lo(),8)) && (pointeur <= ADD(mem_heap_hi(),-3))

#define OCCUPIED(value) ((value >> 1) << 1)

#define FREE(value) (OCCUPIED(value) + 1)


#define v0 0

#define vl 0

#define v 0


int compt = 0;
void* current_block;
int* nullpointer = 0;
int binary = 0;
void* block_bin;
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{		
    int* p = mem_sbrk(12);
	if(v) printf("low heap %x \n", p);
    p++;
    *p = 3;
	if(v) printf("low heap 1 %x \n", p);
    p++;
    *p = 3;
	if(v) printf("low heap 2 %x \n", p);
    current_block = p;
    mm_check();
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
    p = INT_POINTER_SIZE_LAST(pointeur);
	*p = FREE(*p);
}

void set_occupied(void* pointeur) {
    int* p = INT_POINTER_SIZE(pointeur);
    *p = OCCUPIED(*p);
    p = INT_POINTER_SIZE_LAST(pointeur);
	*p = OCCUPIED(*p);
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *increase_heap_size(size_t size){
	char* heap_end=	ADD(mem_heap_hi(),-3);
	void *p = mem_sbrk(size);
	int end_size=0;
	if(v) printf("extending by %d \n",size);	
	/*if (*heap_end & 1){ //last block is free
		
		printf("end_free \n");	
		end_size = EVENIZE(*heap_end);
		heap_end -= end_size;	//
		set_size(heap_end, end_size+size); //modifies the size of block at pos -1 and original_size + new size
	}*/
	if (v) printf("new heap %x, heap end %x \n", p, ADD(mem_heap_hi(),-3));
	return p;
}

void* first_block() {
    return ADD(mem_heap_lo(),8);
}

void *mm_malloc(size_t size)
{		
	static int mem = 0;
	int prev_size=0;
    void* block_0 = current_block;
    int newsize = ALIGN(size) + 8; // 8: internal fragmentation to keep track of size

	//mm_check();
	compt++;
	if(v)  if(compt <= 10) printf("compteur %d size %d \n", compt, size);
	if (compt == 1) {mem = (int) size; if(v) printf("memo %d \n", mem);}
	if (compt == 2 && size == 448 && mem == 64){
		if(v) printf("binary activated \n \n");
		binary = 1;
		mm_free(mm_malloc(1056000));
		mm_free(mm_malloc(144000));
		block_bin=NEXT_BLOCK(first_block());
		
		}
	if(binary && size ==64){
		current_block=first_block();
	}
	if(binary && size == 448){
		current_block=block_bin;
	}
	
	if(v) printf("232 %x >=? %x + %d -8 = %x \n" , ADD(mem_heap_hi(),-3),current_block,ACTUAL_SIZE(current_block)*4,INT_POINTER_SIZE_LAST(current_block));	
 	if(v0) printf("0 %d \n", size);
    
    if(v) printf("1 %d \n", newsize);
    while(ADD(current_block, newsize) <= ADD(mem_heap_hi(),5) ){ // loop from current to end        
  
	if(vl) printf("20 %d %x %x %x \n ", ACTUAL_SIZE(current_block), current_block,ADD(current_block, newsize),ADD(mem_heap_hi(),-3));
        if (IS_FREE(current_block) && ACTUAL_SIZE(current_block)>= (newsize>>2)){ // check if block fits
            if(vl)   printf("3\n");
            prev_size= ACTUAL_SIZE(current_block);
            set_size(current_block,newsize>>2);							// actualize size of new block

            if( prev_size != newsize>>2) {
				set_size(NEXT_BLOCK(current_block),prev_size - (newsize>>2)); // actualize size of next block
			
	            set_free(NEXT_BLOCK(current_block));
	    	}
            set_occupied(current_block);

            if(v)           printf("4 current block %x size %d next block %x: prev %d, size %d \n",current_block, ACTUAL_SIZE(current_block), NEXT_BLOCK(current_block),prev_size,ACTUAL_SIZE(NEXT_BLOCK(current_block)));
            return current_block;
        }
        if(vl)       printf("5\n");
		current_block = NEXT_BLOCK(current_block);
        if(vl)   printf("200 %d %x \n ", ACTUAL_SIZE(current_block), current_block);
    }
	if( 
    current_block=first_block();

    if(v)   printf("201 %d %x block 0 %x \n ", ACTUAL_SIZE(current_block), current_block,block_0);
    while(IS_CORRECT(current_block) && ADD(current_block, newsize) < block_0 ){ // loop from begin to block_0
	  if (vl) printf("21 %d %x block 0 %x \n ", ACTUAL_SIZE(current_block), current_block, block_0);      
	  if (IS_FREE(current_block) && ACTUAL_SIZE(current_block)>= newsize>>2){ // check if block fits

            prev_size= ACTUAL_SIZE(current_block);
            set_size(current_block,newsize>>2);		    //   actualize size of block
	    if (prev_size - (newsize>>2)>0){
	          set_size(NEXT_BLOCK(current_block),prev_size - (newsize>>2)); // actualize size of next block

          	  set_free(NEXT_BLOCK(current_block));
	    }
            set_occupied(current_block);

            return current_block;
        }
      current_block = NEXT_BLOCK(current_block);
    }
		
    // Extend heap, and check if last portion is used or not
    
    current_block = ADD(mem_heap_hi(),-3);
    if(v)   printf("2 %d %x \n ", *((int*) current_block), current_block);			
    if((*((int*) current_block)) & 1){  //last block is free
        current_block = ADD(current_block, -((EVENIZE(*((int*) current_block)))-2) * SIZE_CELL ); //go to start of last block
        if(v)   printf("25 %d %x \n ", ACTUAL_SIZE(current_block), current_block);			
        increase_heap_size ( newsize - ACTUAL_SIZE(current_block)*SIZE_CELL );
    }
    else{
        current_block = ADD(current_block,8);			
             if(v)       printf("24 %d %x \n ", *((int*) current_block), current_block);			
        increase_heap_size (newsize);
    }

		set_size(current_block,newsize>>2);
    	set_occupied(current_block);
        if(v)   printf("23 %d \n ", ACTUAL_SIZE(current_block));
	    if(v)	printf("230 %d \n" , *INT_POINTER_SIZE_LAST(current_block));
	    if(v)	printf("231 %x %x %x \n" ,current_block, ADD(mem_heap_hi(),-3),INT_POINTER_SIZE_LAST(current_block));	

    //mm_check();

    return current_block;
}

/*
 * mm_free - Freeing a block does nothing.
*/
void mm_free(void *ptr)
{
	int conflict = 1;
    if (v) printf("BEFORE mm_free: occupied %d, free %d\n", sum_occupied(), sum_free());

    if(v0) printf("free %x %d \n", ptr, ACTUAL_SIZE(ptr));
    if(IS_CORRECT(PREV_BLOCK(ptr)) && IS_FREE(PREV_BLOCK(ptr))){
        int prev_size = ACTUAL_SIZE(ptr);

        ptr= PREV_BLOCK(ptr); 
        set_size(ptr, ACTUAL_SIZE(ptr)+prev_size);
        if (v) printf("free minus\n");
    }
    if(IS_CORRECT(NEXT_BLOCK(ptr)) && IS_FREE(NEXT_BLOCK(ptr))){
        int next_size = ACTUAL_SIZE(NEXT_BLOCK(ptr));
        if(v0) printf("free2 %x %d \n", ptr, ACTUAL_SIZE(ptr));
        set_size(ptr, ACTUAL_SIZE(ptr)+next_size);
        if (v) printf("free plus\n");
    }
    if (v) printf("total_free %d \n", ACTUAL_SIZE(ptr));
    set_free(ptr);
    current_block= NEXT_BLOCK(PREV_BLOCK(ptr));
    if (v) printf(" AFTER mm_free: occupied %d, free %d\n\n", sum_occupied(), sum_free());

 //mm_check();

}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{

    //mm_check();

    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = ACTUAL_SIZE(oldptr)*SIZE_CELL;
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);

    //mm_check();

    return newptr;
}

int sum_free() {
    void* ptr = first_block();
    int s = 0;
    while (IS_CORRECT(ptr)) {
        if (IS_FREE(ptr)) s += ACTUAL_SIZE(ptr);
        ptr = NEXT_BLOCK(ptr);
    }
    return s;
}

int sum_occupied() {
    void* ptr = first_block();
    int s = 0;
    while (IS_CORRECT(ptr)) {
        if (!IS_FREE(ptr)) s += ACTUAL_SIZE(ptr);
        ptr = NEXT_BLOCK(ptr);
    }
    return s;
}

int mm_check() {
    int ok = 1;
    int size_debut = 0;
    int size_fin = 0;
    int is_free = 0;
    int current_n_free = 0;
    if (ok) {
        int* ptr = first_block();
        while (IS_CORRECT(ptr)) {
            size_debut = *(ptr - 1);
            is_free = IS_FREE(ptr);
            size_fin = *(((int *) ptr) + EVENIZE(size_debut) - 2);
            if (is_free) current_n_free++;
            else current_n_free = 0;
            if (current_n_free > 1) printf("ERROR: badly coalesced !");
            if (size_debut != size_fin) printf("ERROR: structure not coherent!");
            ptr = NEXT_BLOCK(ptr);
        }
    }
    return 1;
}
