#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef unsigned int uint;

static uint* read_matrix(const char* name, int len_t, int len_q) {
	int fd = open(name,O_RDONLY);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	uint* A = (uint*) malloc((len_q+1)*(len_t+1)*sizeof(uint));
	memset(A,0,(len_t+1)*sizeof(uint));

	int r = read(fd,&A[len_t+1],len_q*(len_t+1)*sizeof(uint));
	if (r == -1) {
		perror("read");
		close(fd);
		free(A);
		exit(EXIT_FAILURE);
	}
	else if (r < len_q*(len_t+1)*sizeof(uint)) {
		fprintf(stderr, "Read %lu uints\n", r/sizeof(uint));
		close(fd);
		free(A);
		exit(EXIT_FAILURE);
	}

	close(fd);
	return A;
}

static void print_usage(const char* program) {
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "%s\n", program);
	fprintf(stderr, "\t<similarity matrix file>\n");          // argv[1]
	fprintf(stderr, "\t<target sequence file (prepared)>\n"); // argv[2]
	fprintf(stderr, "\t<length of target>\n");                // argv[3]
	fprintf(stderr, "\t<query sequence file (prepared)>\n");  // argv[4]
	fprintf(stderr, "\t<length of query>\n");                 // argv[5]
	fprintf(stderr, "\t<match>\n");                           // argv[6]
	fprintf(stderr, "\t<mismatch>\n");                        // argv[7]
	fprintf(stderr, "\t<gap>\n");                             // argv[8]
}

int main(int argc, char const *argv[])
{
	if (argc != 9) {
		print_usage(argv[0]);
		return 1;
	}
	/*
	FILE* pFile_t = fopen(argv[2],"r");
	if (pFile_t == NULL) {
		perror("fopen");
		return 2;
	}
	FILE* pFile_q = fopen(argv[4],"r");
	if (pFile_q == NULL) {
		perror("fopen");
		return 3;
	}*/

	int len_t = atoi(argv[3]);
	int len_q = atoi(argv[5]);
	uint* A = read_matrix(argv[1],len_t,len_q);

	for (int i = 0; i <= len_q; ++i) {
		for (int j = 0; j <= len_t; ++j)
			printf("%d ", A[i*(len_t+1)+j]);
		putchar('\n');
	}

	// fclose(pFile_t);
	// fclose(pFile_q);
	free(A);
	return 0;
}
