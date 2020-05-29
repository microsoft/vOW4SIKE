/********************************************************************************************
* SIDH: an efficient supersingular isogeny cryptography library
*
* Abstract: benchmarking/testing functions for van Oorschot-Wiener attack against generic hash 
*********************************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include "test_extras.h"

typedef struct {
    unsigned int max_crumbs;
	unsigned int* positions;
	unsigned int* max_distance;
	uint64_t* crumbs;
	unsigned int* index_crumbs;
} CrumbsSIDH, *PCrumbsSIDH;




int HanselGretel(int distinguished_position, PCrumbsSIDH StructCrumbs)
{ // Testing
    bool collision_found = false;
    unsigned int i, temp, swap_position = 1, scratch_position = 1, index_position = 1;
    unsigned int max_dist = 1, real_dist = 1;
    unsigned int position = 0;
    unsigned int num_crumbs = 0;
    unsigned int max_crumbs = StructCrumbs->max_crumbs;
    uint64_t cbk;

    while (!collision_found) {

        cbk = (uint64_t)rand();    // Compute some random cbk for testing

        if (num_crumbs < max_crumbs) {
            StructCrumbs->crumbs[position] = cbk;
            StructCrumbs->positions[position] = position;
            StructCrumbs->index_crumbs[position] = position;
            num_crumbs++;
        } else if (position - StructCrumbs->positions[max_crumbs-1] == max_dist) {
            temp = StructCrumbs->index_crumbs[index_position];
            for (i = index_position; i < max_crumbs-1; i++) {    // Updating table with crumb indices for the crump table
                StructCrumbs->index_crumbs[i] = StructCrumbs->index_crumbs[i+1];
            }
            StructCrumbs->index_crumbs[max_crumbs-1] = temp;
            index_position++;
            if (index_position > max_crumbs - 1) {
                index_position = 1;
            }
            StructCrumbs->crumbs[temp] = cbk;    // Inserting a new crumb at the end of the crumb table

            for (i = scratch_position; i < max_crumbs-1; i++) {    // Updating table with crumb positions 
                StructCrumbs->positions[i] = StructCrumbs->positions[i+1];
            }
            StructCrumbs->positions[max_crumbs-1] = position;
            swap_position += 2*real_dist;
            scratch_position++;
            if (swap_position > max_crumbs - 1) {    
                swap_position = 1;  
                real_dist <<= 1;          
            }
            if (scratch_position > max_crumbs - 1) {
                scratch_position = 1;
                max_dist <<= 1; 
                swap_position = max_dist;            
            }            
        }

        if (position == (unsigned int)distinguished_position)
            collision_found = true;        
        position++;
    }
    if ((rand() & 1) == 0) 
        distinguished_position = -distinguished_position;    

    return PASSED;
}


 int main()
 {
    int Status = PASSED;
    int distinguished_position = 100 + (rand() & ((1 << 10)-1));
    unsigned int max_crumbs = 9;
    PCrumbsSIDH StructCrumbs;

    // Initialize crumbs struct    
    StructCrumbs = (PCrumbsSIDH)calloc(1, sizeof(PCrumbsSIDH));
    StructCrumbs->max_crumbs = max_crumbs;                          // Fixed to this values for this test
    StructCrumbs->positions = (unsigned int*)calloc(1, max_crumbs*sizeof(unsigned int));
    StructCrumbs->max_distance = (unsigned int*)calloc(1, max_crumbs*sizeof(unsigned int));
    StructCrumbs->crumbs = (uint64_t*)calloc(1, max_crumbs*sizeof(uint64_t));
    StructCrumbs->index_crumbs = (unsigned int*)calloc(1, max_crumbs*sizeof(unsigned int));
    
    Status = HanselGretel(distinguished_position, StructCrumbs);    // Testing Hansel & Gretel technique
    if (Status != PASSED) {
        printf("\n\n   Error detected ... \n\n");
    }

	#ifdef _WIN64
    system("PAUSE");
	#endif
    return Status;
 }

