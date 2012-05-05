#!/usr/bin/perl -w

use strict;

use Net::Telnet;
use FileHandle;

my $telnet = new Net::Telnet();
$telnet->open("192.168.1.96");
my ($sec,$min,$hour,$mday,$mon,$year) = localtime;
$mon++;
$year += 1900;
my $file = new FileHandle(">>/var/local/tempmon_" . 
                sprintf("%04u%02u%02u", $year,$mon,$mday) .
                ".log");

print $file sprintf("%04u/%02u/%02u,%02u:%02u:%02u,%s",
    $year,$mon,$mday,$hour,$min,$sec,$telnet->getline());
$file->close();

