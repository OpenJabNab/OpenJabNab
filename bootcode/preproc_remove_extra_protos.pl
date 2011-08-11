#!/usr/bin/perl -w

# Removes "proto" declarations that are after the actual function
# definition. The compile handles that badly.

my $input;
if (@ARGV == 0) {
		$input = STDIN;
} else {
		my $filename = $ARGV[0];
		open($input, $filename) || die "Could not open $filename\n";
}

my %funs;

while (my $line = <$input>) {
		if ($line =~ m/^fun ([a-zA-Z0-9_-]+)/) {
				$funs{$1} = 1;
		} elsif ($line =~ m/^proto ([a-zA-Z0-9_-]+)/) {
				if ($funs{$1}) {
						print "//" ;
				} else {
						$funs{$1} = 1;
				}
		}
		print "$line" ;
}

close($input);
