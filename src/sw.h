#pragma once

#ifndef BLUEGENE
#   define CONST const
#else
#   define CONST
#endif

#define A(x,y) A[(x) + (y)*(len_t+1)]

typedef unsigned int uint;

void error(void (*func)(const char*), const char* str) __attribute__((noreturn));

void fatal(const char* message);

char* read_target(CONST char* name, uint len);

char* read_query(CONST char* name, uint L);

uint* fill_similarity_matrix(uint local_max[], double time[], char* t, char* q,
                             uint len_t, uint L, int match, int mismatch,
                             int gap, int rank, int size, int ndev);

void save_matrix(uint* A, CONST char* name, uint len_t, uint L);

// int find_start(int local_max, int rank, int size);
/*
int sticks_n_stars(char* sns, int local_start[], char* t, char* q,
                   int len_t, int L, int match, int mismatch, int gap,
                   int rank, int size);*/
/*
void align(char* align_t, char* align_q, int local_start[], char* t, char* q,
           int len_t, int L, int rank, int size);*/
