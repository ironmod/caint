/*
 * This is a naive (and very slow) solution to the problem. It is meant to be used as a starting
 * point for your final solution, and to save you time from writing boilerplate code (file I/O).
 *
 * This implementation does NOT adhere to the 35 KB memory constraint. The final solution
 * must adhere to all constraints specified in the README file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include "query_utils.h"

#define FILE_SIZE_MAX_BYTES 35000

int main(int argc, char* argv[]) {
    FILE* infile = fopen(argv[1], "rb");
    if (NULL == infile) {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    // Determine input file size (number of 32-bit integers)
    fseek(infile, 0L, SEEK_END);
    int input_file_size = ftell(infile) / sizeof(uint32_t);
    fseek(infile, 0L, SEEK_SET);

    int file_segments = 0;

    file_segments =  input_file_size/FILE_SIZE_MAX_BYTES;
    
    // Allocate buffer to hold entire input file
    //
    // TODO - This does not adhere to the memory constraint in the problem statement.
    // Need to fix. No more than 35 KB of memory can be used.
    uint32_t* inbuffer = (uint32_t*)malloc(sizeof(uint32_t) * FILE_SIZE_MAX_BYTES);
    printf("File Size: %d\n", FILE_SIZE_MAX_BYTES);
    if (inbuffer == NULL) {
        fputs("Malloc error", stderr);
        return 1;
    }

    // Read entire file into buffer
    if (fread(inbuffer, sizeof(uint32_t), FILE_SIZE_MAX_BYTES, infile) != FILE_SIZE_MAX_BYTES) {
        fputs("File read error", stderr);
        return 1;
    }

    fclose(infile);

    // in-place endian swap of all elements
    for (int i = 0; i < FILE_SIZE_MAX_BYTES; i++) {
        inbuffer[i] = htonl(inbuffer[i]);
    }
    // printf("-------------Before Sorting----------------\n");
    // for (int i = 0; i< FILE_SIZE_MAX_BYTES; i++)
    // {
    //     printf("%u\n", inbuffer[i]);
    // }

    // qsort (inbuffer, FILE_SIZE_MAX_BYTES, sizeof(uint32_t), qsort_compare);

    // printf("-------------After Sorting----------------\n");
    // for (int i = 0; i< FILE_SIZE_MAX_BYTES; i++)
    // {
    //     printf("%u\n", inbuffer[i]);
    // }

    int num_test_cases = 0;
    scanf("%d", &num_test_cases);

    for (int i = 0; i < num_test_cases; i++) {
        uint32_t query_val = 0;
        scanf("%u", &query_val);

        // Linear scan to find the closest value from inbuffer
        uint32_t closest_val = 0;
        uint32_t closest_dist = 0xFFFFFFFF;
        for (int j = 0; j < FILE_SIZE_MAX_BYTES; j++) {
            uint32_t candidate = inbuffer[j];
            uint32_t dist = abs(candidate - query_val);

            // Break ties by choosing the lower value
            if ((dist == closest_dist && candidate < closest_val) ||
                (dist < closest_dist)) {
                closest_val = candidate;
                closest_dist = dist;
            }
        }

       // printf("%u\n", closest_val);
    }

    return 0;
}
