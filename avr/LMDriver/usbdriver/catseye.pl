#!/usr/bin/perl -w

use strict;

use FileHandle;
use Getopt::Std;
use Data::Dumper;
use Time::HiRes;

my %option;
my $fifo;

sub lookStraightAhead
{
#                  .  .  .  .  .  .  .  .  .  .
    print $fifo "g001c91c12282224174134145d55d641c0c1a-A-";
}

sub blink
{ 
#                .  .  .  .  .  .  .  .  .  .
    print $fifo '00890811481422272232262243e53e';
    print $fifo '10880821471431461441c51c';
    print $fifo '208308408508608708';
    print $fifo '10880821471431461441c51c';
    print $fifo '11481422272232262243e53e';
    print $fifo '01c91c12282224174134145d55d641';
}

sub fastBlink
{
    print $fifo "C0C1";
    Time::HiRes::usleep(100000);
    print $fifo "c0c1";
}

sub glanceRight
{
    print $fifo "35d541";
    Time::HiRes::usleep(100000);
    print $fifo "25d441";
    sleep 1;
    print $fifo "45d241";
    Time::HiRes::usleep(200000);
    print $fifo "55d341";
    Time::HiRes::usleep(200000);
}

sub glanceLeft
{
    print $fifo "65d441";
    Time::HiRes::usleep(100000);
    print $fifo "75d541";
    sleep 1;
    print $fifo "55d741";
    Time::HiRes::usleep(200000);
    print $fifo "45d641";
    Time::HiRes::usleep(200000);
}

sub pupilStraight
{
    print $fifo '341459559641';
}

sub pupilRight
{
    print $fifo '359541';
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
    lookStraightAhead();
    while (1)
    {
        sleep 4;
        glanceRight();
        sleep 3;
        glanceLeft();
        sleep 2;
        fastBlink();
    }
    $fifo->close();
}

getopts('xsv', \%option);
main();
