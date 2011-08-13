#!/usr/bin/perl

#use strict;

my $nb_args = $#ARGV + 1;

if( $nb_args < 2 ) {
	print "usage: preprocessor.pl <source file> <output file>\n";
	exit(0);
}
my $temp_file = $ARGV[1] . ".cm";
`cat $ARGV[0] | ./c_source_analyzer > $temp_file`;
PreProcess($temp_file, $ARGV[1]);

exit;

#
# PreProcess
# Args: input file to preprocesss
#       output file : preprocessed file
#
sub PreProcess() {
	my $fhi;
	my $fho;
	my %defHash = ();
	my @defRes = ();

	my ($infile, $outfile) = @_;
	open($fhi, "<" . $infile) || die "Cant open: " . $infile . "\n";
	open($fho, ">" . $outfile) || die "Cant open: " . $outfile . "\n";
	
	while(<$fhi>) {
		if($_ =~ m/#define\s+([a-zA-Z0-9_]+)\s+(.+)/) {
			$defHash{$1} = $2;
		} elsif($_ =~ m/#define\s+([a-zA-Z0-9_]+)/) {
			$defHash{$1} = '';
		} elsif($_ =~ m/#undef\s+([a-zA-Z0-9_]+)\s*\n/) {
			delete $defHash{$1};
		} elsif ($_ =~ m/#ifdef\s+([a-zA-Z0-9_]+)\s*\n/) {
			my $key = $1;
			if( exists $defHash{$key} ) {
				push @defRes,1;
			} else {
				push @defRes,0;
			}
		} elsif ($_ =~ m/#endif/) {
			pop @defRes;
		} else {
			if($#defRes != -1) {
				if($defRes[$#defHash] == 1) {
					#print $_;
					print $fho $_;
				}
			} else {
				#print $_;
				print $fho $_;
			}
		}
	}
	
	close($fhi);
	close($fho);
}


# ToDo: finish!
sub ProcessInclude() {
	my ($infile) = @_;

	open($fhi, "<" . $infile) || die "Cant open: " . $infile . "\n";

	while(<$fhi>)	{
		my $dest;
		my $build_list;
		if($_ =~ m/[ \t]*(.+):(.+)\n/) {
			$dest = $1;
			$build_list = $2;
		}
	}
	close($fhi);
}

