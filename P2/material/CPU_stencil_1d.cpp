#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <cuda_runtime.h>

#define RADIOUS 3
int BLOCK_SIZE = 16;
int N = 16;

void stencil_1d(int *in, int *out, int N, int thread, int block) {
    
    int temp[BLOCK_SIZE +2 * RADIOUS];
    int gindex = thread + block * BLOCK_SIZE;
    int lindex = thread + RADIOUS;
    
    if(gindex > N) return;

    //Read input elements into shared memory
    temp[lindex] = in[gindex];
    if(thread < RADIOUS){
        temp[lindex - RADIOUS] = in[gindex - RADIOUS];
        temp[lindex + BLOCK_SIZE] = in[gindex + BLOCK_SIZE];
    }

    //Apply the stencil
    int result = 0;
    for (int offset = -RADIOUS; offset <= RADIOUS; offset++)
    {
        result += temp[lindex + offset];
    }
    //Store the result
    out[gindex] = result;

}

int main(int argc, char *argv[]){
    int *in = NULL, *out = NULL; // host copies of a, b, c
    int size;
    int *d_in = NULL, *d_out = NULL; // device copies of a, b, c

    if(argc<3){
        printf("El número de argumentos es incorrecto: ./nombre_ejecutable N BLOCK_SIZE\n");
        return 0;
    }

    N = atoi(argv[1]);
    BLOCK_SIZE = atoi(argv[2]);
    size = N * sizeof(int);

    //Alloc space for host copies of a, b, c and setup input values
    in = (int *)malloc(size);//random_ints(in,N);
    out = (int *)malloc(size);//random_ints(out,N);


}