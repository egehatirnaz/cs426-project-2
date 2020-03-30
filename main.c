//
// Created by Ege Hatırnaz on 29.03.2020.
//
#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <float.h>
#include <math.h>

float *MPI_Map_Func(float *arr, int size, float (*func)(float)){
    int rank, comsize;

    MPI_Comm_size(MPI_COMM_WORLD, &comsize);   // size is no. of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float *buff = calloc(size + 1, sizeof(float));
    MPI_Scatter(&arr, size, MPI_FLOAT, buff,
                size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    float *locals = NULL;
    locals = calloc(size + 1, sizeof(float));
    float local = func(arr[rank]);
    MPI_Allgather(&local, 1, MPI_FLOAT, locals, 1, MPI_FLOAT,
               MPI_COMM_WORLD);

    for(int i = 0; i < size; i++){
        arr[i] = locals[i];
    }
    return arr;
}

float* MPI_Fold_Func(float* arr, int size, float initial_value, float (*func)(float, float)){
    int rank, comsize;
    float *result = calloc(size + 1, sizeof(float));

    MPI_Comm_size(MPI_COMM_WORLD, &comsize);   // size is no. of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float *buff = calloc(size + 1, sizeof(float));
    MPI_Scatter(&arr, size, MPI_FLOAT, buff,
                size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    float *locals = NULL;
    locals = calloc(size + 1, sizeof(float));
    float local = func(arr[rank], initial_value);
    MPI_Allgather(&local, 1, MPI_FLOAT, locals, 1, MPI_FLOAT, MPI_COMM_WORLD);

    for(int i = 0; i < size; i++){
        result[i] = locals[i];
    }
    return result;
}

float* MPI_Filter_Func(float* arr, int size, bool (*pred)(float)){
    int rank, comsize;
    float *result = calloc(size + 1, sizeof(float));

    MPI_Comm_size(MPI_COMM_WORLD, &comsize);   // size is no. of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float *buff = calloc(size + 1, sizeof(float));
    MPI_Scatter(&arr, size, MPI_FLOAT, buff,
                size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    float *locals = NULL;
    locals = calloc(size + 1, sizeof(float));
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

float multiply(float a, float initial_value){
    return a * initial_value;
}

float randomfloat(float val){
    val = (float)arc4random() / UINT32_MAX;
    return val;
}

float hitscan(float x, float y){
    float center_x, center_y;
    center_x = 0.5f;
    center_y = 0.5f;
    if(pow((x - center_x), 2) + pow((y - center_y), 2) <= pow(0.5f, 2)){
        return 1.0f;
    } else {
        return -1.0f;
    }
}

float monte_carlo(float* arr_x, int size){
    int comsize, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int hit_count = 0;
    float ratio = 0.0f;
        
    float* scan;
    float initial_value = 0;
    float* hit_results;
    
    float (*func_rand)(float) = &randomfloat;
    float (*func_hits)(float, float) = &hitscan;
    bool (*func_pos)(float) = &is_positive;
    
    // Use MPI_Map for random initialization.
    arr_x = MPI_Map_Func(arr_x, size, func_rand);
    float rand_coord = randomfloat(initial_value);
    
    // Use MPI_Fold for combining one coordinate with another random coordinate.
    scan = MPI_Fold_Func(arr_x, size, rand_coord, func_hits);
    
    // Filter if it's a hit.
    hit_results = MPI_Filter_Func(scan, size, func_pos);
    
    for(int i = 0; i < size; i++){
        if(hit_results[i] == 1.00)
            hit_count += 1;
    }
      
    return hit_count;
}

float monte_helper(float* orig_arr, int size){
    float result = 0.0f;
    int comsize, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    
    float *buff = calloc(comsize + 1, sizeof(float));
    MPI_Scatter(&orig_arr, size, MPI_FLOAT, buff,
                size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    float *locals = NULL;
    locals = calloc(size + 1, sizeof(float));
    float local = monte_carlo(buff, size/comsize);
    MPI_Allgather(&local, 1, MPI_FLOAT, locals, 1, MPI_FLOAT, MPI_COMM_WORLD); // Maybe it's a bad idea.

    for(int i = 0; i < size; i++){
        result += locals[i];
    }
    return result/size;
}

int main(int argc, char *argv[]){
    float arr1[100];
    int size = 100;

    MPI_Init(&argc, &argv);
    int rank, comsize;
    MPI_Comm_size(MPI_COMM_WORLD, &comsize);   // size is no. of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //res1 = MPI_Map_Func(arr1, size, func_add);
    //res2 = MPI_Fold_Func(arr2, size, initial_value, func_mult);
    //res3 = MPI_Filter_Func(arr3, size, func_pos);
    printf("%f\n", monte_helper(arr1, size));
    /*
    if(rank == 0) {
        printf("%f\n", res1[0]);
        printf("%f\n", res1[1]);
        printf("%f\n\n", res1[2]);

        printf("%f\n", res2[0]);
        printf("%f\n", res2[1]);
        printf("%f\n\n", res2[2]);

        printf("%f\n", res3[0]);
        printf("%f\n", res3[1]);
        printf("%f\n\n", res3[2]);
    }*/
    MPI_Finalize();

}

