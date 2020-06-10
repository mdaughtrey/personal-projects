#!/usr/bin/perl -w

use strict;
use IO::Handle;

open (THEDEV, '>/dev/ttyS14') or die "cant open port";
binmode(THEDEV);
THEDEV->autoflush(1);

print THEDEV pack("CC", ord("a"), hex $ARGV[0]);
#	sleep 1;
print THEDEV pack("CC", ord("A"), hex $ARGV[0]);
sleep 1;

close THEDEV;
