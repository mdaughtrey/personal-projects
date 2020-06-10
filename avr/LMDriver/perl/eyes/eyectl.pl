#!/usr/bin/perl -w

use strict;
push @INC, '.';
use Eyes;

my $left = Eyes->new(0);
my $right = Eyes->new(1);

sub blink
{
  for my $routine ($left->blinkSeries())
    {
      &$routine($left);
      &$routine($right);
    }
}


sub lookLeft
{
  for my $routine ($left->lookLeftSeries())
    {
      &$routine($left);
      &$routine($right);
    }
}

sub lookRight
{
  for my $routine ($left->lookRightSeries())
    {
      &$routine($left);
      &$routine($right);
    }
}

sub lookUp
{
  for my $routine ($left->lookUpSeries())
    {
      &$routine($left);
      &$routine($right);
    }
}

sub lookDown
{
  for my $routine ($left->lookDownSeries())
    {
      &$routine($left);
      &$routine($right);
    }
}

sub returnToCenter
{
  my (@series) = @_;
  for my $routine (reverse @series)
    {
      &$routine($left);
      &$routine($right);
    }
  }

sub rage
  {
    
    $left->evilEye();
    $right->evilEye();

    for my $ii (0..7)
      {
	$left->color($ii);
	$right->color($ii);
	for my $jj (0..10)
	  {
	    $left->blank($jj % 2);
	    $right->blank( $jj % 2)
	  }
      }

#    $left->invert(1);
#    $right->invert(1);
#    for my $ii (0..7)
#      {
#	$left->color($ii);
#	$right->color($ii);
#	for my $jj (0..10)
#	  {
#	    $left->blank( $jj % 2);
#	    $right->blank( $jj % 2)
#	  }
#      }
#    $left->invert(0);
#    $right->invert(0);

  }

sub main
  {
    while (1)
      {
	$left->color(0);
	$right->color(0);
	$left->baseExpression();
	$right->baseExpression();
	sleep 2;
	blink;
	sleep 2;
	lookLeft;
	sleep 2;
	returnToCenter($left->lookLeftSeries());
	sleep 2;
	lookRight;
	sleep 2;
	returnToCenter($left->lookRightSeries());
	sleep 2;
	blink;
	sleep 2;
	$left->frown();
	sleep 2;
	$right->frown();
	sleep 2;
#	lookUp;
#	sleep 2;
#	returnToCenter($left->lookUpSeries());
#	sleep 2;
#	lookDown;
#	sleep 2;
#	returnToCenter($left->lookDownSeries());
#	sleep 2;
	rage;
	sleep 2;
      }
  }

main;
