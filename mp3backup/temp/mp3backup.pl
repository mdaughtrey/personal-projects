#!/usr/bin/perl -w 

use strict;

my $TAPE_THRESHOLD = 19.5 * 1e6; # 19.5 GB in KB
my $sofar = 0;

#my @fileList = `du --max-depth=1 /var/mp3/mp3  | sort -n`;
my @fileList = `du --max-depth=1 .  | sort -n`;
my @fileSet;

sub writeFileSet
{
    my $fileSet = shift;
    my $time = time;
    my $tempFilename = "/var/tmp/mp3backup-$time.tar.cmd";
    my $command = "tar cv --files-from ${tempFilename} --to-stdout > /dev/tape 2>&1";
    print "Insert tape: ";
    print "\nrewinding...";
    if (system('mt rewind') != 0)
    {
	die "Error in rewinding\n";
    }
    open(TEMPFILE, ">$tempFilename") or die "Cannot open $tempFilename\n";
    for my $file (@{$fileSet})
    {
	$file =~ /\d+[\t ]+(.*)$/;
	$file = $1;
	chomp $file;
#	$file =~ s:([<>\'\"]):\\$1:;
#	print TEMPFILE "\'$file\'\n";
	print TEMPFILE "$file\n";
    }
    close TEMPFILE;
    my @result = `$command`;
    open (RESULTFILE, ">/var/tmp/mp3backup-$time.log");
    print RESULTFILE @result;
    close RESULTFILE;
	  
}

sub main
{
    my $index = shift;
    my $total = pop @fileList;
    $total =~ /(\d+)/;

    print "$1 bytes, ", int($1/$TAPE_THRESHOLD) + 1, " tapes needed.\n";
    for my $entry (@fileList)
    {
	push @fileSet, $entry;
	$entry =~ /(\d+)/;
	$sofar += $1;
	if ($sofar > $TAPE_THRESHOLD)
	{
	    if ($index == 0)
	    {
		writeFileSet(\@fileSet);
		return;
	    }
	    $index--;
	    $sofar = 0;
	    @fileSet = ();
	}
    }
    writeFileSet(\@fileSet);
}
sub usage
{
    print "mp3backup.pl [index]\n";
    die;
}

scalar @ARGV || die usage;

main ($ARGV[0]);
