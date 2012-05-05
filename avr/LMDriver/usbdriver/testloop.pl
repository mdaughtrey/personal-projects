#!/usr/bin/perl -w

use strict;
$| = 1;
while (1)
  {
    for my $f ('a' .. 'z')
      {
	print  "a$f\n";
	sleep 1;
      }
  }
