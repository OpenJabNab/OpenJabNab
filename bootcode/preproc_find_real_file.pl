#!/usr/bin/perl -w

(@ARGV == 2) || die "Need two arguments: filename and linenum" ;

my $filename = $ARGV[0];
my $lineNum = $ARGV[1];


my $F ;
open($F, $filename) || die "Could not open file $filename" ;

my $currentLineInMainFile = 0;

my $currentFilename = $filename;
my $currentLineNum = 1;

while (my $line = <$F>) {
		$currentLineInMainFile++;
		if ($currentLineInMainFile == $lineNum) {
				last ;
		}
		if (($line =~ m/^\/\/ file ([a-zA-Z0-9_.-]+)/) && ($line !~ m/already included/)) {
				$currentFilename = $1;
				$currentLineNum = 1;
		} elsif ($line =~ m/^\/\/ back to file ([a-zA-Z0-9_.-]+), line ([0-9]+)/ ) {
				$currentFilename = $1;
				$currentLineNum = $2;
		} elsif ($line =~ m/^\/\/ end of file ([a-zA-Z0-9_.-]+)/ ) {
				# skip
				;
		} else {
				$currentLineNum++;
		}
}

print "$currentFilename, line $currentLineNum\n";
