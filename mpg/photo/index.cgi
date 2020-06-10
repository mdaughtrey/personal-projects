#!/usr/bin/perl

####################################################################
# My Photo Gallery Script
# Version shown in the variable set below 
# 
# This script is part of the FuzzyMonkey.org Script Archive at
# www.fuzzymonkey.org and is released under the GPL license.  Please
# see the included LICENSE.txt file for details.
#
####################################################################

#if you had to install Image Magick as NON-root, please add the
# use lib line
#use lib "/home/username/local/lib/site_perl/5.6.0/i686-linux/";
#use lib "/home/username/local/lib/site_perl/5.6.0/";


use CGI(-debug);
use CGI::Carp qw(fatalsToBrowser);
use Image::Magick;
require "./sitevariables.pl";
require "./common.pl";
require "./imagemagick.pl";
if($exif=~ /yes/i){
    use Image::Info qw(image_info);
}
srand();
############ BEGIN MAIN PROGRAM ################################
$version = "4.03";

$cur = CGI->new();
$content = "";
$page = 1;
$mode = "default";
unless($resize_quality){$resize_quality="90";}
unless($thumbnails_per_page){$thumbnails_per_page ="100";}
if(!$directory_selection){$directory_selection = 'directory.gif';}

#get cookies
@cookies=split(/;/,$ENV{'HTTP_COOKIE'});
foreach $cookie (@cookies) {
	($name,$value) = split (/=/,$cookie);
	$crumbs{$name}=$value;
}


if($cur->param("maxWidth")) {
	$maxWidth= $cur->param("maxWidth");
	print "Set-Cookie: maxWidth=$maxWidth\n"; #set a cookie for the maxWidth preference
} else {
	$maxWidth = 640;

	#check for cookie
	if($cur->cookie('maxWidth')){$maxWidth=$cur->cookie('maxWidth');}
}

if($cur->param("mode")) {
	$mode = $cur->param("mode");
}
if($cur->param("album")) {
	$selectedalbum = $cur->param("album");
	$htmlalbum = $selectedalbum;
	$htmlalbum =~ s/ /%20/g;
	if(-e "$photoroot/$selectedalbum/lock"){
		if($cur->cookie('username') || $cur->param("username")){
			$found = "no";
			$username_cookie = $cur->cookie('username');
			$password_cookie = $cur->cookie('password');
			$username_form = $cur->param("username");
			$password_form = $cur->param("password");
			open(LOCKFILE,"<$photoroot/$selectedalbum/lock");
			while(<LOCKFILE>){
				chomp;
				next unless $_;
				($username_file,$password_file) = split(/=/,$_);
				if(($username_cookie eq $username_file)&&($password_cookie eq $password_file)){
					$found = "yes";
				}
				if(($username_form eq $username_file)&&($password_form eq $password_file)){
					$found = "yes";
				}
			}
			close(LOCKFILE);
			unless($found eq "yes"){
				$mode = "askpassword";
			}
		}else{
			$mode = "askpassword";
		}
	}
}
if($cur->param("page")) {
   $page = $cur->param("page");
}
if($cur->param("picture")) {
	$selectedpicture= $cur->param("picture");
	$htmlpicture = $selectedpicture;
	$htmlpicture =~ s/ /%20/g;
	$htmlpicture =~ s/#/%23/g;
}
if($cur->param("movie")) {
	$selectedmovie = $cur->param("movie");
	$htmlmovie = $selectedmovie;
	$htmlmovie =~ s/ /%20/g;
}
if($cur->param("username")){
	$username = $cur->param("username");
	$password = $cur->param("password");
	print "Set-Cookie:username=$username\n";
	print "Set-Cookie:password=$password\n";
}
checkforbadthings();
if($mode eq "default"){
	$selectedalbum = '';
	$mode = "view";
}
if($mode eq "view"){
	$content .= check_data_directories($selectedalbum);
	$content .= checkdirectories($selectedalbum); #check for thumbnail and descriptions directories
	$content .= generatealbum("$selectedalbum"); 
}
if($mode eq "viewpicture"){
	if($cur->param("comment")){
		getcomment();
	}
	$content .= generatepicture($selectedalbum,$selectedpicture); 
}
if($mode eq "askpassword"){
	$content .= passwordform();
}

printpage($content,"template.html");
############ BEGIN SUBROUTINES #################################

################################################################
# takes the selected album and makes sure that we have all of
# the directories that we need up to that point
###############################################################
sub check_data_directories($){
	my $directory = $_[0];
	my $return;
	my @steps;
	my $step;
	my $sofar;

	@steps = split (/\//,"$directory");
	#$return .= "checking data directories for $directory<br>";
	foreach $step (@steps){
		$step =~ s/\///gi;
		if($sofar){
			$sofar = "$sofar/$step"
		}else{
			$sofar = "$dataroot/$step";
		}
		unless(-e "$sofar"){
			#$return .= "creating $sofar with 755 permissions<br>";
			mkdir("$sofar",0755)||dienice("Could not create data directory! $!");
		}
	}
	return $return;
}

sub generatealbum ($) {
    my $return;
    my $selectedalbum = $_[0];
    #sanity check
    #if(!$selectedalbum){
	    #dienice("No album was selected");
	    #$content .= "warning: no album was selected<br>";
    #}

    $return .= print_location($selectedalbum, pageParams());
    
    $_[0] =~ s/ /\\ /g; #fix spaces for globbing and file opening
    
    ###########################################
    #get the hits and stuff for this album
    ###########################################
	my $hits = logpicture("$selectedalbum/");
	my $hitstring = "<font size =-1 color=blue>album viewed <b>$hits</b> times</font>";

    ###########################################
	#check for description of this album 
    ###########################################
	my $description;
	if(open (INFILE, "$dataroot/$selectedalbum/descriptions/album.txt")){ 
		$description .= '';
		flock (INFILE, 2); #locks the file for exclusive access.
		while(<INFILE>){
			$description .= "$_";
		}
		flock (INFILE, 8); #unlocks the file
		close(INFILE);
	}
	unless($description){
		$description ="No album description.";
	}
	if($smiley =~ /yes/i){
		$description = make_faces($description);
	}
	$description =~ s/\n/<br>/g;
	$return .= "<center><table bgcolor=#000000 cellspacing=0 cellpadding=2 border=0>
			<tr><td align=middle>
			<table bgcolor=#e0e0e0 width=400 border=0 cellspacing=0 cellpadding=5>
				<tr>
					<td>$description
					<br>$hitstring</td>
				</tr>
			</table>
			</td></tr>
		</table></center>";


	my @files= glob "$photoroot/$_[0]/*"; #find a better way to do this
    my $count =0;

	# Hack to sort Directories first. -KWP 11/05/2003
    @files = sort {(-d $b) <=> (-d $a) || $a cmp $b} @files;

    # take only those files in the current page.  If the 1024, site-images, etc.
    # are in this directory, then they're included in the count even though they
    # are not displayed.
    my ($start, $end, $total_pages) = pageBounds(scalar(@files));
    @files = @files[$start..$end];
    $return .= "<p><div align=\"center\">\n";
    $return .= generatepagewidget(
        "index.cgi?mode=view\&album=$selectedalbum", 
        $total_pages,
        $page
    );
    $return .="</div>\n";
    

    $return .= "<p><table width=100% border=0 cellpadding=5 width=100%>";
    foreach $file (@files){
        if(-d $file){ # it is a directory
            unless (($file =~ /1024/)
                ||($file =~ /800/)
            	||($file =~ /640/)
            	||($file =~ /320/)
            	||($file =~ /site-images/)
            	||($file =~ /comments/)
            	||($file =~ /descriptions/)
            	||($file =~ /thumbnails/)){
            $file =~ s/.*\/(.*)/$1/g;
            my $cgihtmlalbum = $selectedalbum;
            $cgihtmlalbum =~ s/ /+/g;
            my $cgihtmlsubdirectory = $file;
            $cgihtmlsubdirectory =~ s/ /+/g;
	    my $prettysubdirectory = $file;
	    $prettysubdirectory = makepretty($prettysubdirectory);

	    $count++;
		 if(-e "$dataroot/$selectedalbum/$file/descriptions/directory.gif"){
		  	  $folder_picture = "$dataurl/$selectedalbum/$file/descriptions/directory.gif";
		 }else{
	    	if(-e "$photoroot/$selectedalbum/$file/lock"){
		  	  $folder_picture = "$dataurl/site-images/$directory_selection";
		  	  $folder_picture =~ s/(.*)\.(.*)/$1-locked.$2/;
	    	}else{
		  	  $folder_picture = "$dataurl/site-images/$directory_selection";
	    	}
		}
	    $return .= "<td align=middle><a href=\"index.cgi?mode=view&album=$cgihtmlalbum/$cgihtmlsubdirectory\"><img src=\"$folder_picture\" border=0></a><br><a href=\"index.cgi?mode=view&album=$cgihtmlalbum/$cgihtmlsubdirectory\"><font size=-1>$prettysubdirectory</font></a></td>";
            }
        }elsif($file =~ /\.(jpg|png|gif)\b/i){ #this is a supported image format (jpeg,png, or gif)
            fileinfo($file);
            $file =~ s/.*\/(.*)/$1/g;

            ################################################
            # lets check for a thumbnail ###################
            ################################################
            my $thumbnail = $file;
            #$thumbnail =~ s/(.*)\.(jpg|png|gif)\b/$1.jpg/i;
            unless(-e "$dataroot/$selectedalbum/thumbnails/$thumbnail"){
		resize("$photoroot/$selectedalbum/$file","$dataroot/$selectedalbum/thumbnails/$thumbnail",$previewWidth,$resize_quality);
		if($mag_on_thumbnails =~ /yes/i){
			overlay("$dataroot/$selectedalbum/thumbnails/$thumbnail","$dataroot/$selectedalbum/thumbnails/$thumbnail","$dataroot/site-images/mag.png");
		}
            }
            my $cgihtmlalbum = $selectedalbum;
            $cgihtmlalbum =~ s/ /+/g;
            my $cgihtmlpicture = $file;
            $cgihtmlpicture =~ s/ /+/g;
				$cgihtmlpicture =~ s/&/%26/g;
				

            my $prettypicture = makepretty($file);

            #fix spaces for old netscape and such
            $thumbnail =~ s/ /%20/g;
            $thumbnail =~ s/#/%23/g;

				my $comments_text = "";
				if((-e "$dataroot/$selectedalbum/comments/$file.txt") && ($indicate_comments =~ /yes/i)){
					$comments_text = "<img src=\"$dataurl/site-images/comments.gif\" alt=\"Comments Available\" title=\"Comments Available\">";
				}
            
            #NOTE to self: each line below has a <a> tag because the new
            #netscape was putting a funky line because of the line break
			$return .= "<td align=middle valign=center>
			<a href=\"index.cgi?album=$cgihtmlalbum&mode=viewpicture&picture=$cgihtmlpicture\">
			<img src=\"$dataurl/$htmlalbum/thumbnails/$thumbnail\" alt=\"$file\" border=0 title=\"click to view full size\"></a>
			<br><a href=\"index.cgi?album=$cgihtmlalbum&mode=viewpicture&picture=$cgihtmlpicture\"><font size=-1>$prettypicture</a></font>$comments_text
			<br><b></b><font color=green size=-2>$time</font></font></td>\n"; 
            $count++;
            #not to me....this line broken ... get extention and look it up to display icon
        }elsif( ($file =~ /.*(\..*)\b/i) && (defined $filetypes{"$1"})){
            fileinfo($file);
            my $ext = $1;
            $file =~ s/.*\/(.*)/$1/g;
            my $htmlfile = $file;
            $htmlfile =~ s/ /%20/g;
            $htmlfile =~ s/#/%23/g;
            $return .= "<td valign=center align=middle>
            <a href=\"$htmlroot/$htmlalbum/$htmlfile\"><img src=\"$dataurl/site-images/$filetypes{\"$ext\"}\" border=0></a><br>
            <a href=\"$htmlroot/$htmlalbum/$htmlfile\"><font size=-1>$file</font></a><br>
            <font size=-2 color=green>$time ($kfilesize)</font>
            </td>";
            $count++;
        }else{ #we don't know what kind of file this is
            fileinfo($file);
            $file =~ s/.*\/(.*)/$1/g;
	    unless($file eq "lock" || $file eq "Thumbs.db"){
            	my $htmlfile = $file;
            	$htmlfile =~ s/ /%20/g;
            	$htmlfile =~ s/#/%23/g;
            	$return .= "<td valign=center align=middle>
            	<a href=\"$htmlroot/$htmlalbum/$htmlfile\"><img src=\"$dataurl/site-images/unknown.jpg\" border=0></a><br>
            	<a href=\"$htmlroot/$htmlalbum/$file\"><font size=-1>$file</font></a><br>
            	<font size=-2 color=green>$time ($kfilesize)</font>
            	</td>";
            	$count++;
	    }
        }
        if($count >= $thumbnails_per_row){
            $count =0;
            $return .= "\n</tr><tr>";
        }
    }
    $return .= "</table>";

    $return .= "<p><div align=\"center\">\n";
    $return .= generatepagewidget(
        "index.cgi?mode=view\&album=$selectedalbum", 
        $total_pages,
        $page
    );
    $return .="</div>\n";
    
    return $return;
}



###########################################################
# Generates the CGI parameters needed to direct an album
# to the current page.
# by Todd Gamblin 5/29/2004
###########################################################
sub pageParams {
    $page = 1 unless $page;
    my $pageParams;
    $pageParams = "page=$page" unless ($thumbnails_per_page =~ /off/i);
    return $pageParams;
}

###########################################################
# Given a total number of files, returns a triple like so:
#
# ($startIndex, $endIndex, $totalPages)
#
# where start index is the first file to be on the page,
# end index is the last to be on the page, and total
# pages is the total number of pages in the album.
#
# by Todd Gamblin 5/29/2004
###########################################################
sub pageBounds {
    my ($total_files) = @_;
    my $last_file = $total_files - 1;

    return (0, $last_file, 1) if ($thumbnails_per_page =~ /off/i);
    
    my $start_file = ($page - 1) * $thumbnails_per_page;
    if ($start_file < 0 || $start_file > $last_file) {
        $start_file = 0;
        $page = 1;
    }
    
    my $end_file = $start_file + $thumbnails_per_page - 1;
    if ($end_file > $last_file) {
        $end_file = $last_file;
    }
    
    my $total_pages = int($total_files / $thumbnails_per_page);
    $total_pages++ if ($total_files % $thumbnails_per_page);    #take ceiling

    return ($start_file, $end_file, $total_pages);
}


###########################################################
# Given a base URL, the total number of pages, and the
# current page, generates a widget to move between pages
# and returns the html for it.
# by Todd Gamblin 5/29/2004
###########################################################
sub generatepagewidget {
    my ($baseurl, $total_pages, $current_page) = @_;
    
    return if ($total_pages == 1 || ($thumbnails_per_page =~ /off/i));

    my $return;
    $return .= "<table border=\"0\"><tr>\n";
    $return .= "<td align=\"left\">Page: </td>\n";

    if ($current_page > 1) {
        my $prev = $current_page - 1;
        $return .= "<td align=\"left\">";
        $return .= "<a href=\"$baseurl\&page=$prev\"><img boder=0 src=\"$dataurl/site-images/prev.gif\" border=\"0\" alt=\"<-\"></a>\n";
        $return .= "</td>";
    }

    $return .= "<td align=\"center\">";
    foreach $i (1..$total_pages) {
        if ($i == $current_page) {
            $return .= "$i";
        } else {
            $return .= "<a href=\"$baseurl\&page=$i\">$i</a>";
        }
        $return .="&nbsp;";
    }
    $return .= "</td>";

    if ($current_page < $total_pages) {
        my $next = $current_page + 1;
        $return .= "<td align=\"right\">";
        $return .= "<a href=\"$baseurl\&page=$next\"><img src=\"$dataurl/site-images/next.gif\" border=\"0\" alt=\"->\"></a>&nbsp;";
        $return .= "</td>";
    }

    $return .= "</tr></table>";

    return $return;
}


sub checkforbadthings {
	$selectedalbum =~ s/\\//g;
	$selectedpicture =~ s/\\//g;
	$selectedalbum =~ s/\*//g;
	$selectedpicture =~ s/\*//g;
	$selectedpicture =~ s/\(//g;
	$selectedpicture =~ s/\}//g;
	$selectedalbum =~ s/\(//g;
	$selectedalbum =~ s/\}//g;
	if($selectedalbum =~ /\.\./){
	    hacking_attempt("<p>Attempted hack detected!</p>
	    <p>Your malicious behavior has been caught and logged.  Depending
	    on where you live, there could be severe criminal penalties
	    associated with computer crimes.  The webmaster may use this
	    information to prosecute you to the full extent of the law.  In
	    short, don't do this anymore.  Even just poking around could
	    land you in serious trouble!</p>");
	}
	if($selectedpicture=~ /\.\./){
	    hacking_attempt("<p>Attempted hack detected!</p>
	    <p>Your malicious behavior has been caught and logged.  Depending
	    on where you live, there could be severe criminal penalties
	    associated with computer crimes.  The webmaster may use this
	    information to prosecute you to the full extent of the law.  In
	    short, don't do this anymore.  Even just poking around could
	    land you in serious trouble!</p>");
	}
	if($cur->param("q") =~ /\.\./){
		hacking_attempt("Illegal!  You should be ashamed. ");
	}

}

sub passwordform{
	my $return;

	$return .= "
	<form>
	<input type=hidden name=mode value=\"view\">
	<input type=hidden name=album value=\"$selectedalbum\">
	<table width=350 bgcolor=#000000 cellspacing=0 cellpadding=1 border=0><tr><td>
	<table width=100% bgcolor=#FFFFFF cellspacing=0 cellpadding=4 border=0>
		<tr bgcolor=#000000>
			<td colspan=2><font color=white><b>This gallery
			requires a username and passord.</b></font></td>
		</tr>
		<tr>
			<td>Username</td>
			<td><input type=\"text\" name=\"username\"></td>
		</tr>
		<tr>
			<td>Password</td>
			<td><input type=\"password\" name=\"password\"></td>
		</tr>
		<tr>
			<td colspan=2 align=middle><input type=\"submit\" value=\"Enter\"></td>
		</tr>
	</table>
	</td></tr></table>
	</form>
	";
	return $return;
}

# Extract the EXIF data from a picture
sub get_exifdata ($) {
	my $value;
	my $exifdata="";
	my $info = image_info("$_[0]");

	# make and model
	my $Make = $info->{'Make'};
	my $Model = $info->{'Model'};
	if ((defined($Make)) || (defined($Model))){
		$value = ($Model =~ /$Make/) ? "$Model" : "$Make $Model";
		$exifdata .= exifentry("Camera", $value);
	}

	# exposure time
	$value = '';
	if (defined($info->{'ExposureTime'}[1])){
		if (($info->{'ExposureTime'}[1] > $info->{'ExposureTime'}[0]) &&
		    ($info->{'ExposureTime'}[0] != 0)){
			$value = "1/" . sprintf("%.0f",
			         $info->{'ExposureTime'}[1] / $info->{'ExposureTime'}[0]);
		}elsif ($info->{'ExposureTime'}[1] != 0){
			$value = $info->{'ExposureTime'}[0] / $info->{'ExposureTime'}[1];
		}
	}elsif (defined($info->{'ShutterSpeedValue'}[1]) &&
	        ($info->{'ShutterSpeedValue'}[1] != 0)){
		$value = 2**($info->{'ShutterSpeedValue'}[0] / $info->{'ShutterSpeedValue'}[1]);
		if ($value < 1){
			$value = 1 / $value;
		}else{
			$value = "1/" . sprintf("%.0f", $value);
		}
	}
	if ($value){
		if ($value =~ /\..../){
			$value = sprintf("%.2f", $value);
		}
		$exifdata .= exifentry("Exposure", "$value sec.");
	}

	# aperture (f number)
	$value = '';
	if (defined($info->{'FNumber'}[1]) && ($info->{'FNumber'}[1] != 0)){
		$value = $info->{'FNumber'}[0] / $info->{'FNumber'}[1];
	}elsif (defined($info->{'ApertureValue'}[1]) &&
	        ($info->{'ApertureValue'}[1] != 0)){
		$value = sqrt(2)**($info->{'ApertureValue'}[0] / $info->{'ApertureValue'}[1]);
	}
	if ($value){
		if ($value =~ /\.../){
			$value = sprintf("%.1f", $value);
		}
		$exifdata .= exifentry("Aperture", "f/$value");
	}

	#focal length
	if (defined($info->{'FocalLength'}[1]) && ($info->{'FocalLength'}[1] != 0)) {
		$value = $info->{'FocalLength'}[0] / $info->{'FocalLength'}[1];
		if ($value =~ /\..../){
			$value = sprintf("%.2f", $value);
		}
		$exifdata .= exifentry("Focal&nbsp;length", $value . " mm");
	}

	# ISO speed
	if (defined($info->{'ISOSpeedRatings'})){
		$exifdata .= exifentry("ISO",$info->{'ISOSpeedRatings'});
	}

	# focus distance
	if (defined($info->{'SubjectDistance'}[1]) &&
	    ($info->{'SubjectDistance'}[1] != 0)){
		$value = $info->{'SubjectDistance'}[0] / $info->{'SubjectDistance'}[1];
		if ($value =~ /\...../){
			$value = sprintf("%.3f", $value);
		}
		$exifdata .= exifentry("Focus", $value . " m");
	}

	# flash
	if (defined($info->{'Flash'})) {
		my $value = (($info->{'Flash'} =~ /fired/i) ||
		             ($info->{'Flash'} =~ /yes/i) ||
		             ($info->{'Flash'} == 1)) ? "Yes" : "No";
		$exifdata .= exifentry("Flash", $value);
	}

	# date and time
	if (defined($info->{'DateTimeOriginal'}) &&
	    ($info->{'DateTimeOriginal'} ne '0000:00:00 00:00:00')){
		my $cam_date = $info->{'DateTimeOriginal'};
		my ($date,$time) = split(/ /,"$cam_date");
		my ($year,$month,$day) = split(/:/,"$date");
		my ($hours,$minutes,$seconds) = split(/:/,"$time");
		my @months = ("","Jan","Feb","Mar","Apr","May","Jun",
		                 "Jul","Aug","Sep","Oct","Nov","Dec");
		$month = $months[$month];
		if($hours > 11 && $hours != 0){
			$hours = $hours - 12;
			$ampm = "PM";
		}else{
			if($hours == 0){$hours = 12;}
			$ampm = "AM";
		}
		$exifdata .= exifentry("Date&nbsp;taken",
		               "$month $day, $year at $hours:$minutes:$seconds $ampm");
	}

	# comment
	if (defined($info->{'Comment'}) && ($info->{'Comment'} ne "")) {
		$exifdata .= exifentry("Comment", $info->{'Comment'});
	}
	if (defined($info->{'UserComment'}) && ($info->{'UserComment'} ne "")) {
		$usercomment = $info->{'UserComment'};
		$usercomment =~ s/ASCII//g;
		$usercomment =~ s/[^\w\s]//g;
		$exifdata .= exifentry("User&nbsp;comment", $usercomment);
	}


	if ($exifdata){
		$exifdata = "
	<table bgcolor=#000000 border=0 cellspacing=0 cellpadding=1><tr><td>
	<table bgcolor=$rowcolor border=0 cellspacing=0 cellpadding=2>
	<tr><th bgcolor=#000000><font color=#FFFFFF size=-2>Embedded EXIF data</font></th></tr>
	<tr><td>
		<table border=0 cellpadding=0 cellspacing=2>$exifdata
		</table>
	</td></tr>
	</table>
	</td></tr></table>";
	}

	return $exifdata;
}

# format an EXIF table entry
sub exifentry($$) {
	return "\n\t\t<tr><td valign=\"top\"><font size=-2><b>$_[0]:</b></font></td><td><font size=-2>$_[1]</font></td></tr>";
}


sub generatepicture {
	my $return;
	
	getPrevNextButtons($selectedalbum, $selectedpicture);

	#get width and height
	my($imageToPing) = Image::Magick->new;
	my($itemToDisplay) = "$photoroot/$selectedalbum/$selectedpicture";

	$x = $imageToPing->Read($itemToDisplay);
	warn $x if $x;
	($width,$height) = $imageToPing->Get('width','height');
	undef $imageToPing;

	#fixing spaces for cgi link
	my $cgihtmlalbum = $selectedalbum;
	$cgihtmlalbum =~ s/ /+/g;
	my $shutterflytext;
	if($allowshutterfly =~ /YES/i){
		my $thumbnail = $htmlpicture;
        $thumbnail =~ s/(.*)\.(jpg|png|gif)\b/$1.jpg/i;
		$shutterflytext = "
<form name=\"sflyc4p\" action=\"http://www.shutterfly.com/c4p/UpdateCart.jsp\" method=\"post\">
<input type=submit value=\"order this photo\">
<input type=hidden name=addim value=1>
<input type=hidden name=protocol value=\"SFP,100\">
<input type=hidden name=pid value=C4P>
<input type=hidden name=psid value=AFFL>
<input type=hidden name=referid value=mikespice>
<input type=hidden name=returl value=\"$ENV{'HTTP_REFERER'}\">
<input type=hidden name=imraw-1 value=\"$htmlroot/$htmlalbum/$htmlpicture\">
<input type=hidden name=imrawheight-1 value=\"$height\">
<input type=hidden name=imrawwidth-1 value=\"$width\">
<input type=hidden name=imthumb-1 value=\"$dataurl/$htmlalbum/thumbnails/$thumbnail\">
<input type=hidden name=imbkprntb-1 value=\"provided by a FREE script from www.fuzzymonkey.org\">
</form> from shutterfly.com";
		$shutterflytext =~ s/\n//g;
	}	

	my $clubphototext;
	if($allowclubphoto =~ /YES/i){
		my $thumbnail = $htmlpicture;
		$thumbnail =~ s/(.*)\.(jpg|png|gif)\b/$1.jpg/i;
      $clubphototext = <<HTML;
 <form name="cpaffil" action="http://www.clubphoto.com/_cgi-bin/quickorder" method="post">
 <input type=submit value=\"order this photo\">
 <input type="hidden" name="referer" value="my photo gallery">
 <input type="hidden" name="returnUrl" value="$ENV{'HTTP_REFERER'}">
 <input type="hidden" name="image_url" value="$htmlroot/$htmlalbum/$htmlpicture">
 <input type="hidden" name="image_width" value="$width">
 <input type="hidden" name="image_height" value="$height">
 </form> from ClubPhoto.com
HTML
		$clubphototext =~ s/\n//g;
   }

	$saveWidth = $width;
	$saveHeight= $height;
	$sizewarning = "";
	if(($width >= $maxWidth) || ($height >= $maxWidth)){
		$content .= createNewWidthPicture();

		#fix a few things when printing the picture out
		$htmlpicture = "$maxWidth/$htmlpicture";
		if($width > $height){
			$height =int ($height/($width/$maxWidth));
			$width = int ($maxWidth);
		}else{
			$width = int ($width/($height/$maxWidth));
			$height = int ($maxWidth);
		}
		$sizewarning = "(viewing at $width x $height)";
	}
	my $resizebutton ="
	<form method=post>
		<input type=hidden name=mode value=viewpicture>
		<input type=hidden name=album value=\"$selectedalbum\">
		<input type=hidden name=picture value=\"$selectedpicture\">
		<select name=maxWidth onchange=\"window.location=('index.cgi?mode=viewpicture&album=$selectedalbum&picture=$selectedpicture&maxWidth='+this.options[this.selectedIndex].value)\">";

	if($maxWidth eq "320"){
		if($saveWidth > 320){ $resizebutton .= "<option value=320 selected>very small (320)";}
	} else {
		if($saveWidth > 320){ $resizebutton .= "<option value=320>very small (320)";}
	}
	if($maxWidth eq "640"){
		if($saveWidth > 640){ $resizebutton .= "<option value=640 selected>small (640)";}
	} else {
		if($saveWidth > 640){ $resizebutton .= "<option value=640>small (640)";}
	}
	if($maxWidth eq "800"){
		if($saveWidth > 800){ $resizebutton .= "<option value=800 selected>medium (800)";}
	} else {
		if($saveWidth > 800){ $resizebutton .= "<option value=800>medium (800)";}
	}
	if($maxWidth eq "1024"){
		if($saveWidth > 1024){ $resizebutton .= "<option value=1024 selected>large (1024)";}
	} else {
		if($saveWidth > 1024){ $resizebutton .= "<option value=1024>large (1024)";}
	}
	if($maxWidth eq "9999"){
		$resizebutton .= "<option value=9999 selected>full size";
	} else {
		$resizebutton .= "<option value=9999>full size";
	}


	$resizebutton .= "
		</select>
		<!--<input type=submit value=\"resize\">-->
	</form>";
	my ($prettypicture) = $selectedpicture;
	$prettypicture = makepretty($prettypicture);

    $return .= print_location($selectedalbum, pageParams());

	my $table_width = $width;
	if($table_width < 320){
		$table_width = 320;
	}
	$return .= "
<center>
<br>
<table border=0 cellspacing=1 cellpadding=0 width=\"$table_width\"><tr><td>
<table cellpadding=0 border=0 cellspacing=0 width=100%>
<tr><td align=left>
	<table width=100%><tr>
	<!-- <td valign=middle align=left><a href=index.cgi?mode=view&album=$cgihtmlalbum><img src=$dataurl/site-images/back.gif border=0>&nbsp;<font size=-1>gallery</font></a></td> -->
	<a name=start_picture></a>
	<td valign=middle align=left>$prev</td>
	<td valign=middle align=middle><b><font style=\"font-family: Verdana;font-size: medium;\">$prettypicture</font></b></td>
	<td valign=middle align=right>$next</td>
	</tr></table>
</td></tr>";
	my $picture = "$photoroot/$selectedalbum/$selectedpicture";
	fileinfo($picture);
	my $hits = logpicture("$selectedalbum/$selectedpicture");
	my $hitstring = "<tr><td><b><font size=-2>viewed:</b></td>
		<td><font size=-2>$hits times</td></tr>";
	
	my $description = getdescription($selectedalbum,$selectedpicture);
	if($smiley =~ /yes/i){
		$description = make_faces($description);
	}
	my $commentmsg ='';
	if($allowcomments =~ /YES/i){
	$commentmsg = "<font size=-2>Users may add comments <a href=#comments>below</a></font>";
	}
	$shadowwidth=$width -16; #lower left shadow is 16 pixels wide. Current table layout requires the bottom shadow to that much less.
    if($exif=~ /yes/i){
        $exifdata = get_exifdata("$picture");
    }else{
        $exifdata = "";
    }
#<tr bgcolor=$descriptioncolor><td>$description</td></tr>
	$return .= "
<tr><td align=center>";
	if($shadow =~ /yes/i){
		$return .= "
	   <table border=0 cellspacing=1 cellpadding=0 width=\"$table_width\"><tr><td>
		<table bgcolor=\"#FFFFFF\" cellpadding=0 border=0 cellspacing=0 width=100%><tr><td align=center>";
		$return .="
	<table border=0 cellspacing=0 cellpadding=0>
		<tr>
		<td colspan=2>";
	}else{
		$return .= "
	   <table bgcolor=\"#000000\" border=0 cellspacing=1 cellpadding=0 width=\"$table_width\"><tr><td>
		<table bgcolor=\"#000000\" cellpadding=0 border=0 cellspacing=0 width=100%><tr><td align=center>";
	}
		
	if(($width < $maxWidth) && ($height < $maxWidth)){
		$return .= "<img src=\"$htmlroot/$htmlalbum/$htmlpicture\" width=$width height=$height border=0>";
	}else{
		$return .= "<img src=\"$dataurl/$htmlalbum/$htmlpicture\" width=$width height=$height border=0>";
	}
	
	if($shadow =~ /yes/i){
		$return .= "</td>
			<td align=left valign=top background=\"$dataurl/site-images/shadow_middle_right.jpg\"><img src=\"$dataurl/site-images/shadow_top_right.jpg\"></td>
		</tr>
		<tr>
			<td align=left valign=top><img src=\"$dataurl/site-images/shadow_bottom_left.jpg\"></td>
			<td width=$shadowwidth background=\"$dataurl/site-images/shadow_middle_bottom.jpg\">&nbsp;</td>
			<td align=right valign=top><img src=\"$dataurl/site-images/shadow_bottom_corner.jpg\"></td>
		</tr>
	</table>
	";
	}
	
	$return .="
</td></tr>
<tr bgcolor=$descriptioncolor><td>$description</td></tr>
<tr bgcolor=$descriptioncolor><td>

	<table width=100%><tr><td>
		<table cellspacing=0 cellpadding=0 border=0>
				<tr><td><b><font size=-2>uploaded: </b></td><td><font size=-2>$time</td></tr>
				<tr><td><b><font size=-2>file size: </td><td><font size=-2>$kfilesize</td></tr>
				$hitstring
				<tr><td><b><font size=-2>image size: </b></td><td><font size=-2>$saveWidth x $saveHeight $sizewarning</td></tr>
				<tr><td colspan=2><font size=-2> &nbsp;</td></tr>
				<tr><td colspan=2><font size=-2>$resizebutton</td></tr>
				<tr><td colspan=2><font size=-2> &nbsp;</td></tr>
				<tr><td colspan=2><font size=-2>$shutterflytext&nbsp;$clubphototext</td></tr>";

	$return .="
		</table>

		</td><td align=right valign=top>
 	   $exifdata
		</td></tr>
	</table>
</td></tr></table></td></tr></table>

</td></tr>
<tr>
	<td ><center> 
		<table width=100%><tr>
			<td valign=middle align=left>$prev</td>
			<td valign=middle align=left>$prevThumb</td>
			<td valign=middle nowrap><font size=-1>$pictureNumber out of $numberofpictures</font></td>
			<td valign=middle align=right>$nextThumb</td>
			<td valign=middle align=right>$next</td>
		</tr></table>
	</center></td></tr>
	<tr><td align=center>";

	if($allowcomments =~ /YES/i) {
		$return .= "
	   <table bgcolor=\"#000000\" border=0 cellspacing=1 cellpadding=0 width=\"$table_width\"><tr><td>
		<table bgcolor=\"$descriptioncolor\" cellpadding=0 border=0 cellspacing=0 width=100%><tr><td>";
		$return .= getcomments();
		$return .= "
		</td></tr></table></td></tr></table>";
	}
   	my $shutterflytext;
   	if($allowshutterfly =~ /YES/i){
   		$shutterflytext="<hr>
   		<img src=$dataurl/site-images/printsbyshutterfly.gif align=left><font size=-1>
   		This gallery has support for Shutterfly. Click the \"order this photo\" 
		button to quickly and easily order high-quality photographic prints of 
		your favorite pictures through Shutterfly.com";
   }
   $return .= "$shutterflytext";

   $return .= "
   	</td></tr></table>
	</td></tr></table>
	</center>
	";

	return $return;
}

sub logpicture {
	#needs to have the name of the picture passed as an argument. Opens log file. Increments for
	#that picture, and write the data file back out.
	
	#open datafile and get info
	if(-e "$dataroot/$selectedalbum/log.txt"){ # only if it exists
		open (INFILE, "$dataroot/$selectedalbum/log.txt")|| 
		dienice("I could not open the log file!
		If this is your first time running it, please manually create a blank data file.  You need a blank text file called log.txt in the same directory as the index.cgi.  This file should be rw by whatever user your web server runs as.");
		flock (INFILE, 2); #locks the file for exclusive access.
		while(<INFILE>){
			chomp();
			my ($picture,$hits) = split(/=/,$_); #read in each timeslot one per line
			$picturelog{"$picture"} = $hits;
		}
		flock (INFILE, 8); #unlocks the file
		close(INFILE);
	}

	#incriment hit counter for the current picture
	$picturelog{$_[0]}++;
	
	#re-write data file with new info
	open(OUTFILE,">$dataroot/$selectedalbum/log.txt") || dienice("Cound not open data file for writing!");
	flock (OUTFILE, 2);
	foreach $key (sort keys %picturelog) {
		print OUTFILE "$key=$picturelog{\"$key\"}\n";
	}
	flock (OUTFILE, 8);
	close(OUTFILE);

	return $picturelog{$_[0]};

}


sub getdescription($$) {
	my $return = "";
	
	#open datafile 
	if(open (INFILE, "$dataroot/$_[0]/descriptions/$_[1].txt")){ 
		flock (INFILE, 2); #locks the file for exclusive access.
		while(<INFILE>){
			$_ =~ s/[\r\f]/<br>/gi;
			$_ =~ s/\n//gi;
			$return .= "$_";
		}
		flock (INFILE, 8); #unlocks the file
		close(INFILE);
	}

	$return = "<table><tr><td>$return</td></tr></table>";
	
	return $return;
}

sub makepretty {
	my $pretty = $_[0];

	if($prettypicturenames =~ /YES/i) {
		if($remove_leading_numbers =~ /yes/i){
			$pretty =~ s/^[0-9]*//g;
		}
		$pretty =~ s/_/ /g;
		$pretty =~ s/-/ /g;
		$pretty =~ s/ /&nbsp;/g;
		$pretty =~ s/(.*)\..*/\1/g;
	}
	return $pretty;
}

#########################################################################
## open comments file and return a pretty table of the comments##########
#########################################################################
sub getcomments {
	my $return;
	my $count=0;
	my @comments;
	my $selecteditem;
	#make comments directory if it doesn't exist
	unless(-e "$dataroot/$selectedalbum/comments"){
        #print "Making directory $photoroot/$selectedalbum/comments...<br><br>";
		mkdir("$dataroot/$selectedalbum/comments",0755) || dienice("Error, could not make comments directory
			please check the permissions!<br><br>");
	}

	#little trick to get comments on movies to work :-)
	if(!($selectedpicture)){
		$selectedpicture = $selectedmovie;
	}

	#open datafile 
	if(open (INFILE, "$dataroot/$selectedalbum/comments/$selectedpicture.txt")){ 
		flock (INFILE, 2); #locks the file for exclusive access.
		while(<INFILE>){
			chomp();
			push (@comments,$_);
		}
		flock (INFILE, 8); #unlocks the file
		close(INFILE);
	}
	my $cgihtmlalbum = $selectedalbum;
	$cgihtmlalbum =~ s/ /+/g;
	my $cgihtmlpicture = $selectedpicture;
	$cgihtmlpicture =~ s/ /+/g;
	$return .= "
<center>
<form method=post>
<table width=100% border=0 cellspacing=0 cellpadding=3>";
	
	$return .= "<tr bgcolor=$rowcolor><td colspan=2 align=middle><a name=comments><b><font size=-1>Comments</font></b></td></tr>";
	my $commentcolor ="$commentcolor1";
	foreach $comment (@comments) {
		$count++;
		my ($time,$name,$email,$comment) = split(/=/,$comment,4);
		#alternate row colors
		if($commentcolor eq "$commentcolor1"){
			$commentcolor = $commentcolor2;	
		}else {
			$commentcolor = $commentcolor1;
		}
		my $email_comment;
		if ($email){ $email_comment = "<a href=mailto:$email><img src=$dataurl/site-images/email.gif border=0></a>";}
		if($smiley =~ /yes/i){
			$comment = make_faces($comment);
		}
		$return .="
<tr bgcolor=$commentcolor><td><b><font size=-1>$name $says[int(rand(@says))] : </font></b></td><td align=right><font size=-1>&nbsp;$email_comment</font></td></tr>
<tr bgcolor=$commentcolor><td colspan=2><font size=-1>$comment</font></td></tr>
<tr bgcolor=$commentcolor><td colspan=2 align=right><font size=-2>$time</font></td></tr>
";

	}
	unless($count){
		$return .= "<tr><td colspan=2>There are currently no comments for this item.</td></tr>";
	}
	$mode = $cur->param("mode");
	if($mode =~ /picture/i){
		$return .= "
<tr bgcolor=$rowcolor><td colspan=2 align=middle><b><font size=-1>Add Comment</font></b></td></tr>
<tr><td valign=top><font size=-1>Name:</font></td><td><input type=text name=name size=30></td></tr>
<tr><td valign=top><font size=-1>Email (optional):</font></td><td><input type=text name=email size=30></td></tr>
<tr><td valign=top><font size=-1>Comments:</font></td><td><textarea name=comment rows=3 cols=30 wrap=soft></textarea></td></tr>
<tr><td colspan=2 align=middle>
<input type=hidden name=mode value=viewpicture>
<input type=hidden name=album value=\"$selectedalbum\">
<input type=hidden name=picture value=\"$selectedpicture\">
<input type=submit value=\"Add Comment\"></td></tr>";
	} else {
		$return .= "
<tr bgcolor=$rowcolor><td colspan=2 align=middle><b>Add Comment</b></td></tr>
<tr><td valign=top>Name:</td><td><input type=text name=name size=30></td></tr>
<tr><td valign=top>Email:</td><td><input type=text name=email size=30></td></tr>
<tr><td valign=top>Comments:</td><td><textarea name=comment rows=3 cols=30 wrap=soft></textarea></td></tr>
<tr><td colspan=2 align=middle>
<input type=hidden name=mode value=viewmovie>
<input type=hidden name=album value=\"$selectedalbum\">
<input type=hidden name=movie value=\"$selectedmovie\">
<input type=submit value=\"Add Comment\"></td></tr>";
	}
	$return .="</table></form></center>";

	return $return;
}

#########################################################################
# takes comments from a form and adds them to the comments file #########
#########################################################################
sub getcomment {
	my $name = $cur->param("name");
	my $email= $cur->param("email");
	my $comment = $cur->param("comment");
	my $time = localtime(time());
	my $email_comment;
	my $body;

	$name= &stripBadHtml($name);	
	unless($name){$name = "anonymous";}
	$email = &stripBadHtml($email);	
	$comment = &stripBadHtml($comment);	
	$comment =~s /\n/<br>/g;

	#little trick to get comments on movies to work :-)
	if(!($selectedpicture)){
		$selectedpicture = $selectedmovie;
	}

	#append new comment to data file
	if(open(OUTFILE,">>$dataroot/$selectedalbum/comments/$selectedpicture.txt")) {
		flock (OUTFILE, 2);
		print OUTFILE "$time=$name=$email=$comment\n";
		flock (OUTFILE, 8);
		close(OUTFILE);
		
		if($email_comments_to_admin =~ /YES/i){
			$body = "$name ($email) posted a comment to your photo gallery at $ENV{'HTTP_REFERER'} on $time.
			\nAlbum:\t\t$selectedalbum\nPicture:\t$selectedpicture\n\nThey commented:\n$comment
			\nProvided by a a FREE script from http://www.fuzzymonkey.org/";
			email("comment posted on photo gallery",$admin_email,$email,$email,$body);
		}

		return $picturelog{$_[0]};
	}else {
		dienice("There was an error writing to the comments file.  This script does
		not have the proper file permissions to write to the comments file!  
		Please see the README or the CGI tutorial on fuzzymonkey.org for more information.");
	}
}

# searches current directory for current picture in order to get previous
# and next pictures.  This will be used to make previous and next buttons.
# the text for the previous and next buttons will be stored in
# global varaibles $next and prev
# also sets the $page variable to the page that the current picture is on.
sub getPrevNextButtons {
	my $album = $_[0];
	my $picture = $_[1];

	$album =~ s/ /\\ /g; #make spaces glob friendly
	my @files= glob ("$photoroot/$album/*");

	$album =~ s/\\ /+/g; #make glob friendly spaces, cgi friendly
	my @pictures;


	# discard directories, record offset of current picture, 
    # total number of pictures, and page number of current image.
	my $offset = -1;
    $numberofpictures = 0;  #for use in the main script
	for ($i=0; $i < (@files); $i++) {
        my $file = @files[$i];

        if ($files[$i] eq "$photoroot/$_[0]/$picture") {
            # know pic's offset
            $offset = $numberofpictures;

            #to know which picture out of how many we are viewing
            $pictureNumber = $offset + 1;

            #know what page the pic's on (note that this count includes dirs)
            $page = int($pictureNumber / $thumbnails_per_page);
            $page++ if ($pictureNumber % $thumbnails_per_page);  #take ceiling
        }

		if($file =~ /\.(jpg|png|gif)\b/i) {
			push(@pictures, $file);
            $numberofpictures++;
		}
	}

    if ($offset < 0) {
        dienice("Not found: $picture"); # pic wasn't there.
    }

	$prev = '&nbsp;';
	$next = '&nbsp;';
	$prevThumb = '&nbsp;';
	$nextThumb = '&nbsp;';
        
    # make next and prev buttons
    if($offset > 0) { 
        #this is not the first picture, so we make a prev button
        my $prevPicture = $pictures[$offset-1];
        $prevPicture =~ s#.*/##; # remove directory crap
        my $htmlprevPictureThumb = $prevPicture; #make a version for html (for thumbnail)
        $htmlprevPictureThumb =~ s/ /%20/g;	
        $htmlprevPictureThumb =~ s/#/%23/g;	
        #$htmlprevPictureThumb =~ s/(.*)\.(jpg|png|gif)\b/$1.jpg/i;
        $prevPicture =~ s/ /+/g; # make a cgi link friendly one
        $prevPicture =~ s/&/%26/g;# thanks to Andrew I Baznikin for fixing '&' in pictures
            
        $prev = "
<a href=\"index.cgi?album=$album&mode=viewpicture&picture=$prevPicture#start_picture\">
<img src=\"$dataurl/site-images/prev.gif\" border=0>&nbsp;<font size=-1>previous</font></a>";
        $prevThumb = "
<a href=\"index.cgi?album=$album&mode=viewpicture&picture=$prevPicture#start_picture\">
<img src=\"$dataurl/$htmlalbum/thumbnails/$htmlprevPictureThumb\" border=0></a>";
    }
    if($offset < ((@pictures)-1)){
        #this is not the last picture we make next button
        
        #prepair picture for link (get ride of 
        #directory info and spaces
        my $nextPicture = $pictures[$offset+1];
        $nextPicture =~ s#.*/##;
        my $htmlnextPictureThumb = $nextPicture; #make a version for html (for thumbnail)
        $htmlnextPictureThumb =~ s/ /%20/g;	
        $htmlnextPictureThumb =~ s/#/%23/g;	
        #$htmlnextPictureThumb =~ s/(.*)\.(jpg|png|gif)\b/$1.jpg/i;
        $nextPicture =~ s/ /+/g;	
        $nextPicture =~ s/&/%26/g; # thanks to Andrew I Baznikin for fixing '&' in pictures
        
        $next = "
<a href=\"index.cgi?album=$album&mode=viewpicture&picture=$nextPicture#start_picture\">
<font size=-1>next</font>&nbsp;<img src=\"$dataurl/site-images/next.gif\" border=0></A>";
        $nextThumb = "
<a href=\"index.cgi?album=$album&mode=viewpicture&picture=$nextPicture#start_picture\">
<img src=\"$dataurl/$htmlalbum/thumbnails/$htmlnextPictureThumb\" border=0></a>";
    }
}

##############################################################################
# Creates a new picture in the subdirectory for the current width
##############################################################################
sub createNewWidthPicture{
	my $return;

	#Make sure it is an allowed width
	if(!($maxWidth eq 320 ||$maxWidth eq 640 || $maxWidth eq 800 || $maxWidth eq 1024 || $maxWidth eq 9999)){
		dienice("$maxWidth is an invalid parameter for the picture width!");
	}

        #make directory for smaller pictures if it doesn't exist
        unless(-e "$dataroot/$selectedalbum/$maxWidth"){
                #print "Making directory $photoroot/$selectedalbum/$maxWidth ...<br><br>";
                mkdir("$dataroot/$selectedalbum/$maxWidth",0755) || dienice("Error, could not make directory for this size image
                        please check the permissions!<br><br>");
        }


	#create smaller size if it doesnt exist or the thumbnail is older than the picture
	unless((-e "$dataroot/$selectedalbum/$maxWidth/$selectedpicture")){
		resize("$photoroot/$selectedalbum/$selectedpicture",
			"$dataroot/$selectedalbum/$maxWidth/$selectedpicture",$maxWidth,$resize_quality);
	}

	return $return;
}

sub make_faces($){
	my $text = $_[0];
	unless($text && $smiley){return;} #skip if we don't have any text
					  #or we don't have smiley turned on

	$text =~ s/[:=][-o^]?\)/<img src="$dataurl\/site-images\/smiley-5.gif">/g;
	$text =~ s/[:=][-o^]?P/<img src="$dataurl\/site-images\/smiley-10.gif">/gi;
	$text =~ s/[:=][-o^]?D/<img src="$dataurl\/site-images\/smiley-1.gif">/g;
	$text =~ s/[:=][-o^]?[0O]/<img src="$dataurl\/site-images\/smiley-2.gif">/g;
	$text =~ s/[;=][-o^]?\)/<img src="$dataurl\/site-images\/smiley-4.gif">/g;
	$text =~ s/[:=][-o^]?\|/<img src="$dataurl\/site-images\/smiley-8.gif">/g;
	$text =~ s/[:=][-o^]?[\\]/<img src="$dataurl\/site-images\/smiley-9.gif">/g;
	$text =~ s/[:=][-o^][\/]{1,1}/<img src="$dataurl\/site-images\/smiley-9.gif">/g;
	#$text =~ s/o/test/g;

	return $text;
}

sub printsearchmatch {
	my $album = $_[0];
	$album =~ s/\\ /+/g; #make web link friendly
	$album =~ s/ /+/g; #make web link friendly
	my $picture = $_[1];
	$picture =~ s/\\ /+/g; #make web link friendly
	$picture =~ s/ /+/g; #make web link friendly
	if($picture =~ /.*\.(mpg|avi|mov|ram)/){ # we have a movie
		$mode = 'viewmovie';
		$arg = "movie=$picture";
	} else {
		$mode = 'viewpicture';
		$arg = "picture=$picture";
	}
	my $return = "<blockquote>
<table border=0 cellspacing=0>
<tr><td bgcolor=$descriptioncolor>Image: </td><td><a href=index.cgi?album=$album&mode=$mode&$arg>$picture</a></td></tr>
<tr><td bgcolor=$descriptioncolor>Album: </td><td>$album</td></tr>
<tr><td bgcolor=$descriptioncolor>Found in: </td><td>$_[2]</td></tr>
</table><br></blockquote>";
	return $return;
}

#####################################################3
#subs from slashdot to remove bad html
#####################################################
# Thanks to Slashdot for these
# Approved HTML tags for HTML posting
@approvedtags = (
        'B','I','P .*','P','A',
        'LI','OL','UL','EM','BR',
        'STRONG','BLOCKQUOTE',
        'HR','DIV .*','DIV','TT'
        );

sub stripBadHtml
{
        my ($str) = @_;

        $str =~ s/(\S{90})/$1 /g;
        $str =~ s/<(?!.*?>)//;
        $str =~ s/<(.*?)>/approvetag($1,@approvedtags)/sge; #replace
tags with approved ones
        return $str;
}

sub approvetag
{
        my ($tag,@apptag) = @_;

        $tag =~ s/^\s*?(.*)\s*?$/$1/e; #trim leading and trailing spaces

        if (uc(substr ($tag, 0, 2)) eq 'A ')
        {
                $tag =~ s/^.*?href="?(.*?)"?$/A HREF="$1"/i; #enforce "s
                return "<" . $tag . ">";
        }

        foreach my $goodtag (@apptag)
        {
                $tag = uc $tag;
                if ($tag eq $goodtag || $tag eq '/' . $goodtag)
                        {return "<" . $tag . ">";}
                #check against my list of tags
        }
        return "";
}

