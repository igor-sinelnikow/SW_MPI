#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>

static void print_usage(const char* program) {
	fprintf(stderr, "Формат командной строки:\n");
	fprintf(stderr, "%s\n", program);
	fprintf(stderr, "\t<имя файла в формате FASTA>\n"); // argv[1]
	fprintf(stderr, "\t<имя выходного файла>\n");       // argv[2]
	fprintf(stderr, "\t<[ограничение на длину последовательности]>\n"); // argv[3]
}

int main(int argc, char const *argv[])
{
	if ((argc < 3) || (argc > 4)) {
		print_usage(argv[0]);
		return 1;
	}

	FILE* pFile = fopen(argv[1],"r");
	if (pFile == NULL) {
		perror("fopen");
		return 2;
	}

	int fd = open(argv[2],O_CREAT|O_EXCL|O_WRONLY,0666);
	if (fd == -1) {
		if (errno == EEXIST)
			return 0;
		perror("open");
		return 3;
	}

	int limit = INT_MAX;
	if (argc == 4)
		limit = atoi(argv[3]);
	// printf("%d\n", limit);

	int c;
	do {
		c = fgetc(pFile);
		putchar(c);
	} while (!iscntrl(c));

	int len = 0;
	char nuc;
	while ((c = fgetc(pFile)) != EOF) {
		if (isspace(c))
			continue;
		nuc = (char) toupper(c);
		write(fd,&nuc,sizeof(char));
		++len;
		if ((argc == 4) && (len >= limit))
			break;
	};
	printf("length: %d\n", len);

	fclose(pFile);
	close(fd);
	return 0;
}
