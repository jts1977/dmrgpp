#!/usr/bin/perl -w
# USAGE IS
# perl thisScript.pl < oldInput.jsn > newInput.jsn
#
#
use strict;

use lib "JSON-2.53/lib";
use JSON;


my $json = new JSON;

my $content = "{}";

my $json_text = $json->allow_nonref->utf8->relaxed->escape_slash->loose->allow_singlequote->allow_barekey->decode($content);

my %keyValue;

my ($file)=@ARGV;
#reading part:
readKeyValue($file);


###### writing part:
foreach my $key (keys %keyValue) {
	my $parent = findParent($key);
	my $x = $keyValue{$key}; 
	$x =~ s/^ +//;
	$x =~ s/ +$//;
	$x += 0 if (isANumber($x));
	$json_text->{programSpecific}->{DMRGPP}->{$parent}->{$key} = $x;
}


my $final  = $json->pretty->encode($json_text);

print "$final\n";

sub isANumber
{
	my ($x)=@_;
	return 1 if ($x=~/^[\d\.\d\-\+]+$/);
	return 0;
}


sub readKeyValue
{
	my ($file)=@_;
	my $buffer="";
	open(FILE,$file) or die "Cannot open file $file: $!\n";
	while(<FILE>) {
		if (/(^[^=]+)=(.*)$/) {
			$keyValue{$1}=$2 unless (/^density/ ||/^TSPFilename/);
			next;	
		}
	
		if (/^JMVALUES/) {
			my @jmvalues = split;
			die "JMVALUES needs exactly 2 values\n" if ($#jmvalues!=2);
			my @vv = ($jmvalues[1]+0,$jmvalues[2]+0);
			$keyValue{"JMVALUES"}=\@vv;
			next;
		}

		if (/^RAW_MATRIX/) {
			$_ = <FILE>;
			die "Problem reading RAWMATRIX\n" if (!defined($_));
			my @colAndRow = split;
			my @vV;
			my $numberOfRows = $colAndRow[0] + 0;
			for (my $i=0;$i<$numberOfRows;$i++) {
				$_ = <FILE>;
				die "Problem reading RAWMATRIX\n" if (!defined($_));
				my @matrixTemp = split;
				my @v2;
				numberize(\@v2,\@matrixTemp);
				$vV[$i] = \@v2;
			}
			$keyValue{"RAW_MATRIX"}=\@vV;
			next;
		}

		s/Connectors[ \s]/Connectors0 /;
		s/ConnectorsX/Connectors0/;
		s/ConnectorsY/Connectors1/;

		$buffer = $buffer.$_." ";
		$buffer =~ s/^[\t \n]+//;
		my @temp = split/[\t \n]+/,$buffer;
		my $n = $#temp + 1;
		
		next if ($n<2) ;	
		
		my $expected = $temp[1];
		$expected *= 3 if ($buffer =~ /^FiniteLoops/);
		my $want = $expected +2;
		next if ($n<$want) ;
		$buffer = "";
		my @v;
		for (my $i=2;$i<$#temp+1;$i++) {
			my $x = $temp[$i];
			$x += 0 if (isANumber($x));
			$v[$i-2] = $x;
		}
		$keyValue{$temp[0]}=\@v;
	}
	close(FILE);
}

sub findParent
{
	my ($key)=@_;
	return "Geometry" if (
		$key eq "TotalNumberOfSites" 
		or
		$key eq "NumberOfTerms"
		or
		$key eq "DegreesOfFreedom"
		or
		$key eq "GeometryKind"
		or
		$key eq "GeometryOptions"
		or
		$key=~/Connectors.?/
	);

	return "Model" if (
		$key eq "hubbardU" 
		or
		$key eq "potentialV"
	);

	return "Solver" if (
		$key eq "SolverOptions" 
		or
		$key eq "Version"
		or
		$key eq "OutputFile"
		or
		$key eq "InfiniteLoopKeptStates"
		or
		$key eq "FiniteLoops"
		or
		$key eq "TargetQuantumNumbers" 
	);
	return "Concurrency" if (
		$key eq "Threads"
	);
	return "Dynamic";
}

sub numberize
{
	my ($dest,$src) = @_;
	my $n = scalar(@$src);
	for (my $i=0;$i<$n;$i++) {
		my @vx = ($src->[$i] + 0,0);
		$dest->[$i] = \@vx;
	}
}
