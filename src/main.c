#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include "sw.h"

static void print_usage(const char* program)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s\n", program);
    fprintf(stderr, "\t<target sequence file (prepared)>\n"); // argv[1]
    fprintf(stderr, "\t<length of target>\n");                // argv[2]
    fprintf(stderr, "\t<query sequence file (prepared)>\n");  // argv[3]
    fprintf(stderr, "\t<length of query>\n");                 // argv[4]
    fprintf(stderr, "\t<match>\n");                           // argv[5]
    fprintf(stderr, "\t<mismatch>\n");                        // argv[6]
    fprintf(stderr, "\t<gap>\n");                             // argv[7]
    fprintf(stderr, "\t[similarity matrix file (output)]\n"); // argv[8]
}

int main(int argc, char *argv[])
{
    int rank, size;

    MPI_Init(&argc,&argv);

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if (argc < 8)
        error(print_usage,argv[0]);

    const int ndev = omp_get_num_devices();

    if (rank == 0) {
        int nthreads;
        #pragma omp parallel
            nthreads = omp_get_num_threads();
        fprintf(stderr, "size = %d, nthreads = %d, ndevices = %d\n",
                size, nthreads, ndev);
    }

    if (ndev && ndev < size)
        error(fatal,"The number of devices is less than the number "
                    "of MPI processes. Hence, some devices "
                    "will have to be shared between a couple of "
                    "processes, and it doesn't work. Aborting...");
    omp_set_default_device(rank % ndev);

    const uint len_t = atoi(argv[2]);
    char* t = read_target(argv[1],len_t);

    // TODO: если длина query не делится нацело на size, то перед
    // MPIE_File_read_ordered дополнять последовательность до нужного размера
    const uint len_q = atoi(argv[4]);
    if (len_q % size)
        error(fatal,"Query sequence length does not divide evenly "
                    "by the number of MPI processes. Can't handle "
                    "this yet, results may be wrong. Aborting...");
    const uint L = len_q/size;
    char* q = read_query(argv[3],L);

    const int match = atoi(argv[5]);
    const int mismatch = atoi(argv[6]);
    const int gap = atoi(argv[7]);

    uint local_max[1] = {0};
    double time[1] = {0.};
    // time[6] = -MPI_Wtime();
    uint* A = fill_similarity_matrix(local_max,time,t,q,len_t,L,
                                     match,mismatch,gap,rank,size,ndev);
    // time[6] += MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);

    uint max_score;
    MPI_Reduce(&local_max[0],&max_score,1,MPI_UNSIGNED,MPI_MAX,0,MPI_COMM_WORLD);

    double max_time;
    MPI_Reduce(&time[0],&max_time,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);

    /*
    for (int p = 0; p < size; ++p) {
        if (rank == p) {
            // printf("rank %d:\n", rank);
            printf("%f\n", time[1]); // recv
            printf("%f\n", time[0]); // work
            printf("%f\n", time[2]); // wait
            printf("%f\n", time[3]); // send
            putchar('\n');
            fflush(stdout);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }*/
    /*
    for (int p = 0; p < size; ++p) {
        if (rank == p) {
            printf("rank %d:\n", rank);
            printf(" send/receive/wait: %f\n", time[1]+time[2]+time[3]);
            // printf("              decl: %f\n", time[5]);
            // printf("              recv: %f\n", time[1]);
            printf("              work: %f\n", time[0]);
            // printf("              wait: %f\n", time[2]);
            // printf("              send: %f\n", time[3]);
            printf("             total: %f\n", time[0]+time[1]+time[2]+time[3]);
            // printf("               sum: %f\n", time[5]+time[0]+time[1]+time[2]+time[3]);
            // printf("         total_int: %f\n", time[4]+time[5]);
            // printf("         total_ext: %f\n", time[6]);
            printf("       effectivity: %.2f%%\n", time[0]/(time[0]+time[1]+time[2]+time[3])*100);
            fflush(stdout);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }*/
    /*
    for (int p = 0; p < size; ++p) {
        if (rank == p) {
            for (int i = 0; i <= L; ++i) {
                for (int j = 0; j <= len_t; ++j)
                    printf("%d ", A(i,j));
                putchar('\n');
            }
            // printf("local max = %d at (%d,%d)\n\n", local_max[0], local_max[1], local_max[2]);
            fflush(stdout);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }*/

    if (rank == 0) {
        fprintf(stderr, "maxValue = %u\n", max_score);
        fprintf(stderr, "%f\n", max_time);
    }

    if (argc > 8)
        save_matrix(A,argv[8],len_t,L);

    // char* sns = (char*) malloc((len_t+L)*sizeof(char));
    // int len_align = sticks_n_stars(sns,local_max,t,q,len_t,L,match,mismatch,gap,rank,size);

    // char* align_t = (char*) malloc((len_t+L)*sizeof(char));
    // char* align_q = (char*) malloc((len_t+L)*sizeof(char));
    // align(align_t,align_q,local_max,t,q,len_t,L,rank,size);

    #pragma omp target exit data map(delete: t[0:len_t])
    #pragma omp target exit data map(delete: q[0:L])
    #pragma omp target exit data map(delete: A[:(len_t+1)*(L+1)])
    free(t);
    free(q);
    free(A);
    // free(sns);
    // free(align_t);
    // free(align_q);
    MPI_Finalize();
    return 0;
}
