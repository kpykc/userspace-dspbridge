/* 
 *  Copyright 2007
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */
/**
 *  @file       ti/sdo/fc/dskt2/permute.c
 *
 *  @brief      Utility functions + test for iterating through unique 
 *              permutations of a list of elements. 
 *
 *  @author     Murat Karaorman
 */
#ifdef _RTSMODE_
#include <stdio.h>
#else
#include <std.h>
#include <dbg.h>
#include <mem.h>
#include "_dskt2.h"
#endif

#include "dskt2_permute.h"

static void _rotateSubgroup(struct _DSKT2_PERMUTE_Enumeration *h, int pos0, 
                            int pos1, int rightOrLeft);
static void _rotateListElements(struct _DSKT2_PERMUTE_Enumeration *h);



_DSKT2_PERMUTE_EnumerationHandle _DSKT2_Permute_createHandle()
{
#ifdef _RTSMODE_
    _DSKT2_PERMUTE_EnumerationHandle h = (_DSKT2_PERMUTE_Enumeration *)
            malloc(sizeof(_DSKT2_PERMUTE_Enumeration));
#else
    _DSKT2_PERMUTE_EnumerationHandle h = (_DSKT2_PERMUTE_Enumeration *)
            MEM_calloc(_DSKT2_HEAP, sizeof(_DSKT2_PERMUTE_Enumeration),0);
#endif
    return h;
}

void _DSKT2_Permute_deleteHandle(_DSKT2_PERMUTE_EnumerationHandle h)
{
    if (NULL != h) {
#ifdef _RTSMODE_
        free(h);
#else
        MEM_free(_DSKT2_HEAP, h, sizeof(_DSKT2_PERMUTE_Enumeration));
#endif
    }
}

void _DSKT2_Permute_setInitialState(_DSKT2_PERMUTE_EnumerationHandle h, 
        void **initList, int numElements)
{
    int i=0;
    
    if (NULL == h) {
        return;
    }

    h->initialList = initList;
    h->done= 0;
    h->firstTime = 1;

    /* cap the size of list to module maximum */
    h->listSize = numElements<_DSKT2_PERMUTE_MAXSIZE ? numElements : _DSKT2_PERMUTE_MAXSIZE;

    for (i=0; i<numElements; i++) {
        h->state[i]=i+1;
        h->nextIteration[i] = &initList[i];
    }
}

void **_DSKT2_Permute_iterateNext(struct _DSKT2_PERMUTE_Enumeration *h)
{
    if (NULL == h) {
        return NULL;
    }

    if (h->firstTime) {
        h->firstTime = 0;
        return h->nextIteration;
    }
    
    if (h->listSize<=1 || h->done) {
        return NULL;
    }

    _rotateListElements(h);

    if (h->done) {
        return NULL;
    } else {
        return h->nextIteration;
    }
}

static void _rotateListElements(struct _DSKT2_PERMUTE_Enumeration *h)
{
    int i = 0;
    int numZeroes = 0;
    int direction;

    /*
     * Calculate number of elements in current iteration to rotate.
     * This is obtained by the number of inner-most indices counted down to 0.
     */
    for (i=0; i<h->listSize; i++) {

        if (0 == h->state[i]) {
            numZeroes++;
        } else {
            break;
        }
    }

    /*
     * Last enumeration is flagged when numZeroes reach list-size -1 
     */
    if (numZeroes == (h->listSize - 1) ) {
        h->done = 1;
        return;
    }

    for (i=0, direction = _DSKT2_PERMUTE_RIGHT; i<=numZeroes+1; i++) {
    
            _rotateSubgroup(h, 0, i, direction);
            direction *= (-1); /* rotate each subgroup in opposite direction */
        }

    /* reflect subgroup rotation by decrementing count in h->state */
    h->state[numZeroes]--;
    
    /* after each rotation of the subgroup, reset zero counts in h->state */
    for (i=0; i<numZeroes; i++) {
         h->state[i]=i+1;
    }
}

static void _rotateSubgroup(struct _DSKT2_PERMUTE_Enumeration *h, int pos0, 
            int pos1, int rightOrLeft)
{
    int direction;
    int startIndex; 
    void *save;
    int i;
    int count;

    if (rightOrLeft == _DSKT2_PERMUTE_RIGHT) {
        direction = 1;
        startIndex = pos0;
    } else {
        direction = -1;
        startIndex = pos1;
    }

    save = (void *)h->nextIteration[startIndex];

    for (i=startIndex, count=pos1-pos0; count>0; i=i+direction, count--) {
        h->nextIteration[i] = (void *)h->nextIteration[i+direction];
    }

    h->nextIteration[i] = save;
}

#ifdef _RTSMODE_
static int _testMain(int argc, char *argv[])
{
    // int list1[][1] = {{4}, {3}, {2}, {1}, {0}};
    int numElements; // = sizeof(list1)/sizeof(void *);
    int enumerationCount = 0;
    int **list = NULL;
    char **permList;
    _DSKT2_PERMUTE_EnumerationHandle pHandle = NULL;

    if (argc > 1) {
        permList = argv+1; 
    }
    else {
        printf("\nUsage: test <list> \n\n");
        return (0);
    }

    numElements = argc - 1;
    pHandle = _DSKT2_Permute_createHandle();

    _DSKT2_Permute_setInitialState(pHandle, (void **)permList, numElements);
 // Permute_setInitialState(pHandle, (void **)&list1, numElements);

    while (NULL != (list = (int **)_DSKT2_Permute_iterateNext(pHandle)) ) {

        enumerationCount++;
        {
            int i;
            printf ("%3d. ", enumerationCount);
            for (i=0; i<numElements; i++) {
                printf ("%s%s%s", (i==0)?"list = {" : "", 
                        list[i][0], (i==(numElements-1))? "}\n": ",");
            }
        }
    }
    _DSKT2_Permute_deleteHandle (pHandle);

    return (0);
}
#endif 
/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

