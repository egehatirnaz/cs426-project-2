//
// Created by Ege Hatırnaz on 29.03.2020.
//
#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <float.h>

float *MPI_Map_Func(float *arr, int size, float (*func)(float)){
    int rank, comsize;

    MPI_Comm_size(MPI_COMM_WORLD, &comsize);   // size is no. of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float *buff = malloc(sizeof(float) * size + 1);
    MPI_Scatter(arr, size, MPI_FLOAT, buff,
                size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    float *locals = NULL;
    locals = malloc(sizeof(float) * size + 1);
    float local = func(arr[rank]);
    MPI_Allgather(&local, 1, MPI_FLOAT, locals, 1, MPI_FLOAT,
               MPI_COMM_WORLD);

    for(int i = 0; i < size; i++){
        arr[i] = locals[i];
    }
    return arr;
}

float* MPI_Filter_Func(float* arr, int size, bool (*pred)(float)){
    int rank, comsize;
    float *result = malloc(sizeof(float) * size + 1);

    MPI_Comm_size(MPI_COMM_WORLD, &comsize);   // size is no. of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float *buff = malloc(sizeof(float) * size + 1);
    MPI_Scatter(arr, size, MPI_FLOAT, buff,
                size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    float *locals = NULL;
    locals = malloc(sizeof(float) * size + 1);
    float local = pred(arr[rank]);
    MPI_Allgather(&local, 1, MPI_FLOAT, locals, 1, MPI_FLOAT, MPI_COMM_WORLD); // Maybe it's a bad idea.

    for(int i = 0; i < size; i++){
        result[i] = locals[i];
    }
    return result;
}

// A simple method for testing.
float add(float a){
    return a + 1;
}

bool is_positive(float a){
    if (a >= 0.0f) {
        return true;
    } else {
        return false;
    }
}

int main(int argc, char *argv[]){
    float *res2, *res1;
    float arr1[3] = {-1.0f, 1.2f, 1.7f};
    float arr2[3] = {-1.0f, 1.2f, 1.7f};
    float (*func_add)(float) = &add;
    bool (*func_pos)(float) = &is_positive;
    int size = 3;

    MPI_Init(NULL, NULL);
    int rank, comsize;
    MPI_Comm_size(MPI_COMM_WORLD, &comsize);   // size is no. of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    res1 = MPI_Filter_Func(arr1, size, func_pos);
    res2 = MPI_Map_Func(arr2, size, func_add);
    if(rank == 0) {
        printf("%f\n", res1[0]);
        printf("%f\n", res1[1]);
        printf("%f\n\n", res1[2]);

        printf("%f\n", res2[0]);
        printf("%f\n", res2[1]);
        printf("%f\n\n", res2[2]);
    }
    MPI_Finalize();

}

