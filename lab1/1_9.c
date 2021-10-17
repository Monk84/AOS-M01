#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

void cpy(void)
{
	int c;
	while ((c = getchar()) != EOF)
		putchar(c);
}

int main(int argc, char *argv[])
{
	char *f1, *f2;
	if (3 == argc) {
		f1 = argv[1];
		f2 = argv[2];
	} else {
		printf("Usage: %s [f1 f2]\n", argv[0]);
		exit(1);
	}
	freopen(f1, "r", stdin);
	freopen(f2, "w", stdout);
	cpy();
	freopen("/dev/stdin", "r", stdin);
	freopen("/dev/stdout", "w", stdout);
	return 0;
}
