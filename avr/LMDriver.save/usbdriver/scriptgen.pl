#!/usr/bin/perl -w

use strict;

my %commandMap = (
		  spiral0 => sub {
		    print "c0";
		    print "*t000*t001*t002*t003*t004";
		    print "*t014*t024*t034*t044*t054*t064";
		  },
		  test1 => sub {
		    print "c0";
		    print "t044t054";
#		    print "*t014*t024*t034*t044*t054*t064";
		  },
		  spiralOnOff => sub {
		    print "c0c1l00L00";
#		    print "c0c1";
		    print "*t000*t001*t002*t003*t004*t100*t101*t102*t103*t104";
		    print "*t114*t124*t134*t144*t154*t164";
		    print "*t163*t162*t161*t160*t064*t063*t062*t061*t060";
		    print "*t050*t040*t030*t020*t010";
		    print "*t011*t012*t013*t014*t110*t111*t112*t113*t114";
		    print "*t123*t133*t143*t153";
		    print "*t152*t151*t150*t054*t053*t052*t051";
		    print "*t041*t031*t021";
		    print "*t022*t023*t024*t120*t121*t122*t123";
		    print "*t133*t143";
		    print "*t142*t141*t140*t044*t043*t042";
		    print "*t032*t033*t034*t130*t131*t132";
#		    print "l01L01";
		    print "*T000*T001*T002*T003*T004*T100*T101*T102*T103*T104";
		    print "*T114*T124*T134*T144*T154*T164";
		    print "*T163*T162*T161*T160*T064*T063*T062*T061*T060";
		    print "*T050*T040*T030*T020*T010";
		    print "*T011*T012*T013*T014*T110*T111*T112*T113*T114";
		    print "*T123*T133*T143*T153";
		    print "*T152*T151*T150*T054*T053*T052*T051";
		    print "*T041*T031*T021";
		    print "*T022*T023*T024*T120*T121*T122*T123";
		    print "*T133*T143";
		    print "*T142*T141*T140*T044*T043*T042";
		    print "*T032*T033*T034*T130*T131*T132";
		  },
		  blockFlash =>  sub  {
		    print "a A i0i1I0I1";
		  },
		  
		  twoLines => sub  {
		    print "c0c1";
		    print "041141241341441541641741841941";
		    print "022122222322422522622722822922";
		    print "014114214314414514614714814914";
		    print "008108208308408508608708808908";
		    print "014114214314414514614714814914";
		    print "022122222322422522622722822922";
		  },

		  alphaLC => sub  {
		    for my $xx ('a'..'z')
		      {
			print "a$xx";
		      }
		  },
		  palettecycle => sub {
		    print "a A ";
		    for my $ii (0 .. 7)
		      {
			print "*l0${ii}L0${ii}";
		      }
		    for my $jj (7,6,5,4,3,2,1)
		      {
			print "*l0${jj}L0${jj}";
		      }
		  }

);

sub main
  {
    $commandMap{$ARGV[0]}();
  }

main;
