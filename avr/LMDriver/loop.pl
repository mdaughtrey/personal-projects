#!/usr/bin/perl -w

use strict;
use IO::Handle;

open (THEDEV, '>/dev/ttyS0') or die "cant open port";
binmode(THEDEV);
THEDEV->autoflush(1);
my $fontIndex = 0;
while (1)
{
#    print THEDEV "l$fontIndex";
#    for my $count (hex 'f2'.. hex 'ff')
    for my $count (0 .. 255)
    {
	print THEDEV pack("CC", ord("a"), $count);
#	sleep 1;
	print THEDEV pack("CC", ord("A"), $count);
	sleep 1;
    }
#    $fontIndex = 1-$fontIndex;
}
close THEDEV;
