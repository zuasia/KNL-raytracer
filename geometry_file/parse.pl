#!/lusr/bin/perl

use strict;
use warnings;

my $filename;
my $readfile;
my $writefile;

$filename = "teapot.ply";
open($readfile, '<', $filename) or die "Could not open file '$filename' $!";

$filename = "teapot.ray";
open($writefile, '>', $filename) or die "Could not open file '$filename' $!";
 
my $max1;
my $max2;
my $max3;

$max1=0;
$max2=0;
$max3=0;

print $writefile "points=( ";
while (my $row = <$readfile>) {
  chomp $row;

  my @values = split(' ', $row);
  my $count;
  $count = scalar @values;

  if($count == 8) {

	  if ($values[0]>$max1) {
		  $max1=$values[0];
	  }
	  if ($values[1]>$max2) {
		  $max2=$values[1];
	  }
	  if ($values[2]>$max3) {
		  $max3=$values[2];
	  }

	  my $scale;
	  $scale = 0.01;
	  @values = map { $_ * $scale } @values;

	  print $writefile "( $values[0],$values[1],$values[2] ),";
  }
}
print $writefile " );\n";

print "$max1\t$max2\t$max3\n";
close $readfile;

$filename = "teapot.ply";
open($readfile, '<', $filename) or die "Could not open file '$filename' $!";

print $writefile "faces=( ";
while (my $row = <$readfile>) {
  chomp $row;

  my @values = split(' ', $row);
  my $count;
  $count = scalar @values;

  if($count == 4) {
	  print $writefile "( $values[1],$values[2],$values[3] ),";
  }
}
print $writefile " );\n";

$filename = "teapot.ply";
open($readfile, '<', $filename) or die "Could not open file '$filename' $!";

print $writefile "map_points=( ";
while (my $row = <$readfile>) {
  chomp $row;

  my @values = split(' ', $row);
  my $count;
  $count = scalar @values;

  if($count == 8) {
	  my $u;
	  my $v;
	  $u = $values[6]/2.0;
	  $v = ($values[7]+1.0)/2.0;
	  print $writefile "( $u,$v ),";
  }
}
print $writefile " );\n";


close $writefile;
