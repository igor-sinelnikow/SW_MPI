#include "sw.h"

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "wrapper.h"

#ifdef BLUEGENE
#   define reduction(m)
#endif

void error(void (*func)(const char*), const char* str)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if (rank == 0)
        func(str);
    MPI_Finalize();
    exit(EXIT_FAILURE);
}

void fatal(const char* message)
{
    fprintf(stderr, "%s\n", message);
}

char* read_target(CONST char* name, uint len)
{
    MPI_File fh_t;
    MPIE_File_open(MPI_COMM_WORLD,name,MPI_MODE_RDONLY,MPI_INFO_NULL,&fh_t);

    char* t = (char*) malloc(len*sizeof(char));
    if (t == NULL)
        error(perror,"malloc");

    MPIE_File_read(fh_t,t,len,MPI_CHAR,MPI_STATUS_IGNORE);

    MPI_File_close(&fh_t);
    return t;
}

char* read_query(CONST char* name, uint L)
{
    MPI_File fh_q;
    MPIE_File_open(MPI_COMM_WORLD,name,MPI_MODE_RDONLY,MPI_INFO_NULL,&fh_q);

    char* q = (char*) malloc(L*sizeof(char));
    if (q == NULL)
        error(perror,"malloc");

    MPIE_File_read_ordered(fh_q,q,L,MPI_CHAR,MPI_STATUS_IGNORE);

    MPI_File_close(&fh_q);
    return q;
}

#define min(x,y) (((x)<(y))?(x):(y))
#define max(x,y) (((x)<(y))?(y):(x))

static uint max4(int a, int b, int c, int d)
{
    int x = max(a, b);
    int y = max(c, d);
    return (x < y) ? y : x;
}

static double fill_block_omp(uint local_max[], uint* A, char* t, char* q,
                             int len_t, int height, int ofs, int width,
                             int match, int mismatch, int gap)
{
    uint max_score = 0;
    double time = -MPI_Wtime();
    #pragma omp parallel reduction(max:max_score)
    {
        int nthreads = omp_get_num_threads();
        int threadnum = omp_get_thread_num();
        int L = height/nthreads;
        int begin = L*threadnum + 1;
        int end = begin + L;
        int j;
        int up, diag, left;
        uint score;
        for (int step = 0; step < width+nthreads-1; ++step) {
            j = ofs + step - threadnum;
            if (j >= ofs && j < ofs + width)
                for (int i = begin; i < end; ++i) {
                    up   = A(i-1, j ) + gap;
                    diag = A(i-1,j-1) + ((q[i-1] == t[j-1]) ? match : mismatch);
                    left = A( i ,j-1) + gap;

                    A(i,j) = score = max4(up,diag,left,0);
                    if (score > max_score)
                        max_score = score;
                }
            #pragma omp barrier
        }
#ifdef BLUEGENE
        #pragma omp flush(max_score)
        if (score > max_score) {
            #pragma omp critical
            {
                if (score > max_score)
                    max_score = score;
            }
        }
#endif
    }
    if (max_score > local_max[0]) {
        local_max[0] = max_score;
        // local_max[1] = i;
        // local_max[2] = j;
    }
    return (time += MPI_Wtime());
}

static double fill_block_orig(uint local_max[], uint* A, char* t, char* q,
                              uint len_t, uint L, uint ofs, uint width,
                              int match, int mismatch, int gap)
{
    int up, diag, left;
    uint score;
    double time = -MPI_Wtime();
    for (int j = 1; j <= L; ++j)
        for (int i = ofs; i < ofs+width; ++i) {
            up   = A( i ,j-1) + gap;
            diag = A(i-1,j-1) + ((t[i-1] == q[j-1]) ? match : mismatch);
            left = A(i-1, j ) + gap;

            A(i,j) = score = max4(up,diag,left,0);

            if (score > local_max[0]) {
                local_max[0] = score;
                // local_max[1] = i;
                // local_max[2] = j;
            }
        }
    return (time += MPI_Wtime());
}

static uint elementsOnDiag(uint d, uint width, uint height)
{
    if (d <= width && d <= height)
        return d;
    if (d <= max(width, height))
        return min(width, height);
    if (d <= width + height - 1)
        return width + height - d;
    return 0;
}

static uint col(uint d, uint e, uint width, uint height)
{
    uint x;
    if (d <= width && d <= height)
        x = d;
    else if (d <= max(width, height))
        x = (width < height) ? width : d;
    else if (d <= width + height - 1)
        x = width;
    else
        error(fatal,"sw.c:161: Too many diagonals in a matrix");
    return x - e;
}

static uint row(uint d, uint e, uint width, uint height)
{
    uint y;
    if (d <= width && d <= height)
        y = 1;
    else if (d <= max(width, height))
        y = (width < height) ? d-width+1 : 1;
    else if (d <= width + height - 1)
        y = d - width + 1;
    else
        error(fatal,"sw.c:175: Too many diagonals in a matrix");
    return y + e;
}

static double fill_block(uint local_max[], uint* A, char* t, char* q,
                         uint len_t, uint height, uint offset, uint width,
                         int match, int mismatch, int gap)
{
    int up, diag, left;
    uint i, j, score;
    const uint nDiag = width + height - 1;
    double time = -MPI_Wtime();
    for (uint d = 1; d <= nDiag; ++d) {
        for (uint e = 0; e < elementsOnDiag(d,width,height); ++e) {
            i = col(d,e,width,height) + offset;
            j = row(d,e,width,height);

            // printf("(%u,%u) ", i, j);

            up   = A( i ,j-1) + gap;
            diag = A(i-1,j-1) + ((t[i-1] == q[j-1]) ? match : mismatch);
            left = A(i-1, j ) + gap;

            A(i,j) = score = max4(up,diag,left,0);

            if (score > local_max[0]) {
                local_max[0] = score;
                // local_max[1] = i;
                // local_max[2] = j;
            }
        }
        // putchar('\n');
    }
    return (time += MPI_Wtime());
}

uint* fill_similarity_matrix(uint local_max[], double time[], char* t, char* q,
                             uint len_t, uint L, int match, int mismatch,
                             int gap, int rank, int size)
{
    // time[5] = -MPI_Wtime();
    uint N = len_t/L, L_last = len_t%L;
    uint ofs, width;
    uint* A = (uint*) calloc((len_t+1)*(L+1),sizeof(uint));
    if (A == NULL)
        error(perror,"calloc");

    MPI_Request request = MPI_REQUEST_NULL;
    // time[5] += MPI_Wtime();
    time[0] = -MPI_Wtime();
    for (uint k = 0; k < (L_last ? N+1 : N); ++k) {
        ofs = 1+k*L;
        width = (k == N) ? L_last : L;
        if (rank != 0)
            // time[2] +=
            MPIT_Recv(&A(ofs,0),width,MPI_UNSIGNED,rank-1,0,MPI_COMM_WORLD,
                      MPI_STATUS_IGNORE);

        // time[1] +=
        // fill_block_omp(local_max,A,t,q,len_t,L,ofs,width,match,mismatch,gap);
        fill_block(local_max,A,t,q,len_t,L,ofs-1,width,match,mismatch,gap);

        if (rank != size-1) {
            if (request != MPI_REQUEST_NULL)
                // time[3] +=
                MPIT_Wait(&request,MPI_STATUS_IGNORE);
            // time[4] +=
            MPIT_Isend(&A(ofs,L),width,MPI_UNSIGNED,rank+1,0,MPI_COMM_WORLD,
                       &request);
        }
    }
    time[0] += MPI_Wtime();
    return A;
}

void save_matrix(uint* A, CONST char* name, uint len_t, uint L)
{
    MPI_File fh_A;
    MPIE_File_open(MPI_COMM_WORLD,name,MPI_MODE_CREATE|MPI_MODE_WRONLY,
                   MPI_INFO_NULL,&fh_A);

    MPIE_File_write_ordered(fh_A,&A(0,1),L*(len_t+1),MPI_UNSIGNED,
                            MPI_STATUS_IGNORE);

    MPI_File_close(&fh_A);
}
/*
int find_start(int local_max, int rank, int size)
{
    int errorcode;

    int* maximums = (int*) malloc(size*sizeof(int));
    errorcode = MPI_Allgather(&local_max,1,MPI_INT,maximums,1,MPI_INT,
                              MPI_COMM_WORLD);
    if (errorcode != MPI_SUCCESS)
        Abort(__FILE__,__LINE__-2,"MPI_Allgather",errorcode);

    int start = size-1;
    int global_max = maximums[start];
    for (int p = start-1; p >= 0; --p)
        if (maximums[p] > global_max) {
            global_max = maximums[p];
            start = p;
        }

    free(maximums);
    return start;
}*/
/*
int sticks_n_stars(char* sns, int local_start[], char* t, char* q,
                   int len_t, int L, int match, int mismatch, int gap,
                   int rank, int size)
{
    int start = find_start(local_start[0],rank,size);
    int k = 0;
    if (rank != start) {
         // MPI_Recv
         //   local_start[1] =
         //   local_start[2] =
    }
    int i = local_start[1];
    int j = local_start[2];
    // TODO: проверить i и j, потому что в макросе поменял местами
    while (((i > 0) || (j > 0)) && (A(i,j) > 0)) {
        if ((i > 0) && (A(i,j) == A(i-1,j)+gap)) {
            sns[k] = '*';
            ++k; --i;
        }
        else if ((i > 0) && (j > 0) &&
                 (A(i,j) == A(i-1,j-1)+((q[i-1]==t[j-1])?match:mismatch))) {
            sns[k] = '|';
            ++k; --i; --j;
        }
        else if ((j > 0) && (A(i,j) == A(i,j-1)+gap)) {
            sns[k] = '*';
            ++k; --j;
        }
        else {
            printf("i = %d, j = %d, A[i,j] = %d\n", i, j, A(i,j));
            break;
        }
    };
    if (rank != 0) {
        errorcode = MPI_Send(&j,1,MPI_INT,rank-1,0,MPI_COMM_WORLD);
        if (errorcode != MPI_SUCCESS)
            Abort(__FILE__,__LINE__-2,"MPI_Send",errorcode);
    }

    return k;
}*/
