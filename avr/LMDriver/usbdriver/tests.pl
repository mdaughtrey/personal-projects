#!/usr/bin/perl -w

use strict;

use FileHandle;
use Getopt::Std;
use Data::Dumper;
use Time::HiRes;

my %option;
my $fifo;

sub vOut
{
    exists $option{'v'} && print "@_\n"; 
}

sub doCommand
{
    vOut @_;
    exists $option{'s'} || `@_`;
}
    
sub startServer
{
    my $server = `ps -eo comm  | grep [u]sbdriver`;
    if ($server) { return 0; }
    unless ($server)
    {
        unless (-e '/tmp/usbdriver.fifo.in')
        {
            vOut "Creating fifo";
            doCommand "mkfifo /tmp/usbdriver.fifo.in"; 
            doCommand "chmod 777 /tmp/usbdriver.fifo.in"; 
        }
        vOut "Starting server";
        doCommand 'xterm -e ./usbdriver -l -f /tmp/usbdriver.fifo &';
    }   
    return 1;
}

sub resetTx
{
    print $fifo "!0!1";
    Time::HiRes::usleep(500000);
}
    
sub testGPOutput
{
    for my $jj (0..15)
    {
        print $fifo 'g' . sprintf("%x", $jj);
        Time::HiRes::usleep(100000);
    }
    print $fifo "g0";
}

sub allCodes
{
    for my $ii (20..255)
    {
        print $fifo 'a' . chr($ii). 'A' . chr($ii);
        Time::HiRes::usleep(100000);
    }
}

sub testBlank
{
    print $fifo "aXAx";
    for my $ii (0 .. 10)
    {
        print $fifo "b0b1";
        Time::HiRes::usleep(100000);
        print $fifo "B0B1";
        Time::HiRes::usleep(100000);
    }
    print $fifo "B0B1";
    
}

sub testInvert
{
    resetTx();
    print $fifo "axA4";
    for my $ii (0 .. 10)
    {
        print $fifo "i0i1";
        Time::HiRes::usleep(100000);
        print $fifo "I0I1";
        Time::HiRes::usleep(100000);
    }
}

sub testFlip
{
    resetTx();
    print $fifo "ayA2";
    for my $ii (0 .. 5)
    {
        print $fifo "f0f1";
        Time::HiRes::usleep(500000);
        print $fifo "F0F1";
        Time::HiRes::usleep(500000);
    }
}

sub testMirror
{
    resetTx();
    print $fifo "ahA2";
    for my $ii (0 .. 5)
    {
        print $fifo "m0m1";
        Time::HiRes::usleep(500000);
        print $fifo "M0M1";
        Time::HiRes::usleep(500000);
    }
}

sub testRollUpDown
{
    resetTx();
    print $fifo "aaA2";
    for my $ii (0..5)
    {
        for my $jj (0..7)
        {
            print $fifo "r0d${jj}r1u${jj}";
            Time::HiRes::usleep(100000);
        }
    } 
}

sub testRollLeftRight
{
    resetTx();
    print $fifo "aaA1";
    for my $ii (0..5)
    {
        for my $jj (0..5)
        {
            print $fifo "r0l${jj}r1r${jj}";
            Time::HiRes::usleep(100000);
        }
    } 
}

sub testShiftUpDown
{
    resetTx();
    print $fifo "aaA1";
    for my $ii (0..5)
    {
        for my $jj (0..7)
        {
            print $fifo "s0d${jj}s1u${jj}";
            Time::HiRes::usleep(100000);
        }
    } 
}

sub testShiftLeftRight
{
    resetTx();
    print $fifo "aaA1";
    for my $ii (0..5)
    {
        for my $jj (0..5)
        {
            print $fifo "s0l${jj}s1r${jj}";
            Time::HiRes::usleep(100000);
        }
    } 
}

sub testFlashColors
{
    resetTx();
    print $fifo "a0AH";
    for my $ii (0..1)
    {
        print $fifo "l00L00";
        sleep 1;
        print $fifo "l01L01";
        sleep 1;
    } 
    for my $ii (0..15)
    {
        print $fifo "l00L00";
#        Time::HiRes::usleep(100000);
        print $fifo "l01L01";
#        Time::HiRes::usleep(100000);
    } 
}

sub testSpiral
{
    print $fifo "p20P20";
    print $fifo "l00L01c0c1";
    print $fifo "t000t001t002t003t004t100t101t102t103t104";
    print $fifo "t114t124t134t144t154t164";
    print $fifo "t163t162t161t160t064t063t062t061t060";
    print $fifo "t050t040t030t020t010t011t012t013t014t110t111t112t113";
    print $fifo "t123t133t143t153";
    print $fifo "t152t151t150t054t053t052t051";
    print $fifo "t041t031t021t022t023t024t120t121t122";
    print $fifo "t132t142";
    print $fifo "t141t140t044t043t042";
    print $fifo "t032t033t034t130t131";
    print $fifo "C0C1";
}

sub testProgColumns
{
    print $fifo "p20P20c0c1l01L01";
    print $fifo "05512a25532a455";
    print $fifo "57f67077f87097f";
    for (0..5)
    {
        print $fifo "i0i1";
        Time::HiRes::usleep(400000);
        print $fifo "I0I1";
        Time::HiRes::usleep(400000);
    }
    print $fifo "C0C1";
}

sub main
{
    if ($option{'x'})
    {
        return startServer();
    }
    $fifo = new FileHandle ">/tmp/usbdriver.fifo.in";
    $fifo->autoflush();
    print $fifo '*';
    sleep 1;
#    while (1)
# //   {
        testGPOutput();
        allCodes();
        testBlank();
        testInvert();
        testFlip();
        testMirror();
        testRollUpDown();
        testShiftUpDown();
        testRollLeftRight();
        testShiftLeftRight();
        testFlashColors();
        testSpiral();
        testProgColumns();
        print $fifo "a:A)";
#//   }
    $fifo->close();
}

getopts('xsv', \%option);
main();
