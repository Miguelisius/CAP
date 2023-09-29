#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define ARRAY_SIZE 2048
#define NUMBER_OF_TRIALS 1000000

/*
 * Statically allocate our arrays.  Compilers can
 * align them correctly.
 */
static double a[ARRAY_SIZE], b[ARRAY_SIZE], c;

int main(int argc, char *argv[]) {
    int i,t;
    struct timeval start_time, end_time;
    float seconds = 0;
    double m = 1.0001;

    /* Populate A and B arrays */
    for (i=0; i < ARRAY_SIZE; i++) {
        b[i] = i;
        a[i] = i+1;
    }
    FILE *pf = fopen("tiempos_simple.txt", "w");
    if (!pf)
    {
        if (pf)
            fclose(pf);
        return -1;
    }
    /* Perform an operation a number of times */
    for (t=100000; t < NUMBER_OF_TRIALS + 1; t+=100000) {
        gettimeofday(&start_time, NULL); // Obtiene el tiempo de inicio
        for (i=0; i < ARRAY_SIZE; i++) {
            c += m*a[i] + b[i];
        }
        gettimeofday(&end_time, NULL); // Obtiene el tiempo de finalización
        seconds = ((end_time.tv_sec * 1000000 + end_time.tv_usec) - (start_time.tv_sec * 1000000 + start_time.tv_usec)) * 1.0 / 1000000.0;
        fprintf(pf, "%f %d\n", seconds, t);
    }
    fclose(pf);
    printf("c = %f\n", c);
    return 0;
}
