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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_SIZE_MAX_BYTES 35000

char segFileName[20] = "seg_";
char smallFileName[260];
char line[FILE_SIZE_MAX_BYTES];
int accum = 0;
int qsort_compare (const void * elem1, const void * elem2); 

int main(int argc, char* argv[]) {

    FILE* seg_temp;
    FILE* infile = fopen(argv[1], "rb");
    if (NULL == infile) {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    // Determine input file size (number of 32-bit integers)
    fseek(infile, 0L, SEEK_END);
    int input_file_size = ftell(infile) / sizeof(uint32_t);
    fseek(infile, 0L, SEEK_SET);

    uint32_t* inbuffer = (uint32_t*)malloc(sizeof(uint32_t) * input_file_size);
    if (inbuffer == NULL) {
        fputs("Malloc error", stderr);
        return 1;
    }
    mkdir("/media/minty/PRAECLUSIO/Code/cruise/seg_temp", 0700);
    //
    // Determine the number of segments the binary file needs to be broken down into (due to RAM restrictions
    //
    int file_segments = 0;

    file_segments =  (input_file_size/FILE_SIZE_MAX_BYTES) + 1; //figure out how many segments this needs to be broken into, +1 for the eof
    printf("Number of bytes: %u\nNumber of segs: %u\n", input_file_size, file_segments);

    //
    //  This is to handle input files that can fit within RAM
    //  
    // TODO: FIgure out a different way to handle this
    //
    if(file_segments == 1)
    {
        // Determine input file size (number of 32-bit integers)
        fseek(infile, 0L, SEEK_END);
        int input_file_size = ftell(infile) / sizeof(uint32_t);
        fseek(infile, 0L, SEEK_SET);
    }
    else{
        input_file_size = FILE_SIZE_MAX_BYTES;
    }

    //
    // Read through the binary, 35KB at a time, and store the results
    //  into segment files, if needed
    //
    for(int i=0; i < file_segments; i++)
    {
        sprintf(smallFileName, "%s%d.bin", segFileName, i);
        seg_temp = fopen(smallFileName, "wb");

        if (fread(inbuffer, sizeof(uint32_t), input_file_size, infile) != input_file_size) {
            fputs("File read error\n", stderr);
            return 1;
        }
        //
        // in-place endian swap of all elements
        // TODO: can this be combined into the lower loop?
        //
        for (int j = 0; j < input_file_size; j++) {
            inbuffer[j] = htonl(inbuffer[j]);
        }

        //
        // Sort the buffer, before storing it to speed up the search later
        //
        qsort (inbuffer, input_file_size, sizeof(uint32_t), qsort_compare);

        //
        // Finally store the buffer into it's seg file
        //
        for (int j = 0; j < input_file_size; j++)
        {
            fprintf(seg_temp, "%u\n", inbuffer[j]);
           // printf("%u\n", inbuffer[j]);
        }

        //
        // Close the file and move onto the next
        //
        fclose(seg_temp);
    }

    //
    // Done with the user input file 
    //
    fclose(infile);

    //
    // TODO: External merge sort the files to drastically speed up the searching
    //

    //
    // Now we need to start reading back and searching through the files we just created
    //
    for(int segfile_index = 0; segfile_index < file_segments; segfile_index++)
    {
        seg_temp = fopen("seg_0.bin", "r");
        if (NULL == seg_temp) {
            printf("Could not open segtemp readback file %s\n", argv[1]);
            return 1;
        }
        //
        // Determine input file size (number of 32-bit integers)
        // TODO: this should only be needed for incomplete 35KB files
        //
        fseek(seg_temp, 0L, SEEK_END);
        int seg_file_size = ftell(seg_temp) / sizeof(uint32_t);
        fseek(seg_temp, 0L, SEEK_SET);

        uint32_t* segbuffer = (uint32_t*)malloc(sizeof(uint32_t) * seg_file_size);
        if (segbuffer == NULL) {
            fputs("Malloc error", stderr);
            return 1;
        }

        // Read entire file into buffer
        if (fread(segbuffer, sizeof(uint32_t), seg_file_size, infile) != seg_file_size) {
            fputs("File read error", stderr);
            return 1;
        }
        // in-place endian swap of all elements
        for (int i = 0; i < seg_file_size; i++) {
            segbuffer[i] = htonl(segbuffer[i]);
        }
        for(int i=0; i < seg_file_size; i++)
        {
            printf("%u\n", segbuffer[i]);  
        }
    }

    int num_test_cases = 0;
    scanf("%d", &num_test_cases);
    for (int i = 0; i < num_test_cases; i++) {
        uint32_t query_val = 0;
        scanf("%u", &query_val);

        // Linear scan to find the closest value from inbuffer
        uint32_t closest_val = 0;
        uint32_t closest_dist = 0xFFFFFFFF;
        for (int j = 0; j < input_file_size; j++) {
            uint32_t candidate = inbuffer[j];
            uint32_t dist = abs(candidate - query_val);

            // Break ties by choosing the lower value
            if ((dist == closest_dist && candidate < closest_val) ||
                (dist < closest_dist)) {
                closest_val = candidate;
                closest_dist = dist;
            }
        }

        printf("%u\n", closest_val);
    }
/*
    //
    // Break the large binary into smaller little bitty binaries so that they can be RAM Loaded (<35KB)
    //
    for(int i=0; i<file_segments; i++)
    {
        sprintf(smallFileName, "%s%d.bin", segFileName, i);
        seg_temp = fopen(smallFileName, "wb");
        accum = 0;
        if(seg_temp)
        {
            while(fgets(line, FILE_SIZE_MAX_BYTES, infile) && accum <= FILE_SIZE_MAX_BYTES)
           {
                accum += strlen(line);
                fputs(line,seg_temp);
           }
        }
        fclose(seg_temp);
    }
*/

/*
    //
    // Grab each file at a time, and sort them
    //
    uint32_t* segbuffer = (uint32_t*)malloc(sizeof(uint32_t) * FILE_SIZE_MAX_BYTES);
    FILE* segfile = fopen("easy_input.bin", "rb");


    // Read entire file into buffer
    if (fread(segbuffer, sizeof(uint32_t), FILE_SIZE_MAX_BYTES, segfile) != FILE_SIZE_MAX_BYTES) {
        fputs("Seg File read error\n", stderr);
        return 1;
    }
    
    if(segfile)
    {
        fread(segbuffer, sizeof(uint32_t), FILE_SIZE_MAX_BYTES, segfile);
    }

    fclose(segfile);
    
    // in-place endian swap of all elements
    for (int i = 0; i < FILE_SIZE_MAX_BYTES; i++) {
        segbuffer[i] = htonl(segbuffer[i]);
    }

    printf("-------------Seg Before Sorting----------------\n");
    for (int i = 0; i< FILE_SIZE_MAX_BYTES; i++)
    {
        printf("%u\n", segbuffer[i]);
    }

    qsort (segbuffer, FILE_SIZE_MAX_BYTES, sizeof(uint32_t), qsort_compare);

    printf("-------------Seg After Sorting----------------\n");
    for (int i = 0; i< FILE_SIZE_MAX_BYTES; i++)
    {
        printf("%u\n", segbuffer[i]);
    }
*/   
    // Allocate buffer to hold entire input file
    //
    // TODO - This does not adhere to the memory constraint in the problem statement.
    // Need to fix. No more than 35 KB of memory can be used.


 /*   printf("-------------Before Sorting----------------\n");
    for (int i = 0; i< input_file_size; i++)
    {
        printf("%u\n", inbuffer[i]);
    }

    qsort (inbuffer, input_file_size, sizeof(uint32_t), qsort_compare);

    printf("-------------After Sorting----------------\n");
    for (int i = 0; i< input_file_size; i++)
    {
        printf("%u\n", inbuffer[i]);
    }

 /*   int num_test_cases = 0;
    scanf("%d", &num_test_cases);

    for (int i = 0; i < num_test_cases; i++) {
        uint32_t query_val = 0;
        scanf("%u", &query_val);

        // Linear scan to find the closest value from inbuffer
        uint32_t closest_val = 0;
        uint32_t closest_dist = 0xFFFFFFFF;
        for (int j = 0; j < input_file_size; j++) {
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
*/
    return 0;
}


/****************************** Utilities *********************************************************/

int qsort_compare (const void * elem1, const void * elem2) 
{
    int f = *((int*)elem1);
    int s = *((int*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}