#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s filenames\n", argv[0]);
		return 1;
	}
	off_t max_len = 0;
	char *max_len_fname;
	struct stat st_buf;
	for (int i = 1; i < argc; ++i) {
		stat(argv[i], &st_buf);
		if (st_buf.st_size > max_len) {
			max_len = st_buf.st_size;
			max_len_fname = &argv[i][0];
		}
	}
	printf("biggest file is \"%s\" of len %lu bytes\n", max_len_fname, max_len);
	return 0;
}
