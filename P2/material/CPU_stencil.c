#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int N = 1024;
#define RADIUS 3
int BLOCK_SIZE = 16;


void stencil_1d(int *in, int *out)
{
    
    for (int i = RADIUS; i < N - RADIUS; ++i)
    {
        int result = 0;
        for (int offset = -RADIUS; offset <= RADIUS; ++offset)
        {
            result += in[i + offset];
        }
        out[i] = result;
    }
}

void fill_ints(int *x, int n)
{
    for (int i = 0; i < n; ++i)
    {
        x[i] = 1;
    }
}

int function(int nsize)
{
    int *in, *out; // host copies of input and output arrays
    int size = nsize * sizeof(int);
    struct timeval ini, fin;

    // Allocate space for input and output arrays
    in = (int *)malloc(size);
    fill_ints(in, nsize);
    out = (int *)malloc(size);

    gettimeofday(&ini, NULL);

    // Apply the stencil
    for (int i = 0; i <= nsize / BLOCK_SIZE; i++) // lanza estos bloques
        for (int j = 0; j < BLOCK_SIZE; j++)  // lanza los threads
            stencil_1d(in, out);

    gettimeofday(&fin, NULL);

    printf("%d %f\n", nsize, ((fin.tv_sec * 1000000 + fin.tv_usec) - (ini.tv_sec * 1000000 + ini.tv_usec)) * 1.0 / 1000000.0);

    // Cleanup
    free(in);
    free(out);

    return 0;
}

int main(int argc, char **argv)
{
    int i = 100000;

    for(i=100000;i<=1000000;i+=100000)
        function(i);
    return 0;
}