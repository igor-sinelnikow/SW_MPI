#pragma once

#define A(x,y) A[(x)*(len_t+1) + (y)]

typedef unsigned int uint;

void error(void (*func)(const char*), const char* str);

char* read_target(const char* name, int len);

char* read_query(const char* name, int L);

uint max(int a, int b, int c, int d);

uint* fill_similarity_matrix(uint local_max[], double time[], char* t, char* q,
                             int len_t, int L, int match, int mismatch,
                             int gap, int rank, int size);

void save_matrix(uint* A, const char* name, int len_t, int L);

// int find_start(int local_max, int rank, int size);
/*
int sticks_n_stars(char* sns, int local_start[], char* t, char* q,
                   int len_t, int L, int match, int mismatch, int gap,
                   int rank, int size);*/
/*
void align(char* align_t, char* align_q, int local_start[], char* t, char* q,
           int len_t, int L, int rank, int size);*/
