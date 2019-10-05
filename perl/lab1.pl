#!/usr/bin/perl

use Getopt::Std;

$count_words = 0;
$count_lines = 0;
$total_words = 0;
$total_lines = 0;
$multiple_files = 0;

# args: words lines file
sub print_result {
	$words = shift;
	$lines = shift;
	$file = shift;
	if ($count_words) {
		print "$words ";
	}
	if ($count_lines) {
		print "$lines ";
	}
	print "$file\n";
}

# args: file
sub wc_file {
	$file = shift;
	$word_counter = 0;
	$line_counter = 0;

	if ($file eq "-") {
		@input;
		while (<STDIN>) {
    		++$line_counter if $_ =~ /\n/;
	       	++$word_counter while $_ =~ /\S+/g;
		}
	} 
	else {
		if (open(FD, '<', $file)) {
	        while ($currentLine = <FD>) {
	        	++$line_counter if $currentLine =~ /\n/;
	        	++$word_counter while $currentLine =~ /\S+/g;
			}
	    } else {
	        print "wc: $file: No such file or directory\n";
	    }	
	}
	$total_lines += $line_counter;
	$total_words += $word_counter;
	print_result $word_counter, $line_counter, $file;
}

getopts('lw');

if (defined $opt_w) {
	$count_words = 1;
}

if (defined $opt_l) {
	$count_lines = 1;
}

if (!$count_words && !$count_lines) {
	$count_words = $count_lines = 1;
}

@listOfFiles = ();
foreach $filename(@ARGV) {
	next if $filename =~ /^\-(wl|lw|[wl])$/;
	push(@listOfFiles, $filename);
}

foreach $file(@listOfFiles) {
	wc_file $file;
}

print_result $total_words, $total_lines, "итого" if scalar @listOfFiles > 1;