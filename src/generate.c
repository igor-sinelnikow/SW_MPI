#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void print_usage(const char* program) {
	fprintf(stderr, "Формат командной строки:\n");
	fprintf(stderr, "%s\n", program);
	fprintf(stderr, "\t<имя файла>\n");                // argv[1]
	fprintf(stderr, "\t<длина последовательности>\n"); // argv[2]
}

int main(int argc, char const *argv[])
{
	if (argc != 3) {
		print_usage(argv[0]);
		return 1;
	}

	FILE* pFile = fopen(argv[1],"w");
	if (pFile == NULL) {
		perror("fopen");
		return 2;
	}

	int l = atoi(argv[2]);
	fprintf(pFile, ">random sequence, length %d\n", l);

	srand(time(NULL));
	for (int i = 0; i < l; ++i)
		switch (rand()%4) {
		case 0:
			fputc('A',pFile);
			break;
		case 1:
			fputc('C',pFile);
			break;
		case 2:
			fputc('G',pFile);
			break;
		case 3:
			fputc('T',pFile);
			break;
		}
	fputc('\n',pFile);

	fclose(pFile);
	return 0;
}
