#!/usr/bin/perl -w

use strict;

while (my $file = <@ARGV>)
{
  my $INFILE;
  open($INFILE, $file) or die "Cannot open $file";
  my @window;
  my $lineCount = 0;
  while (my $line = <$INFILE>)
    {
      $lineCount++;
      chomp $line;
      next unless length $line;
      $line =~ s/[\t\r\n]//g;
      $line =~ s/ +/ /;
      if ($line =~ /^<\/div>/) { next; }
      if ($line =~ /^<\/td>/) { next; }
      if ($line =~ /^<td/) { next; }
      if ($line =~ /^<div/) { next; }
      if ($line =~ /claimid=(\d+)/) { $line = $1; }
      
      push @window, $line;
      if (scalar @window == 8)
	{	
	  if ($window[1] =~ /JOSEPH MARQUEZ/)
	    {
	      for (0 .. 7)
		{
		  print $window[$_],'*';
		}
	      print "\n";
	      @window = ();
	    }	
	  else
	    {
	      shift @window;
	    }
	}
    }	
  close $INFILE;
}
