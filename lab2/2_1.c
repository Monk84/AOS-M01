#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

extern char **environ;

int main()
{
	printf("some of initial environment variables:\n");
	int count = 0;
	for (; environ[count] != 0; ++count);
	for (int i = count - 5; i < count; ++i) {
		printf("%s\n", environ[i]);
	}
	setenv("TEMPNEWVAR", "12345", 1);
	printf("some of modified environment variables:\n");
	for (; environ[count] != 0; ++count);
	for (int i = count - 6; i < count; ++i) {
		printf("%s\n", environ[i]);
	}
	return 0;
}
