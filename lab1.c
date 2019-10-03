#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>
#include <inttypes.h>

#define BUFFER_SIZE 1024

static uintmax_t total_lines;
static uintmax_t total_bytes;

static int print_lines, print_bytes;
static int have_read_stdin;

static struct option const longopts[] =
{
	{"bytes", no_argument, NULL, 'c'},
	{"lines", no_argument, NULL, 'l'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, NULL, 0}
};

void print_error(int error)
{
	write(STDERR_FILENO, strerror(error), strlen(strerror(error)));
	write(STDERR_FILENO, "\n", 1);
	exit(error);
}

void write_counts(uintmax_t lines, uintmax_t bytes, const char *file)
{
	if (print_bytes) {
		char str[64] = {0x0};
		int size = sprintf(str, "%lu ", bytes);
		write(STDOUT_FILENO, str, size);
	}

	if (print_lines) {
		char str[64] = {0x0};
		int size = sprintf(str, "%lu ", lines);
		write(STDOUT_FILENO, str, size);
	}

	if (file) {
		write(STDOUT_FILENO, file, strlen(file));
	}
	write(STDOUT_FILENO, "\n", 1);
}

int wc(int fd, char const *file_name)
{
	char buf[BUFFER_SIZE + 1];
	size_t bytes_read;
	uintmax_t lines, bytes;
	lines = bytes = 0;

	while ((bytes_read = read(fd, buf, BUFFER_SIZE)) > 0)
	{
		char *p = buf;
		while ((p = memchr (p, '\n', (buf + bytes_read) - p)))
		{
			++p;
			++lines;
		}
		bytes += bytes_read;
	}

	write_counts (lines, bytes, file_name);
	total_lines += lines;
	total_bytes += bytes;

	return 0;
}

int wc_file(char const *file)
{
	if (!file || strcmp(file, "-") == 0)
	{
		return wc(STDIN_FILENO, file);
	}
	else
	{
		int fd = open(file, O_RDONLY);
		if (fd == -1)
		{
			print_error(errno);
			return -1;
		}
		else
		{
			int ok = wc(fd, file);
			if (close(fd) != 0)
			{
				print_error(errno);
				return -1;
			}
			return ok;
		}
	}
}

void usage()
{
	int state;
	state = write(STDERR_FILENO, "usage: wc params\n", 17);
	if (state < 0)
		print_error(errno);
	exit(state);
}

int main(int argc, char* argv[])
{
	int nfiles;
	char **files;
	int optc;
	print_lines = print_bytes = 0;
	total_lines = total_bytes = 0;

	while ((optc = getopt_long (argc, argv, "clh", longopts, NULL)) != -1)
	{
		switch (optc)
		{
		case 'c':
			print_bytes = 1;
			break;

		case 'l':
			print_lines = 1;
			break;

		case 'h':
			usage();
			break;

		default:
			usage();
			break;
		}
	}

	if (!print_bytes && !print_lines)
		print_bytes = print_lines = 1;

	static char *stdin_only[] = { NULL };

	int count = 0;
	for (; optind < argc; optind++)
	{
		count++;
		wc_file(argv[optind]);
	}

	if (count > 1) {
		write_counts(total_lines, total_bytes, "итого");
	}

	exit(0);
}
