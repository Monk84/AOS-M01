#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

void cpy(FILE *to, FILE *from)
{
	int c;
	while ((c = fgetc(from)) != EOF)
		fputc(c, to);
}

int main(int argc, char *argv[])
{
	char *f1, *f2;
	int malloced = 0;
	if (3 == argc) {
		f1 = argv[1];
		f2 = argv[2];
	} else if (1 == argc) {
		malloced = 1;
		f1 = calloc(50, sizeof(*f1));
		f2 = calloc(50, sizeof(*f2));
		printf("Enter first file name: ");
		scanf("%s", f1);
		printf("Enter second file name: ");
		scanf("%s", f2);
	} else {
		printf("Usage: %s [f1 f2]\n", argv[0]);
		exit(1);
	}
	FILE *from = fopen(f1, "r");
	FILE *to = fopen(f2, "w");
	cpy(to, from);
	fclose(to);
	fclose(from);
	if (malloced) {
		free(f1);
		free(f2);
	}
	return 0;
}
