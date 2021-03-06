#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>

void keyGen(int n, int q, int q3, int q9, int* x, int* y);

int rightShift(int n, int q, int* x, int* result);

// The decoding matrix adds unnecessary slowness
//int** dec_matrix(int* x, int* y);

// Returns the result of H*input without expanding matrix H
void leftMultiplyByH(int n, int q, int* x, int* y, int* v1, int* v2, int* result);

void noiseGen(int n, int q, int* r1, int* r2, int* e);

void computeSyndrome(int n, int q, int* x, int* y, int* r1, int* r2, int* e, int* result);

int test(int n, int q, int* e);

void setR1(int n, int* r1, int* result);

void setR2(int n, int* r2, int* result);

int decode(int n, int q, int q3, int* x, int* y, int* syndrome, int D_thr, int* r1_tmp, int* r2_tmp, int* p_syndrome, int* r1, int* r2, int* result);

int vectorEqual(int n, int* a, int* b);

void vectorAdd(int n, int q, int* a, int* b, int* result);

void vectorSub(int n, int q, int* a, int* b, int* result);

