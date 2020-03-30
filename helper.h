//
// Created by Ege Hatırnaz on 29.03.2020.
//
typedef enum { false, true } bool;

float* MPI_Map_Func(float* arr, int size, float (*func)(float));
float MPI_FoldFunc(float *arr, int size, float initial_value, float (*func)(float, float));
float* MPI_Filter_Func(float* arr, int size, bool (*pred)(float));
