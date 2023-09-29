#include <stdlib.h>
#include <stdio.h>
#include <x86intrin.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <sys/time.h>

#define ARRAY_SIZE 2048
#define NUMBER_OF_TRIALS 1000000

/*
 * Statically allocate our arrays.  Compilers can
 * align them correctly.
 */
static double a[ARRAY_SIZE], b[ARRAY_SIZE], c;

int main(int argc, char *argv[])
{
    int i, j , t;
    time_t t1, t2;
    double m = 1.0001;
    struct timeval start_time, end_time;
    float seconds=0;

    int diff = ARRAY_SIZE % 4;
    /* Populate A and B arrays */
    for (i = 0; i < (ARRAY_SIZE-diff); i+=4)
    {
        __m256d vb = {i, i+1, i+2, i+3};
        __m256d va = {i+1, i+2, i+3, i+4};
        _mm256_store_pd(&a[i], va);
        _mm256_store_pd(&b[i], vb);
    }
    //Array %4 !=0
    for (i = (ARRAY_SIZE-diff); i < ARRAY_SIZE; i+=4)
    {
        b[i] = i;
        a[i] = i + 1;
    }
    __m256d mm = {1.0001, 1.0001, 1.0001, 1.0001};
   
    
    /* Perform an operation a number of times */

    FILE *pf = fopen("tiempos_intrinseco.txt", "w");
    if (!pf)
    {
        if (pf)
            fclose(pf);
        return -1;
    }

    for (t = 100000; t < NUMBER_OF_TRIALS + 1; t+=100000)
    {
        gettimeofday(&start_time, NULL); // Obtiene el tiempo de inicio
        for (i = 0; i < (ARRAY_SIZE - diff); i += 4)
        {
            __m256d sum = {0.0, 0.0, 0.0, 0.0};
            // Load arrays
            __m256d va = _mm256_load_pd(&a[i]);
            __m256d vb = _mm256_load_pd(&b[i]);
            // Compute m*a+b
            __m256d tmp = _mm256_fmadd_pd(mm, va, vb);
            // Accumulate results
            sum = _mm256_add_pd(tmp, sum);
            for (j = 0; j < 4; j++)
            {
                c += sum[j];
            }
            
        }

        for (i = (ARRAY_SIZE - diff); i < ARRAY_SIZE; i++)
        {
            c += m * a[i] + b[i];
        }
        gettimeofday(&end_time, NULL); // Obtiene el tiempo de finalización
        seconds = ((end_time.tv_sec * 1000000 + end_time.tv_usec) - (start_time.tv_sec * 1000000 + start_time.tv_usec)) * 1.0 / 1000000.0;
        fprintf(pf, "%f %d\n", seconds, t);
    }

    fclose(pf);
    printf("c = %f\n", c);

    return 0;
}
