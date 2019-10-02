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

// #define STDIN_FILENO 0
// #define STDOUT_FILENO 1
// #define STDERR_FILENO 2
#define BUFFER_SIZE 1024

static uintmax_t total_lines;
static uintmax_t total_words;
static uintmax_t total_chars;
static uintmax_t total_bytes;

static int print_lines, print_words, print_chars, print_bytes;
static int have_read_stdin;

static struct option const longopts[] =
{
	{"bytes", no_argument, NULL, 'c'},
	{"chars", no_argument, NULL, 'm'},
	{"lines", no_argument, NULL, 'l'},
	{"words", no_argument, NULL, 'w'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, NULL, 0}
};

void print_error(int error)
{
	write(STDERR_FILENO, strerror(error), strlen(strerror(error)));
	write(STDERR_FILENO, "\n", 1);
	exit(error);
}

void write_counts (uintmax_t lines, uintmax_t bytes, const char *file)
{
	if (print_bytes) {
		char tmp[64] = {0x0};
		sprintf(tmp, "%lu", bytes);
		write(STDOUT_FILENO, tmp, sizeof(tmp));
		write(STDOUT_FILENO, " ", 1);
	}

	if (print_lines) {
		char tmp[64] = {0x0};
		sprintf(tmp, "%lu", bytes);
		write(STDOUT_FILENO, tmp, sizeof(tmp));
		write(STDOUT_FILENO, " ", 1);
	}

	if (file) {
		write(STDOUT_FILENO, file, sizeof(file));
	}
	write(STDOUT_FILENO, "\n", 1);
}

int wc(int fd, char const *file_name)
{
	char buf[BUFFER_SIZE + 1];
	size_t bytes_read;
	uintmax_t lines, words, bytes;
	// printf("file: %s\n", file_name);
	lines = words = bytes = 0;
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
	// printf("file: %s bytes: %lu lines: %lu words: %lu \n", file_name, bytes, lines, words);
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
	print_lines = print_words = print_chars = print_bytes = 0;
	total_lines = total_words = total_chars = total_bytes = 0;

	while ((optc = getopt_long (argc, argv, "clmwh", longopts, NULL)) != -1)
	{
		switch (optc)
		{
		case 'c':
			print_bytes = 1;
			// write(STDERR_FILENO, "c\n", 2);
			break;

		case 'm':
			print_chars = 1;
			// write(STDERR_FILENO, "m\n", 2);
			break;

		case 'l':
			print_lines = 1;
			// write(STDERR_FILENO, "l\n", 2);
			break;

		case 'w':
			print_words = 1;
			// write(STDERR_FILENO, "w\n", 2);
			break;

		case 'h':
			// write(STDERR_FILENO, "h\n", 2);
			usage();
			break;

		default:
			usage();
			break;
		}
	}

	static char *stdin_only[] = { NULL };
	// files = (optind < argc ? argv + optind : stdin_only);
	// nfiles = (optind < argc ? argc - optind : 1);

	//Parse extra arguments
	for (; optind < argc; optind++)
	{
		// printf("extra arguments: %s\n", argv[optind]);
		wc_file(argv[optind]);
	}

}
