#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MIN(a, b) ((a < b) ? a : b)

/* 2_11
 * Выполнить из программы на Си какую-либо команду shell (cp или ls):
 * 1) с помощью forc-exec,
 * 2) с помощью system.
 * Необходимые для команды передать через аргументы командной строки
 * */

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s args\n", argv[0]);
		exit(1);
	}
	pid_t forked;
	switch((forked = fork())) {
	case -1: //err
		perror("fork:");
		return 2;
		break;
	case 0: {
		// exec
		printf("\nCHILD ls: ");
		char *args[10] = {"ls", 0};
		for (int i = 1; i < MIN(argc, 8); ++i)
			args[i] = argv[i];
		execvp("ls", args);
		break;
	}
	default: {
		printf("\nPARENT ls: ");
		int printed;
		char task[100] = {0};
		printed = sprintf(task, "ls ");
		for (int i = 1; i < argc; ++i)
			printed += sprintf(task + printed, "%s ", argv[i]);
		system(task);
		break;
	}
	};
	return 0;
}

