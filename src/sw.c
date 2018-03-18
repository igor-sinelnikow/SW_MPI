#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "wrapper.h"

#include "sw.h"

void error(void (*func)(const char*), const char* str) {
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	if (rank == 0)
		func(str);
	MPI_Finalize();
	exit(EXIT_FAILURE);
}

char* read_target(const char* name, int len) {
	MPI_File fh_t;
	MPIE_File_open(MPI_COMM_WORLD,name,MPI_MODE_RDONLY,MPI_INFO_NULL,&fh_t);

	char* t = (char*) malloc(len*sizeof(char));
	if (t == NULL)
		error(perror,"malloc");

	MPIE_File_read(fh_t,t,len,MPI_CHAR,MPI_STATUS_IGNORE);

	MPI_File_close(&fh_t);
	return t;
}

char* read_query(const char* name, int L) {
	MPI_File fh_q;
	MPIE_File_open(MPI_COMM_WORLD,name,MPI_MODE_RDONLY,MPI_INFO_NULL,&fh_q);

	char* q = (char*) malloc(L*sizeof(char));
	if (q == NULL)
		error(perror,"malloc");

	MPIE_File_read_ordered(fh_q,q,L,MPI_CHAR,MPI_STATUS_IGNORE);

	MPI_File_close(&fh_q);
	return q;
}

uint max(int a, int b, int c, int d) {
	int x = (a < b) ? b : a;
	int y = (c < d) ? d : c;
	return (x < y) ? y : x;
}

uint* fill_similarity_matrix(uint local_max[], double time[], char* t, char* q,
                             int len_t, int L, int match, int mismatch,
                             int gap, int rank, int size) {
	// time[5] = -MPI_Wtime();
	int N = len_t/L, L_last = len_t%L;
	int ofs, up, diag, left;
	// double job;
	uint score;
	uint* A = (uint*) calloc((L+1)*(len_t+1),sizeof(uint));
	if (A == NULL)
		error(perror,"calloc");

	MPI_Request request = MPI_REQUEST_NULL;
	// time[5] += MPI_Wtime();
	time[0] = -MPI_Wtime();
	for (int k = 0; k < ((L_last)?N+1:N); ++k) {
		ofs = k*L+1;
		if (rank != 0)
			// time[2] +=
			MPIT_Recv(&A(0,ofs),(k==N)?L_last:L,MPI_UNSIGNED,rank-1,
			                     0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		// job = -MPI_Wtime();
		for (int i = 1; i <= L; ++i)
			for (int j = ofs; j < ((k==N)?ofs+L_last:ofs+L); ++j) {
				up   = A(i-1, j ) + gap;
				diag = A(i-1,j-1) + ((q[i-1] == t[j-1]) ? match : mismatch);
				left = A( i ,j-1) + gap;

				A(i,j) = score = max(up,diag,left,0);

				if (score > local_max[0]) {
					local_max[0] = score;
					// local_max[1] = i;
					// local_max[2] = j;
				}
			}
		// time[1] += (job += MPI_Wtime());

		if (rank != size-1) {
			if (request != MPI_REQUEST_NULL)
				// time[3] +=
				MPIT_Wait(&request,MPI_STATUS_IGNORE);
			// time[4] +=
			MPIT_Isend(&A(L,ofs),(k==N)?L_last:L,MPI_UNSIGNED,rank+1,
			                      0,MPI_COMM_WORLD,&request);
		}
	}
	time[0] += MPI_Wtime();
	return A;
}

void save_matrix(uint* A, const char* name, int len_t, int L) {
	MPI_File fh_A;
	MPIE_File_open(MPI_COMM_WORLD,name,MPI_MODE_CREATE|MPI_MODE_WRONLY,
	               MPI_INFO_NULL,&fh_A);

	MPIE_File_write_ordered(fh_A,&A(1,0),L*(len_t+1),MPI_UNSIGNED,
	                        MPI_STATUS_IGNORE);

	MPI_File_close(&fh_A);
}
/*
int find_start(int local_max, int rank, int size) {
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
                   int rank, int size) {
	int start = find_start(local_start[0],rank,size);
	int k = 0;
	if (rank != start) {
		 // MPI_Recv
		 //   local_start[1] =
		 //   local_start[2] =
	}
	int i = local_start[1];
	int j = local_start[2];
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
