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

char segFileName[20] = "seg_";
char smallFileName[260];
char line[FILE_SIZE_MAX_BYTES];
int accum = 0;
int qsort_compare (const void * elem1, const void * elem2); 
uint32_t seg_meta[110][2]; //min/max for each segment file

int main(int argc, char* argv[]) {
    clock_t begin = clock();
    FILE *qlog_fp = fopen("logfile.txt", "w"); //query log for cool stuffs


    //
    // User input file 
    //
    FILE* infile = fopen(argv[1], "rb");
    if (NULL == infile) {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    //
    // Determine input file size (number of 32-bit integers)
    //
    fseek(infile, 0L, SEEK_END);
    int input_file_size = ftell(infile) / sizeof(uint32_t);
    fseek(infile, 0L, SEEK_SET);

    //
    // Determine the number of segments the binary file needs to be broken down into (due to RAM restrictions)
    //
    int file_segments = 0;
    if(input_file_size > FILE_SIZE_MAX_BYTES)
    {
        file_segments =  (input_file_size/FILE_SIZE_MAX_BYTES); //figure out how many segments this needs to be broken into, +1 for the eof
        input_file_size = FILE_SIZE_MAX_BYTES;
    }
    else
        file_segments = 1;
    fprintf(qlog_fp, "Number of bytes: %u\nNumber of segs: %u\n", input_file_size, file_segments);
    uint32_t user_in_buffer[FILE_SIZE_MAX_BYTES]; //buffer to be used for temp storing of the bins

    //
    // Get the user test cases and start looping through each 35k block at a time
    //
    int num_test_cases = 0;
    scanf("%d", &num_test_cases);
    if(num_test_cases > 10000)
    {
        printf("You said no more than 10k test cases!!!");
        return 1;
    }
    else if(num_test_cases == 0)
    {
        printf("Why you no enter test case!");
        return 1;
    }

    FILE *seg_temp;
    //
    // Loop through the entire input file and break the file up into
    //  smaller segments - if it is more than 35KB
    //
    for(int j=0; j < file_segments; j++)
    {
        sprintf(smallFileName, "%s%d", segFileName, j);
        seg_temp = fopen(smallFileName, "wb");

        if (fread(user_in_buffer, sizeof(uint32_t), input_file_size, infile) != input_file_size) 
        {
            fputs("File read error\n", stderr);
            return 1;
        }

        for (int k = 0; k < input_file_size; k++) 
        {
            user_in_buffer[k] = htonl(user_in_buffer[k]);
        }

        //
        // Sort the buffer to speed up searching
        //
        qsort (user_in_buffer, input_file_size, sizeof(uint32_t), qsort_compare);

        seg_meta[j][0] = user_in_buffer[0]; //Minimum in the file
        seg_meta[j][1] = user_in_buffer[input_file_size-1]; //Maximum in the file

        fprintf(qlog_fp, "Segment %d Min: %u Max: %u\n", j,  seg_meta[j][0], seg_meta[j][1]);

        for (int k = 0; k < input_file_size; k++) 
        {
            //printf("%u\n", user_in_buffer[k]);
            fprintf(seg_temp, "%u\n", user_in_buffer[k]); //TODO: need to change this to fwrite for binary
        }
        fclose(seg_temp);
    }

    //
    // Get the user test cases and start looping through each 35k block at a time
    //
  /*  int num_test_cases = 0;
    scanf("%d", &num_test_cases);
    printf("num test cases: %d\n", num_test_cases);
    for (int i = 0; i < num_test_cases; i++) 
    {
        uint32_t query_val = 0;
        scanf("%u", &query_val);
        // Linear scan to find the closest value from inbuffer
        uint32_t closest_val = 0;
        uint32_t closest_dist = 0xFFFFFFFF;
        fseek(infile, 0L, SEEK_SET);//reset to begining of file

        //
        // Loop through the entire input file and break the file up into
        //  smaller segments - if it is more than 35KB
        //
        for(int j=0; j < file_segments; j++)
        {

            if (fread(user_in_buffer, sizeof(uint32_t), input_file_size, infile) != input_file_size) 
            {
                fputs("File read error\n", stderr);
                return 1;
            }

            //
            // Sort the buffer to speed up searching
            //
         //   qsort (user_in_buffer, input_file_size, sizeof(uint32_t), qsort_compare);

            for (int k = 0; k < input_file_size; k++) 
            {
                user_in_buffer[k] = htonl(user_in_buffer[k]);
                uint32_t candidate = user_in_buffer[k];
                uint32_t dist = abs(candidate - query_val);

                // Break ties by choosing the lower value
                if ((dist == closest_dist && candidate < closest_val) ||
                    (dist < closest_dist)) 
                {
                    closest_val = candidate;
                    closest_dist = dist;
                }
            }
        }
        printf("%u\n", closest_val);
    }
*/
    fclose(infile);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    fprintf(qlog_fp, "Run Time: %f", time_spent);
    return 0;
}


/****************************** Utilities *********************************************************/

int binary_search(int sorted_list[], int low, int high, int element)
{
    int middle;
    while (low <= high)
    {
        middle = low + (high - low)/2;
        if (element > sorted_list[middle])
            low = middle + 1;
        else if (element < sorted_list[middle])
            high = middle - 1;
        else
            return middle;
    }
    return -1;
}

int qsort_compare (const void * elem1, const void * elem2) 
{
    int first = *((int*)elem1);
    int second = *((int*)elem2);
    if (first > second) return  1;
    if (first < second) return -1;
    return 0;
}

void ram_seg_text_build(void)
{

}