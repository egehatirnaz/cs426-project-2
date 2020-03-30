//
// Created by Ege Hatırnaz on 29.03.2020.
//
#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <float.h>

float* MPI_Map_Func(float* arr, int size, float (*func)(float)){
    int rank, comsize;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comsize);   // size is no. of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float *buff = malloc(sizeof(float) * size);
    MPI_Scatter(arr, size, MPI_FLOAT, buff,
                size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    float *locals = NULL;
    locals = malloc(sizeof(float) * size);
    float local = func(arr[rank]);
    MPI_Gather(&local, 1, MPI_FLOAT, locals, 1, MPI_FLOAT, 0,
               MPI_COMM_WORLD);

    for(int i = 0; i < size; i++){
        arr[i] = locals[i];
    }
    if (rank == 0) {
        return arr;
    }
    MPI_Finalize();
    return NULL;
}

// A simple method for testing.
float add(float a){
    return a + 1;
}

int main(int argc, char *argv[]){
    float* res;
    float arr[3] = {1.1f, 1.2f, 1.7f};
    float (*func)(float) = &add;
    int size = 3;

    res = MPI_Map_Func(arr, size, func);


    printf("%f\n", res[0]);
    printf("%f\n", res[1]);
    printf("%f\n", res[2]);
}

