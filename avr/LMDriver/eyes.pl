#!/usr/bin/perl -w

use strict;
use FileHandle;
use Getopt::Std;

my $fifo;
my %option;

sub vOut
{
    exists $option{'v'} && print "@_\n"; 
}


sub pupilStraight
{
    print $fifo "p20P20
}

sub main
{
    $fifo = new FileHandle ">/tmp/usbdriver.fifo.in";
    $fifo->autoflush();
    print $fifo '*';
    $fifo->close();
}

getopt('m:v', \%option);

main;
