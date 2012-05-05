#file last updated with version 4.0

sub printpage($){
	my $content = $_[0];
	my $template = $_[1];
	my $page;
	open(TEMPLATE,"<$template")||dienice("could not find the template file template.html to use!");
	while(<TEMPLATE>){
		$page .= $_;
	}
	close(TEMPLATE);
	$page =~ s/<!-- ?CONTENT ?-->/$content<br><font color='#000000' size=-1>
Powered by <a color='#008800' href="http:\/\/www.fuzzymonkey.org\/newfuzzy\/software\/perl\/" target=\"new\">My Photo Gallery $version<\/a>.  Licensed under <a href=\"http:\/\/www.fsf.org\/copyleft\/gpl.html\" target=\"newwindow\">GPL<\/a>. &#169; 2000-2003 Fuzzymonkey.org.<\/font><br>/i;
	print "Content-type: text/html\n\n$page";
}
sub dienice {
	my($msg) = @_;
	print "Content-type:text/html\n\n";
	print <<HTML;
<html>
<body>
	<br>
<center><table width="400">
	<tr bgcolor="#FC3C3C"><td align="middle"><b><big>!! Oops !!</big></b></td></tr>
	<tr bgcolor="lightgrey"><td>$msg
    <br><br><br>
    If you continue to have problems, email either the moderator of this
    site or mike\@fuzzymonkey.org.  If this is a setup issue with this script,
    you may want to look at the FAQ or Intro to CGI located at
    http://www.fuzzymonkey.org/perl/ for tips before
    emailing.</td></tr>
</table></center>
</body>
</html>
HTML
	exit;
}

sub prettytime {
	#converts the first argument as the number of seconds since Jan1,1970 into a readable string
	# and returns it.
	@days = ("Sun","Mon","Tues","Wed","Thur","Fri","Sat","Sun");
	@months = ("Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec");
	($sec,$min,$hr,$mday,$mon,$year,$wday,$yday,$isdst) = localtime($_[0]);
	$longyr = $year + 1900;
	$fixmo = $mon + 1;
	
	if($hr > 12)  {
		$hr = $hr - 12;
		$ampm = "PM";
	} else {
		$ampm = "AM";
	}
	
	$yr2 = substr($longyr,2,2);
	
	$time = sprintf("%02s %02d, %04d",
                          $months[$mon],$mday,$longyr);
	return $time;
	
}

sub checkdirectories($) {
	my $return;
	my $selectedalbum = $_[0];
	#make thumbnails directory if it doesn't exist
	unless(-e "$dataroot/$selectedalbum/thumbnails"){
        #print "Making directory $photoroot/$selectedalbum/thumbnails ...<br><br>";
		mkdir("$dataroot/$selectedalbum/thumbnails",0755) || dienice("Error, could not make thumbnails directory
			please check the permissions!<br><br>");
	}
	
	#make descriptions directory if it doesn't exist
	unless(-e "$dataroot/$selectedalbum/descriptions"){
        #print "Making directory $photoroot/$selectedalbum/descriptions...<br><br>";
		mkdir("$dataroot/$selectedalbum/descriptions",0755) || dienice("Error, could not make descriptions directory
			please check the permissions!<br><br>");
	}
	return $return;
}


#########################################################################
# Prints out a trail to the selected album with hyperlinks to view them #
# Optional second parameter is a string of extra CGI parameter/value    #
# pairs to append to the album's url.                                   #
#########################################################################
sub print_location ($) {
    my $return;
	 my @steps;
	 my $step;
	 my $cgistep;
	 my $sofar='';
	 
   $return .= "$title: <a href=\"index.cgi\">main</a> ";
	@steps = split (/\//,"$_[0]");
    $i = 0;
	foreach $step (@steps){
		if($i){$sofar .= "/$step";}else{$sofar .= "$step";}
			$step = makepretty($step);
      	$url = "index.cgi?mode=view&album=$sofar";
      	$url .= "\&$_[1]" if ($i == $#steps && $_[1]);
			$return .="<a href=\"$url\">$step</a> &gt ";
			$i++;
	}
    $return .= "<br><br>\n";
    return $return;
}

sub fileinfo {
	($dev,$ino,$mode,$nlink,$uid,$guid,$rdev,$filesize,$atime,$mtime,$ctime,$blksize,$blocks) = stat $_[0];
	if($filesize > 1024*1024){
		$kfilesize = sprintf("%0.1f M",$filesize/1024/1024);
	}else{
		$kfilesize = sprintf("%d K",$filesize/1024);
	}
	$time = prettytime($mtime);
}


########################################################################################
##  Sends an email using 5 variables $subject, $to, $from, $etitle, $body in @_#########
########################################################################################
sub email ($$$$$) 
{ 
    my ($subject, $to, $from, $etitle, $body) = @_;

    open (MAIL, "| $sendmail -i -t" ) || dienice("could not open mail");
    print MAIL <<MAIL_MESSAGE;
Subject:$subject
To:$to
Reply-to:$from
From:$etitle

$body
MAIL_MESSAGE
     close MAIL;
}

sub hacking_attempt($){
	my $dns = `host $ENV{'REMOTE_ADDR'}`;
	my $time = localtime(time());
	if($dns =~ /not found/i){$dns="";}else{$dns="($dns)";}
	my $info = "
IP address: $ENV{'REMOTE_ADDR'} $dns
Browser: $ENV{'HTTP_USER_AGENT'}
Remote Port: $ENV{'REMOTE_PORT'}
Server Port: $ENV{'SERVER_PORT'}
Script: $ENV{'SCRIPT_NAME'}
Query: $ENV{'QUERY_STRING'}
Time: $time
";
	if($email_comments_to_admin){email("hack attempted on photo gallery",$admin_email,$admin_email,$admin_email,"Hack attempted!\n\n$info");}
	$info =~ s/\n/<br>/g;
	dienice("$_[0] $info");
}


return 1;
