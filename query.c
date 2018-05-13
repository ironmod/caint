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

int qsort_compare (const void * elem1, const void * elem2); 
uint32_t binary_search(uint32_t sorted_list[], int low, int high, uint32_t element);

#define FILE_SIZE_MAX_BYTES 35000

char segFileName[20] = "seg_";
char smallFileName[260];
char line[FILE_SIZE_MAX_BYTES];
int accum = 0;

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
    int input_file_size = ftell(infile);/// sizeof(uint32_t);
    fseek(infile, 0L, SEEK_SET);

    //
    // Determine the number of segments the binary file needs to be broken down into (due to RAM restrictions)
    //
    int file_segments = 0;
    //if(input_file_size > FILE_SIZE_MAX_BYTES)
    //{
        file_segments =  (input_file_size/FILE_SIZE_MAX_BYTES); //figure out how many segments this needs to be broken into, +1 for the eof
        //input_file_size = FILE_SIZE_MAX_BYTES/4;
    //}
    //else
    if(file_segments ==0)
    {
        file_segments = 1;
        input_file_size = input_file_size/sizeof(uint32_t);
    }
    else{
        file_segments =  (input_file_size/FILE_SIZE_MAX_BYTES); //figure out how many segments this needs to be broken into, +1 for the eof
        input_file_size = FILE_SIZE_MAX_BYTES/sizeof(uint32_t);
    }
    fprintf(qlog_fp, "Number of variables: %u\nNumber of segs: %u\n", input_file_size, file_segments);
    uint32_t user_in_buffer[FILE_SIZE_MAX_BYTES]; //buffer to be used for temp storing of the bins

    FILE *seg_temp;
    //
    // Loop through the entire input file and break the file up into
    //  smaller segments - if it is more than 35KB
    //
    for(int j=0; j < file_segments; j++)
    {
        sprintf(smallFileName, "%s%d.bin", segFileName, j);
        seg_temp = fopen(smallFileName, "wb");

        if (fread(user_in_buffer, sizeof(uint32_t), input_file_size, infile) != input_file_size) 
        {
            fputs("File read error: SegBuilder\n", stderr);
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

        //for (int k = 0; k < input_file_size; k++) 
        //{
            //printf("%u\n", user_in_buffer[k]);
            //fprintf(seg_temp, "%u", user_in_buffer[k]); //TODO: need to change this to fwrite for binary
        fwrite(&user_in_buffer, sizeof(uint32_t), input_file_size, seg_temp);
        //}
        fclose(seg_temp);
    }

    //
    // Get the user test cases and start looping through each 35k block at a time
    //
    uint32_t seg_buffer[FILE_SIZE_MAX_BYTES]; //buffer to be used for temp storing of the bins

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

    for (int i = 0; i < num_test_cases; i++) 
    {
        uint32_t query_val = 0;
        scanf("%u", &query_val);
        // Linear scan to find the closest value from inbuffer
        uint32_t closest_val = 0;
        uint32_t closest_dist = 0xFFFFFFFF;
        //
        // Loop through the entire input file and break the file up into
        //  smaller segments - if it is more than 35KB
        //
        //file_segments = 1;
        for(int j=0; j < file_segments; j++)
        {
            uint32_t closest_seg_val = 0;
            sprintf(smallFileName, "%s%d.bin", segFileName, j);
            seg_temp = fopen(smallFileName, "rb");
            //seg_temp = fopen("seg_0.bin", "rb");

            fseek(seg_temp, 0L, SEEK_END);
            input_file_size = ftell(seg_temp) / sizeof(uint32_t);
            fseek(seg_temp, 0L, SEEK_SET);
            //input_file_size = FILE_SIZE_MAX_BYTES/sizeof(uint32_t);

            if (fread(seg_buffer, sizeof(uint32_t), input_file_size, seg_temp) != input_file_size) 
            {
                fputs("File read error: SegReader\n", stderr);
                return 1;
            }

            closest_seg_val = binary_search(seg_buffer, 0, input_file_size, query_val);
            if(closest_seg_val < closest_dist)
            {
                closest_dist = closest_seg_val;
            }
            fclose(seg_temp);          
        }
        printf("%u\n", closest_dist);
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    fprintf(qlog_fp, "Run Time: %f", time_spent);
    return 0;
}


/****************************** Utilities *********************************************************/

uint32_t binary_search(uint32_t sorted_list[], int low, int high, uint32_t element)
{
    uint32_t middle=0;
    uint32_t closest=0;
    uint32_t closest_low=0;
    uint32_t closest_high=0;
    int last_val = 0;
    int max = high;
    uint32_t dist=0;
   /* for (int i=0; i<high; i++)
    {
        printf("%u\n", sorted_list[i]);
    }*/
    //printf("Searching for: %u\n", element);
    while (low <= high)
    {
        middle = low + (high - low)/2;
        //printf("Mid: %u\n", middle);
        if (element > sorted_list[middle])
        {
            last_val = middle;
            low = middle + 1;
            closest_low = sorted_list[middle];
            //printf("Low %d\n", middle);
        }
        else if (element < sorted_list[middle])
        {
            last_val = middle;
            high = middle - 1;
        }
        else
        {
            return middle;
        }
    }
    
    if(last_val == 0)
    {
        return sorted_list[last_val];
    }
    else if( last_val == max)
    {
        return sorted_list[max-1];
    }
    else
    {
        //determine which point is closer to the element
        // and return that location
        uint32_t dist1 = abs( sorted_list[last_val] - element);
        uint32_t dist2 = abs( sorted_list[last_val+1] - element);
        uint32_t dist3 = abs( sorted_list[last_val-1] - element);
        //printf("dist1: %u dist2: %u dist3: %u\n", dist1, dist2, dist3);

        if ( (dist1 > dist2) && (dist2 < dist3))
            return sorted_list[last_val+1];

        else if( (dist2 > dist1) && (dist1 < dist3))
            return sorted_list[last_val];

        else
            return sorted_list[last_val-1];
    }

    return 0;
};

int qsort_compare (const void * elem1, const void * elem2) 
{
    int first = *((int*)elem1);
    int second = *((int*)elem2);
    if (first > second) return  1;
    if (first < second) return -1;
    return 0;
}

int linear_search(void)
{
    // for (int k = 0; k < input_file_size; k++) 
    // {
    //     uint32_t candidate = seg_buffer[k];
    //     uint32_t dist = abs(candidate - query_val);
    //     //printf("%u\n",seg_buffer[k]);
    //     // Break ties by choosing the lower value
    //     if ((dist == closest_dist && candidate < closest_val) ||
    //         (dist < closest_dist)) 
    //     {
    //         closest_val = candidate;
    //         closest_dist = dist;
    //     }
    // }
}