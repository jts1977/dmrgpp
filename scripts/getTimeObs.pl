#!/usr/bin/perl -w

use strict;

my ($site,$file,$whatState,$whatObservable)=@ARGV;

my $labelDoubleOcc = "site nupNdown(gs) nupNdown(timevector) time";
my $labelDensity = "site nUp+nDown(gs) nup+ndown(timevector) time";
my $label = $labelDoubleOcc;

$label = $labelDensity if ($whatObservable eq "density");

defined($site) or die "$0: Undefined site\n";
defined($file) or die "$0: Undefined file\n";
defined($whatState) or die "$0: Must specify what state gs or time\n";
defined($whatObservable) or die "$0: Must specify what observable (density or nd)\n";

my $stateIndex = 2;
$stateIndex = 1 if ($whatState eq "gs");


my $sd = getSuperDensity($site,$file);
$sd = 1 if ($whatState eq "gs");

open(FILE,$file) or die "Cannot open file $file: $!\n";
while(<FILE>) {
	if (/\Q${label}/) {
		last;
	}
}

my $prevT = -1;
while(<FILE>) {
	next if (/^VectorWithOffsets/);
	last if (/^#/);
	my @temp=split;
	last unless $temp[0]=~/^(\d+\.?\d*|\.\d+)$/;  # match valid number
	if ($temp[0]==$site) {
		my $val = $temp[$stateIndex];
		$val =~ s/\(//;
		$val =~ s/,.*\)//;
		next if ($prevT == $temp[3]);
		$prevT = $temp[3];
		$val /= $sd;
		print "$temp[3] $val\n";
	}
}
close(FILE);
	
sub getSuperDensity
{
	my ($site,$file)=@_;
	my $sd;
	open(FILE,$file) or die "Cannot open file $file: $!\n";
	while(<FILE>) {
		if (/SuperDensity.*=\(([^,]+),/) {
			$sd = $1;
			last;
		}
	}
	close(FILE);
	defined $sd or die "SuperDensity is not defined\n";
	return $sd;
}
