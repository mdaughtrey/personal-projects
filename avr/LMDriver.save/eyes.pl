#!/usr/bin/perl -w
use strict;
use IO::Handle;

sub eyeSend
{
    open (THEDEV, '>/dev/ttyS14') or die "cant open port";
    binmode(THEDEV);
    THEDEV->autoflush(1);
    for my $data (@_)
    {
	print THEDEV @$data;
    }
    close THEDEV;
}

sub drawFrames
{
    my ($left, $right) = @_;
    push @$left, 'aO';
    push @$right, 'AO';
#    push @$left, 'a', pack("C", 0xfc);
#    push @$right, 'A', pack("C", 0xfc);
}
sub pinPoint
{
    my ($left, $right) = @_;
    push @$left, 't032';
    push @$right, 't132';
}


sub animateEyePosition
{
    my ($left, $right, @posCodes) = @_;
     
    for my $posCode (@posCodes)
    {
	(@$left, @$right) = ('!0a ', '!1A ');
	drawFrames($left, $right);
	push @$left, 't0', $posCode;
	push @$right, 't1', $posCode;
	eyeSend($left, $right);
	sleep(1);
    }
}

sub animateLeft
{
   my @posCodes = ('32', '31');
   animateEyePosition(@_, @posCodes);
}

sub animateRight
{
   my @posCodes = ('32', '33');
   animateEyePosition(@_, @posCodes);
}

sub animateUp
{
   my @posCodes = ('32', '22', '12');
   animateEyePosition(@_, @posCodes);
}

sub animateDown
{
   my @posCodes = ('32', '42', '52');
   animateEyePosition(@_, @posCodes);
}


sub main
{
    my (@left, @right) = ('!0!a ', '!1A ');

    while (1)
    {
	animateLeft(\@left, \@right);
	animateRight(\@left, \@right);
	animateUp(\@left, \@right);
	animateDown(\@left, \@right);
    }
}

main;
