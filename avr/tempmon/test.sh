cat /var/local/tempmon_20090118.log | sed -e 's/,/ /g;' | perl -ne '@arr=split;print "$arr[0] $arr[1] "; print join(" ", map {$_/16} @arr[2..$#arr]); print "\n";'
