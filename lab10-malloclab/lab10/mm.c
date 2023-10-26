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

/* Design assumption: 
 * - Segregated list allocator 
 * - Explicit lists. pred and succ pointers in each free block
 * - Header in all blocks, footer in free blocks
 * - Minimal block size = 4(header) + 8*2(ptr) + 4(footer) = 24 Bytes
 *   Thus max payload in a minimal block = 24 - 4 = 20 Bytes 
 * - So size class 0 can be defined as [1, 24] 
 *   size class 1 [24, 32] ...
 *   size class 8 [2049, 4096] (Linux page size 4K)
 *   size class 9 [4097, inf]
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)
#define MIN_BLOCK 24

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

/* utilities for encapsulating pointers in textbook */
#define PACK(size, alloc) ((size) | (alloc))

/* RW a WORD at address p */
#define GET(p)          (*(unsigned int *)(p))
#define PUT(p, val)     (*(unsigned int *)(p) = (val))
/* RW a double-word at address p */
#define GET_DW(p)       (*(unsigned long *)(p))
#define PUT_DW(p, val)  (*(unsigned long *)(p) = (unsigned long)(val))

/* RW the size and alloc bit from address p */
#define GET_SIZE(p)     (GET(p) & ~0x7)
#define GET_ALLOC(p)    (GET(p) & 0x1)
#define SET_ALLOC(p, alloc)         (PUT(p, (GET(p) & ~0x1) | (alloc)))
#define GET_PREV_ALLOC(p)           (GET(p) & 0x2)
#define SET_PREV_ALLOC(p, alloc)    (PUT(p, (GET(p) & ~0x2) | (alloc << 1)))

/* Compute the header and footer address of a block ptr */
#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
/* RW the pred and succ pointers of a free block*/
#define GET_SUCC(bp)        (GET_DW((ptr_t *)bp))
#define GET_PRED(bp)        (GET_DW((ptr_t *)bp + 1))
#define SET_SUCC(bp, val)   (PUT_DW((ptr_t *)bp, val))
#define SET_PRED(bp, val)   (PUT_DW((ptr_t *)bp + 1, val))

/* Compute the address of next and previous blocks */
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

static int mm_check();

static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void seglist_insert(void *bp);
static void seglist_remove(void *bp);
static void *seglist_find(size_t size);
static int get_size_class(size_t size);
static void heap_info();

typedef unsigned int word_t;
typedef unsigned long dwrd_t;
typedef unsigned long ptr_t;

/* Store beginning address of each size class */
static ptr_t *size_class_head;
static int size_class_number = 10;
static int size_class_size;

/* heap head pointer */
static char *heap_listp;

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Allocate space for size_class_headers */
    
    size_class_size = ALIGN(size_class_number * sizeof(ptr_t));
    if((size_class_head = (ptr_t *)mem_sbrk(size_class_size)) == (void *)-1)
        return -1;

    /* Initialize size classes */
    for (int i = 0; i < size_class_number; i++)
        PUT_DW(size_class_head + i, NULL);

    
    /* Create initial empty heap */
    if((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;

    PUT(heap_listp, 0);                         /* Padding for alignment */
    PUT(heap_listp + WSIZE, PACK(8, 1));        /* Prologue header */
    PUT(heap_listp + 2 * WSIZE, PACK(8, 1));    /* Prologue footer */
    PUT(heap_listp + 3 * WSIZE, PACK(0, 1));    /* Epilogue header */
    SET_PREV_ALLOC(heap_listp + 3 * WSIZE, 1);  /* Prologue before epilogue is allocated */

    heap_listp += 2 * WSIZE;                    /* point to payload of prologue */

    /* Create initial block*/
    if(extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;
    // heap_info();
    // printf("Heap initialized\n\n");

    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    /* Calculate new total size of required block. align(max(payload+header, minBlockSize))*/
    int newsize = ALIGN(MAX(MIN_BLOCK, size + WSIZE));
    // printf("\n>>> Required %d bytes, aligned to %d bytes\n", size, newsize);

    void *bp = seglist_find(newsize);
    /* Request more space */
    if (!bp)
    {
        // printf("No valid free blocks. Extengding heap\n");
        /* Align_to_4K(newsize) per request */
        int chunk = (newsize - 1) / CHUNKSIZE + 1;
        if ((bp = extend_heap(chunk * CHUNKSIZE / WSIZE)) == NULL)
            return NULL;

        // if ((bp = extend_heap(newsize / WSIZE)) == NULL)
        //     return NULL;
    }
    // printf("Found available block of %d bytes, size class %d\n", GET_SIZE(HDRP(bp)), get_size_class(GET_SIZE(HDRP(bp))));

    if(GET_SIZE(HDRP(bp)) - newsize < MIN_BLOCK) {
        /* No split */
        // printf("Allocate the whole block\n");
        seglist_remove(bp);
        SET_ALLOC(HDRP(bp), 1);
        SET_PREV_ALLOC(HDRP(NEXT_BLKP(bp)), 1);
    }
    else {
        /* split */
        // printf("Splitting block 0x%x of %d bytes into %d + %d bytes\n", bp, GET_SIZE(HDRP(bp)), newsize, GET_SIZE(HDRP(bp)) - newsize);
        seglist_remove(bp);
        size_t totsize = GET_SIZE(HDRP(bp));
        PUT(HDRP(bp), PACK(newsize, GET_PREV_ALLOC(HDRP(bp))));
        SET_ALLOC(HDRP(bp), 1);
        /* the latter split part */
        PUT(HDRP(NEXT_BLKP(bp)), PACK(totsize - newsize, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(totsize - newsize, 0));
        SET_PREV_ALLOC(HDRP(NEXT_BLKP(bp)), 1);
        seglist_insert(NEXT_BLKP(bp));
    }

    // heap_info();
    return bp;
}

/*
 * mm_free - Freeing a block
 */
void mm_free(void *ptr)
{
    // printf("\n\n<<< Freeing block 0x%x of size %d bytes\n", ptr, GET_SIZE(HDRP(ptr)));
    SET_ALLOC(HDRP(ptr), 0);
    SET_PREV_ALLOC(HDRP(NEXT_BLKP(ptr)), 0);
    PUT(FTRP(ptr), PACK(GET_SIZE(HDRP(ptr)), 0));
    /* NOT IN seglist yet */
    SET_PRED(ptr, NULL);
    SET_SUCC(ptr, NULL);

    coalesce(ptr);
    // heap_info();
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

static int mm_check()
{
    printf("-------- Checking heap consistency \n");
    return 0;
}

/* Calculate corresponding size class */
static int get_size_class(size_t size)
{
    if(size <= MIN_BLOCK)
        return 0;
    size = (size - 1) >> 5;
    int k = 1;
    while(size && k < size_class_number) {
        size >>= 1;
        k++;
    }
    return MIN(k, size_class_number - 1);
}

static void *extend_heap(size_t words)
{
    // printf("Extending heap by %d bytes\n", words * WSIZE);

    char *bp;
    size_t size;
    /* round up words to even */
    size = (words & 1) ? (words + 1) * WSIZE : words * WSIZE;
    if((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    PUT(HDRP(bp), PACK(size, 0 | GET_PREV_ALLOC(HDRP(bp)))); /* Inherit PREV_ALLOC bit of epilogue */
    PUT(FTRP(bp), PACK(size, 0));
    SET_PRED(bp, NULL);
    SET_SUCC(bp, NULL);
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // New epilogue header

    return coalesce(bp);
}


/// @brief Coalesce adjacent free blocks around bp
/// @param bp block to be freed
/// @return new blockpointer after coalescing
static void *coalesce(void *bp)
{
    
    int prev_alloc = GET_PREV_ALLOC(HDRP(bp));
    int next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    void *prev_bp = PREV_BLKP(bp);
    void *next_bp = NEXT_BLKP(bp);
    size_t size = GET_SIZE(HDRP(bp));

    int flag = prev_alloc | next_alloc;

    // printf("Coalescing block 0x%x of %d bytes, prev %d, next %d\n", bp, GET_SIZE(HDRP(bp)), prev_alloc >> 1, next_alloc);

    switch (flag)
    {
    /* Case 11: orphan */
    case 3:
        seglist_insert(bp);
        break;

    /* Case 10: next block is free */
    case 2:
        seglist_remove(next_bp);
        size += GET_SIZE(HDRP(next_bp));
        PUT(HDRP(bp), PACK(size, 2));
        PUT(FTRP(bp), PACK(size, 0));
        seglist_insert(bp);
        break;

    /* Case 01: previous block is free */
    case 1:
        seglist_remove(prev_bp);
        size += GET_SIZE(HDRP(prev_bp));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(prev_bp), PACK(size, GET_PREV_ALLOC(HDRP(prev_bp))));
        seglist_insert(prev_bp);
        bp = prev_bp;
        break;

    /* Case 00: both are free */
    case 0:
        seglist_remove(prev_bp);
        seglist_remove(next_bp);
        size += GET_SIZE(HDRP(prev_bp)) + GET_SIZE(HDRP(next_bp));
        PUT(FTRP(next_bp), PACK(size, 0));
        PUT(HDRP(prev_bp), PACK(size, GET_PREV_ALLOC(HDRP(prev_bp))));
        seglist_insert(prev_bp);
        bp = prev_bp;
        break;
    }
    return bp;
}

/// @brief split a block in seglist into [size1, rest] two parts
/// @param bp free block to be split
/// @param size1 first sub block size
/// @return block pointer of first sub block
static void *split(void *bp, size_t size1)
{
    
    return bp;
}

/* Explicit segregated free list */

/* 
 * Insert new free block into segregated lists by LIFO. 
 * Only operates on PRED SUCC pointers 
 */
static void seglist_insert(void *bp)
{
    int k = get_size_class(GET_SIZE(HDRP(bp)));
    // printf("Inserting block 0x%x of %d byte into class %d\n", bp, GET_SIZE(HDRP(bp)), k);

    void *topbp = GET_DW(size_class_head + k);      /* Get first free block */
    /* Empty size class */
    if (!topbp)
    {
        PUT_DW(size_class_head + k, bp);
        SET_PRED(bp, NULL);
        SET_SUCC(bp, NULL);
    }
    else    
    {
        PUT_DW(size_class_head + k, bp);                /* Insert before first block */
        SET_PRED(topbp, bp);                            
        SET_SUCC(bp, topbp);
        SET_PRED(bp, NULL);
    }
}

/*
 * Remove the free block bp.
 * Only operates on PRED SUCC pointers
 */
static void seglist_remove(void *bp)
{
    void *prevbp = GET_PRED(bp);
    void *succbp = GET_SUCC(bp);
    int flag = ((prevbp != NULL) << 1) | (succbp != NULL);
    int k = get_size_class(GET_SIZE(HDRP(bp)));
    // printf("Removing block 0x%x of %d bytes, size class %d\n", bp, GET_SIZE(HDRP(bp)), k);
    // printf("    block in seglist pred=0x%x, succ=0x%x\n", prevbp, succbp);
    switch (flag)
    {
    /* Case 00: orphan */
    case 0:
        PUT_DW(size_class_head + k, NULL);
        break;
    /* Case 01: start of list */
    case 1:
        SET_PRED(succbp, NULL);
        PUT_DW(size_class_head + k, succbp);
        break;
    /* Case 10: end of list */
    case 2:
        SET_SUCC(prevbp, NULL);
        break;
    /* Case 11: amid */
    case 3:
        SET_SUCC(prevbp, succbp);
        SET_PRED(succbp, prevbp);
        break;
    }
}

/// @brief find the first fit block
/// @param size  required size
/// @return bp 
static void *seglist_find(size_t size)
{
    int size_class = get_size_class(size);

    // printf("Seeking for free block of %d bytes. Size class starts from %d\n", size, size_class);

    while(size_class < size_class_number)
    {
        /* walk through the size class */
        void *iter = GET_DW(size_class_head + size_class);
        while(iter)
        {
            if(GET_SIZE(HDRP(iter)) >= size)
                return iter;
            iter = GET_SUCC(iter);
        }
        // printf("Not found in size class %d\n", size_class);
        size_class++;
    }
    return NULL;
}

static void walk_size_class(int k)
{
    void *bp = GET_DW(size_class_head + k);
    // printf("0x%x -> ", bp);
    while(bp && GET_SUCC(bp))
    {
        // printf("0x%x -> ", GET_SUCC(bp));
        bp = GET_SUCC(bp);
    }
    // printf("NULL\n");
}

static void heap_info()
{
    // printf("\n========= Heap info ===========\n");
    char *start = mem_heap_lo();
    char *end = mem_heap_hi();
    char *p = start;                            /* increment of p is 1B per incr */
    size_t heap_size = end - start + 1;
    // printf("heap starts at 0x%x\n", start);
    // printf("heap size %luB, %luKB, %luMB\n", heap_size, heap_size / (1<<10), heap_size / (1<<20));

    for (int offset = 0; offset < size_class_number * DSIZE; offset += DSIZE)
    {
        // printf("size_class_head[%d]: 0x%016x\n", offset / DSIZE, *(dwrd_t *)(p + offset));
        walk_size_class(offset / DSIZE);
    }

    // printf("\n========== End Of INFO ===========\n\n");
}