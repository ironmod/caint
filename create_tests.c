#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

int main(void)
{
    uint32_t num_cases = 10000;
    FILE *testfp = fopen("test10k.txt", "w");

    fprintf(testfp, "%u\n", num_cases);

    for(int i=0; i<num_cases; i++)
    {
        fprintf(testfp, "%u\n", i);
    }
    fclose(testfp);
}