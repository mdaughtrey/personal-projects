#!/usr/bin/perl -w

use strict;

my $count;

for ($count = 0; $count < 256; $count++)
{
    my $asBinary = pack("CC", ord("a"), $count);
    my $asHex = sprintf("%02x", $count);
    open (THEFILE, ">$asHex") or die "cant open file";
    print THEFILE $asBinary;
    close THEFILE;
}
