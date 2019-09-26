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

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
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

void usage ()
{
	int state;
	state = write(STDERR_FILENO, "usage: wc params\n", 17);
	if (state < 0)
		print_error(errno);
	exit(state);
}

int main(int argc, char* argv[])
{
	int optc;
	print_lines = print_words = print_chars = print_bytes = 0;
  	total_lines = total_words = total_chars = total_bytes = 0;

	while ((optc = getopt_long (argc, argv, "clmwh", longopts, NULL)) != -1)
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
