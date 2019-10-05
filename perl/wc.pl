#!/usr/bin/perl -T

use strict; 
use warnings qw(FATAL all);
use Class::Struct;
use Syntax::Keyword::Try;

struct State => {
    characterCounter   => '$',
    wordsCounter       => '$',
    linesCounter       => '$',
};


sub printReport {
    my ($state, $fileName) = @_;
    printf(
        "%6lu %6lu %6lu %s\n", 
        $state -> linesCounter, 
        $state -> wordsCounter, 
        $state -> characterCounter, 
        $fileName
    );
}

sub initializeStruct {
     my ($state) = @_;   
     $state -> linesCounter(0);
     $state -> wordsCounter(0);
     $state -> characterCounter(0);
}

sub countWordsNumber {
    my ($line) = @_;
    my $num_words = 0;
    ++$num_words while $line =~ /\S+/g;
    return $num_words;
}

sub checkEndOfLine {
    my ($line) = @_;
    my $result = $line =~ /\n/;
    return $result;
}

sub processFile {
    my ($fileName) = @_;
    my $currentState = State -> new();
    initializeStruct($currentState);
    if(open(DATA, "<$fileName")) {
        foreach my $currentLine(<DATA>) {
            $currentState -> linesCounter ($currentState -> linesCounter + checkEndOfLine($currentLine));
            $currentState -> wordsCounter ($currentState -> wordsCounter + countWordsNumber($currentLine));
            $currentState -> characterCounter ($currentState -> characterCounter + length($currentLine));
        }
        printReport($currentState, $fileName);    
    } else {
        print("wc: $fileName: No such file or directory\n");
    }
    return $currentState;
}

sub aggregateInfo {
    my (@collection) = @_;
    my $state = State -> new();
    initializeStruct($state);
    foreach my $item(@collection) {
        $state -> wordsCounter($state -> wordsCounter + $item -> wordsCounter);
        $state -> characterCounter($state -> characterCounter + $item -> characterCounter);
        $state -> linesCounter($state -> linesCounter + $item -> linesCounter);
    } 
    return $state;
}

sub mainRoutine {
    my @listOfAllProceedStates = ();
    foreach my $current_filename(@ARGV) {
        push(@listOfAllProceedStates, processFile($current_filename));
    }
    my $total = aggregateInfo(@listOfAllProceedStates);
    if(scalar @listOfAllProceedStates > 1) {
        printReport($total, "total");
    }
}

mainRoutine();