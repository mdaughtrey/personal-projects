#!/usr/bin/perl

####################################################################
# Mike's Photo Gallery Manage Script
# 
# This script is part of Mike's Perl Script Archive at
# www.fuzzymonkey.org and is released under the GPL license.  Please
# see the included LICENSE.txt file for details.
#
# This script should be in a PASSWORD PROTECTED directory!
#
####################################################################

#if you had to install Image Magick as NON-root, please add the
# use lib line
#use lib "/home/username/local/lib/site_perl/5.6.0/i686-linux/";
#use lib "/home/username/local/lib/site_perl/5.6.0/";

use CGI;
use File::Path; #for removing a directory tree
$cur = CGI->new();
$version = "4.0";

require "../common.pl";

if($cur->param('saveconfig')){
	$content .= save_config();
}

if(!$directory_selection){$directory_selection = 'directory-simpleblue.gif';}

require "../sitevariables.pl";

$content .= "<p>My Photo Gallery Manage (Setup)</p>";

if($cur->param('mode')){
	$mode = $cur->param('mode');
}else{
	$mode = "default";
}

if($mode eq "create_dir_img"){
	require "../imagemagick.pl";
	my $album = $cur->param('album');
	my $img = $cur->param('img');
	overlay_directory("$dataroot/site-images/pictures.gif","$dataroot/$album/descriptions/directory.gif","$photoroot/$album/$img");
	print "Content-type: text/html\n\n";
	print <<HTML;
<html>
<body bgcolor=white>
Now using the image below:<br>
<img src=\"$dataurl/$album/descriptions/directory.gif\">
</body>
</html>
HTML
	exit();
}

$content .= printmenu();

if($mode eq "setup"){
	$content .= "<table width=300><tr><td>
	<p>Here we check the sitevariables.pl configuration file
	for simple errors and allow you to change most of the variabless in 
	that file.  At any time, you may also edit this file manually.</p>
	</td></tr></table>";
	$content .= check_deps();
	$content .= print_config_form();
}
if($mode eq "tools"){
	$content .= tools_menu();
}
if($mode eq "manage"){
	require "../imagemagick.pl";
	$content .= manage();
}
if($mode eq "upload"){
	$content .= uploadform();
}

printpage($content,"../template.html");

######################################################################
# subs
######################################################################
sub uploadform{
	my $return;
	my $album = $cur->param('album');	
	my @steps;
	my $sofar;
	my $step;

	@steps = split (/\//,"$album");
	$return .="<p><a href=\"manage.cgi?mode=manage\">Main</a> > ";
	foreach $step (@steps){
		if($sofar){
			$sofar = "$sofar/$step";
		}else{
			$sofar = $step;
		}
		$return .="<a href=\"manage.cgi?mode=manage&album=$sofar\">$step</a> > ";
	}
	$return .= "</p>";

	$return .="
	<form method=post ENCTYPE=\"multipart/form-data\">
	<input type=hidden name=mode value=manage>
	<input type=hidden name=album value=\"$album\">
	<input type=hidden name=upload value=\"yes\">
	<table width=700 bgcolor=#000000 border=0 cellpadding=0 cellspacing=1><tr><td>
	<table width=100% bgcolor=#FFFFFF cellspacing=0>
	<tr><td colspan=2 bgcolor=#000000 align=center><font color=white><b>Upload Pictures</b></font></td></tr>
	<tr><td colspan=2>You may upload pictures or a .zip file of pictures.  If you upload a .zip file, it will be extracted
	<tr><td colspan=2><br></td></tr>
	<tr><td>picture or zip file 1</td><td><input type=\"file\" name=\"FILE1\"></td></tr>
	<tr><td>picture or zip file 2</td><td><input type=\"file\" name=\"FILE2\"></td></tr>
	<tr><td>picture or zip file 3</td><td><input type=\"file\" name=\"FILE3\"></td></tr>
	<tr><td>picture or zip file 4</td><td><input type=\"file\" name=\"FILE4\"></td></tr>
	<tr><td>picture or zip file 5</td><td><input type=\"file\" name=\"FILE5\"></td></tr>
	<tr><td colspan=2><input type=\"submit\" value=\"Upload files\"></td></tr>
	</table></td></tr></table>
	</form>";

	return $return;
}

sub list_directories {
	my $dir_to_list = $_[0];
	my $dir_to_go;
	my @directories;
	my @new_directories;

	$dir_to_list =~ s/\/$//g;
	$dir_to_list =~ s/ /\\ /g;
	
	@directories = glob("$dir_to_list/*/");
	foreach $dir_to_go (@directories){
		push (@new_directories,"$dir_to_go");
		push(@new_directories,list_directories("$dir_to_go"));
	}
	@directories = @new_directories;
	
	return @directories;
}
sub manage{
	my $return;
	my $album;
	my @files;
	my $file;
	my $file_cgi;
	my @dirs;
	my @pictures;
	my ($sofar,$step,@steps);
	my @params;
	my $param;
	my $description;
	my $status;

	$album = $cur->param('album');

	$return .= check_data_directories("$photoroot/$album");

	if($cur->param('upload')){
		$return .= uploadpictures();
	}

	@params = $cur->param();
	foreach $param (@params){
		if($param =~ /^description##(.*)/){
			$file = $1;
			$description = $cur->param("$param");
			
			if($description){
				if(open(DESCRIPTION_OUT,">$dataroot/$album/descriptions/$file.txt")){
					print DESCRIPTION_OUT "$description";
					close(DESCRIPTION_OUT);
				}else{
					$return .= "<font color=red>WARNING: Could not write out to file $dataroot/$album/descriptions/$file! $!</font>";
					
				}
			}
		}elsif($param =~ /^delete##(.*)/){
			$file = $1;
			if((-e "$photoroot/$album/$file")&&(-w "$photoroot/$album/$file")){
				#$return .="<table>";
				#$return .= "<tr><td colspan=2><font color=red>deleting $file</font></td></tr>";
				#$return .= "<tr><td><font color=red>&nbsp;&nbsp;&nbsp;&nbsp;original size (from original source)</font></td><td>[OK]</td></tr>";
				unlink("$photoroot/$album/$file")|| ($warnings .= "<font color=red>$!: could not remove $photoroot/$album/$file!</font><br>");
				if(-e "$dataroot/$album/thumbnails/$file"){
					$status = "[OK]";
					unlink("$dataroot/$album/thumbnails/$file")|| ($status = "[FAILED]");
					#$return .= "<tr><td><font color=red>&nbsp;&nbsp;&nbsp;&nbsp;thumbnail</font></td><td>$status</td></tr>";
				}
				if(-e "$dataroot/$album/320/$file"){
					$status = "[OK]";
					unlink("$dataroot/$album/320/$file")|| ($status = "[FAILED]");
					#$return .= "<tr><td><font color=red>&nbsp;&nbsp;&nbsp;&nbsp;320 size</font></td><td>$status</td></tr>";
				}
				if(-e "$dataroot/$album/640/$file"){
					$status = "[OK]";
					unlink("$dataroot/$album/640/$file")|| ($status = "[FAILED]");
					#$return .= "<tr><td><font color=red>&nbsp;&nbsp;&nbsp;&nbsp;640 size</font></td><td>$status</td></tr>";
				}
				if(-e "$dataroot/$album/800/$file"){
					$status = "[OK]";
					unlink("$dataroot/$album/800/$file")|| ($status = "[FAILED]");
					#$return .= "<tr><td><font color=red>&nbsp;&nbsp;&nbsp;&nbsp;800 size</font></td><td>$status</td></tr>";
				}
				if(-e "$dataroot/$album/1024/$file"){
					$status = "[OK]";
					unlink("$dataroot/$album/1024/$file")|| ($status = "[FAILED]");
					#$return .= "<tr><td><font color=red>&nbsp;&nbsp;&nbsp;&nbsp;1024 size</font></td><td>$status</td></tr>";
				}
				if(-e "$dataroot/$album/comments/$file.txt"){
					$status = "[OK]";
					unlink("$dataroot/$album/comments/$file.txt")|| ($status = "[FAILED]");
					#$return .= "<tr><td><font color=red>&nbsp;&nbsp;&nbsp;&nbsp;comment</font></td><td>$status</td></tr>";
				}
				if(-e "$dataroot/$album/descriptions/$file.txt"){
					$status = "[OK]";
					unlink("$dataroot/$album/descriptions/$file.txt")|| ($status = "[FAILED]");
					#$return .= "<tr><td><font color=red>&nbsp;&nbsp;&nbsp;&nbsp;description</font></td><td>$status</td></tr>";
				}

				#$return .= "</table>";
			}else{
				$return .= "<font color=red>script does not have permissions to modify $photoroot/$album/$file.  This file 
				cannot be deleted by this script.  You will have to remove this file manually or change the permissions of 
				$photoroot/ to allow access.</font>";
			}

		}elsif($param =~ /^deletedir##(.*)/){
			$file = $1;
			if(-d "$photoroot/$file"){
				#$return .= "removing directory $photoroot/$file<br>";
				rmtree("$photoroot/$file",0,0) || dienice("cannot remove $photoroot/$file.  This is probably because the user that your webserver runs as does not have permissions to this directory. $!");
			}
			if(-d "$dataroot/$file"){
				#$return .= "removing directory $dataroot/$file<br>";
				rmtree("$dataroot/$file",0,0) || dienice("cannot remove $dataroot/$file.  This is probably because the user that your webserver runs as does not have permissions to this directory.  $!");
			}

		}elsif($param =~ /^rotatecw##(.*)/){
			$file = $1;
			$return .= rotate("$photoroot/$album/$file",90,$resize_quality);
			$status = "[OK]";
			if(-e "$dataroot/$album/thumbnails/$file"){
				unlink("$dataroot/$album/thumbnails/$file")|| ($status = "[FAILED]");
			}
			if(-e "$dataroot/$album/320/$file"){
				unlink("$dataroot/$album/320/$file")|| ($status = "[FAILED]");
			}
			if(-e "$dataroot/$album/640/$file"){
				unlink("$dataroot/$album/640/$file")|| ($status = "[FAILED]");
			}
			if(-e "$dataroot/$album/800/$file"){
				unlink("$dataroot/$album/800/$file")|| ($status = "[FAILED]");
			}
			if(-e "$dataroot/$album/1024/$file"){
				unlink("$dataroot/$album/1024/$file")|| ($status = "[FAILED]");
			}
			$return .= "<font color=green>rotating $file clockwise. And refreshing old thumbs and sizes ($status).  Your browser may cache the old thumbnail and not update it's new layout.</font><br>";
		
		}elsif($param =~ /^rotateccw##(.*)/){
			$file = $1;
			$return .= rotate("$photoroot/$album/$file",270,$resize_quality);
			$status = "[OK]";
			if(-e "$dataroot/$album/thumbnails/$file"){
				unlink("$dataroot/$album/thumbnails/$file")|| ($status = "[FAILED]");
			}
			if(-e "$dataroot/$album/320/$file"){
				unlink("$dataroot/$album/320/$file")|| ($status = "[FAILED]");
			}
			if(-e "$dataroot/$album/640/$file"){
				unlink("$dataroot/$album/640/$file")|| ($status = "[FAILED]");
			}
			if(-e "$dataroot/$album/800/$file"){
				unlink("$dataroot/$album/800/$file")|| ($status = "[FAILED]");
			}
			if(-e "$dataroot/$album/1024/$file"){
				unlink("$dataroot/$album/1024/$file")|| ($status = "[FAILED]");
			}
			$return .= "<font color=green>rotating $file counter-clockwise and refreshing old thumbs and sizes ($status).  Your browser may cache the old thumbnail and not update it's new layout.</font><br>";

		}elsif($param =~ /^new_dir_name/){
			$new_dir_name = $cur->param('new_dir_name');
			$new_dir_name =~ s/\\$//ig;
			$new_dir_name =~ s/\/$//ig;
			$new_dir_name =~ s/\'//ig;
			$new_dir_name =~ s/\"//ig;
			$new_dir_name =~ s/\s$//ig;
			$new_dir_name =~ s/^\s//ig;
			$new_dir_name =~ s/\n//ig;
			unless(-e "$photoroot/$album/$new_dir_name"){
				mkdir("$photoroot/$album/$new_dir_name",0755);
				check_data_directories("$photoroot/$album/$new_dir_name")
				#$return .= "making $photoroot/$album/$new_dir_name $!<br>";
			}
		}elsif($param =~ /^rename##(.*)/){
			$file = $1;
			$new_name = $cur->param($param);
			unless($file eq $new_name){
				$file =~ /(\.\w{3,4})$/i;
				$ext = $1;
				$new_name =~ s/\.\w{3,4}$//i;
				$new_name =~ s/ /_/g; #replace spaces with underscores (anyone not want this?)
				$new_name .= $ext;

				$return .= move_file($album,$file,$album,$new_name);
			}
		}elsif($param =~ /^move##(.*)/){
			$file = $1;
			$new_dir= $cur->param($param);
			$new_dir =~ s/\/$//;
			if($new_dir){
				$return .= move_file($album,$file,$new_dir,$file);
			}
		}
	}

	@steps = split (/\//,"$album");
	$return .="<p><a href=\"manage.cgi?mode=manage\">Main</a> > ";
	foreach $step (@steps){
		if($sofar){
			$sofar = "$sofar/$step";
		}else{
			$sofar = $step;
		}
		$sofar =~ s/ /+/gi; #make cgi friendly link
		$return .="<a href=\"manage.cgi?mode=manage&album=$sofar\">$step</a> > ";
	}
	$return .= "</p>";

	$album_glob = $album;
	$album_glob =~ s/ /\\ /gi;
	@files = glob("$photoroot/$album_glob/*");

	# get a list of all of the directories so that we know what our options
	# are for moving
	@all_directories = list_directories("$photoroot");
	foreach (@all_directories){
		$_ =~ s/^$photoroot\///g;	
	}

	foreach $file (@files){
		if(-d $file){
			if(
				($file =~ /\/thumbnails$/)||
				($file =~ /\/site-images$/)||
				($file =~ /\/descriptions$/)||
				($file =~ /\/comments$/)||
				($file =~ /\/320$/)||
				($file =~ /\/640$/)||
				($file =~ /\/800$/)||
				($file =~ /\/1024$/)
				){
			}else{
				push(@dirs,"$file");
			}
		}else{
			push(@pictures,"$file");
		}
	}

	$return .="<table width=700><tr><td colspan=2>Please make any changes you like to this album (descriptions, checkboxes, etc) and hit \"Save Changes\" at the bottom of this form.</td></tr>
	<tr><td>";
	$return .= "
		<p><form method=post>
		<input type=hidden name=mode value=manage>
		<input type=hidden name=album value=\"$album\">
		<input type=text name=new_dir_name>
		<input type=submit value=\"add directory\">
		</form></p>";
	$return .="</td><td>";
	$return .= "
		<p><form method=post>
		<input type=hidden name=mode value=upload>
		<input type=hidden name=album value=\"$album\">
		<input type=submit value=\"upload pictures and files\">
		</form></p>";
	$return .="</td></tr></table>";

	$return .= "<form method=post>
		<input type=hidden name=mode value=manage>
		<input type=hidden name=album value=\"$album\">";
	$description="";
	if(open(DESCRIPTION,"<$dataroot/$album/descriptions/album.txt")){
		while(<DESCRIPTION>){
			$description .= $_;
		}
		close(DESCRIPTION);
	}

	if($watermark_file){
		if(open(WATERMARK_LOG,"<../watermark.log")){
			flock(2,WATERMARK_LOG);
			while(<WATERMARK_LOG>){
				chomp;
				push(@watermarked_pictures,$_);
			}
			flock(8,WATERMARK_LOG);
			close(WATERMARK_LOG);
		}
	}

	$return .="
	<table width=500 bgcolor=#000000 cellspacing=1 cellpadding=0 border=0><tr><td>
	<table width=100% bgcolor=#FFFFFF><tr><td>
	<font color=red><small>Note: For any single picture, never rename, move, or edit the comments at the same time.  For instance,
	first, rename the image.  Next, you can edit the comments.  Later, you can move it to a different folder.  If you try to do
	more than one of these at one time then only one of them will occur and the rest of the actions will be ignored.</small></font>
	</td></tr></table>
	</td></tr></table>

	<br>

	<table width=700 bgcolor=#000000 border=0 cellpadding=0 cellspacing=1><tr><td>
	<table width=100% bgcolor=#FFFFFF cellspacing=0>
	<tr><td bgcolor=#000000 align=center><font color=white><b>Album Description</b></font></td></tr>
	<tr><td align=center><textarea name=\"description##album\" cols=50 rows=5>$description</textarea></td></tr>
	</table></td></tr></table>
	";
	if(@dirs){
		$return .= "
		<table width=700 bgcolor=#000000 border=0 cellpadding=0 cellspacing=1><tr><td>
		<table width=100% bgcolor=#FFFFFF cellspacing=0>
		<tr><td colspan=3 bgcolor=#000000 align=center><font color=white><b>Directories</b></font></td></tr>
		";
		foreach $file (@dirs){
			$size = `du -h --max-depth=0 "$file"`;
			($size,@whatever)=split(/\s/,$size);
			$file =~ s/$photoroot//i;
			$file =~ s/^\/*//i;
			#$file =~ s/ /%20/i;
			$file_cgi = $file;
			$file_cgi =~ s/ /+/gi;
			if(-e "$dataroot/$file/descriptions/directory.gif"){
				$directory_img = "<img src=\"$dataurl/$file/descriptions/directory.gif\" border=0>";
			}else{
				$directory_img = "<img src=\"$dataurl/site-images/$directory_selection\" border=0>";
			}
			$return .= "<tr>
					<td valign=center><a href=\"manage.cgi?mode=manage&album=$file_cgi\">$directory_img$file</a></td>
					<td><input type=checkbox name=\"deletedir##$file\">&nbsp;delete</td>
					<td>$size</td>
				</tr>";
		}
		$return .= "</table>
		</td></tr></table>";
	}
	if(@pictures){
		$return .= "
		<table width=700 bgcolor=#000000 border=0 cellpadding=0 cellspacing=1><tr><td>
		<table width=100% bgcolor=#FFFFFF cellspacing=0>
		<tr><td colspan=3 bgcolor=#000000 align=center><font color=white><b>Files</b></font></td></tr>
		";
		foreach $file (@pictures){
			$file =~ s/$photoroot//i;
			$file =~ s/^\/*//i;

			# picture
			if(($file =~ /\.jpg$/i)||($file =~ /\.gif$/i)||($file =~ /\.png$/i)){
				$description="";
				if($file =~ /(.*)\/(.*\..*)/){
					$path = $1;
					$file = $2;
				}
				#if watermarking is turned on, watermark files that haven't been
				#watermarked yet
				if($watermark_file && (-r $watermark_file)){
					# see if we've watermarked this before
					$already_watermarked = "no";
					foreach $watermarked_picture (@watermarked_pictures){
						if($watermarked_picture eq "$photoroot/$path/$file"){
							$already_watermarked = "yes";
						}
					}
					unless($already_watermarked =~ /yes/i){
						if(-w "$photoroot/$path/$file"){
							if(open(WATERMARK_LOG,">>../watermark.log")){
								overlay("$photoroot/$path/$file","$photoroot/$path/$file",$watermark_file);
								flock(2,WATERMARK_LOG);
								print WATERMARK_LOG "$photoroot/$path/$file\n";
								flock(8,WATERMARK_LOG);
								close(WATERMARK_LOG);
							}else{
								dienice("Could not write to the watermarking log file (watermark.log).  You specified a file to watermark to all of your images in the setup.  The file $file is not in this log from what I can tell and so I was going to try to watermark it.  Unfortunately, I must now refuse to do this because I have no way of recording that i am doing this.  If I didn't refuse to do this, I would write over the file every time this page was loaded with another watermark and reduce the qualitiy of the image.  Fix the permissions on the watermark.log file or turn off the watermarking feature.");
							}
						}else{
							dienice("ERROR:  Could not watermark $photoroot/$path/$file: $!.  This is probably a permissions problem.  Does this script have permissiosn to open this file, watermark it, and save it back?\n");
						}
					}

				}

				#check for an existing thumbnail
				unless(-e "$dataroot/$album/thumbnails/$file"){
					#$warnings .= "<font size=-1 color=red>creating thumbnail for $file</font><br>";
					resize("$photoroot/$path/$file","$dataroot/$album/thumbnails/$file",$previewWidth,$resize_quality);
					if($mag_on_thumbnails =~ /yes/i){
						overlay("$dataroot/$album/thumbnails/$file","$dataroot/$album/thumbnails/$file","$dataroot/site-images/mag.png");
					}
				}
				if(open(DESCRIPTION,"<$dataroot/$album/descriptions/$file.txt")){
					while(<DESCRIPTION>){
						$description .= $_;
					}
					#$comment =~ s/[\r\f]/<br>/gi;
					#$comment =~ s/[\n]//gi;
					close(DESCRIPTION);
				}
				my $ctime = time();
				$return .= "<tr>
				<td><a href=\"../index.cgi?mode=viewpicture&picture=$file&album=$album\" target=\"newwindow\"><img src=\"$dataurl/$path/thumbnails/$file?$ctime\" border=\"0\"></a><br>
				<small><input type=\"text\" name=\"rename##$file\" value=\"$file\" size=\"30\"></small></td>
				<td width=100%>
					<input type=checkbox name=\"delete##$file\"><small>delete</small><br>
					<small><input type=checkbox name=\"rotatecw##$file\">rotate cw</small><br>
					<small><input type=checkbox name=\"rotateccw##$file\">rotate ccw</small><br>
					<a HREF=\"#null\" OnClick=\" javascript:window.open('manage.cgi?mode=create_dir_img&img=$file&album=$album','Popup','width=400,height=200,scrollbars=1')\">use as directory image</a><br>
				</td>
				<td><textarea name=\"description##$file\" cols=50 rows=5>$description</textarea><br>move to <select name=\"move##$file\"><option></option>";
				foreach $all_dir (@all_directories){
					$return .= "<option value=\"$all_dir\">$all_dir</option><br>";
				}
				$return .= "</select></td></tr>";
			}elsif( ($file =~ /.*(\..*)\b/i) && (defined $filetypes{"$1"})){
				fileinfo($file);
			        my $ext = $1;
				if($file =~ /(.*)\/(.*\..*)/){
					$path = $1;
					$file = $2;
				}
				$return .= "<tr><td><img src=\"$dataurl/site-images/$filetypes{\"$ext\"}\"><br>
				<small>$file</small></td>
				<td width=100%><input type=checkbox name=\"delete##$file\">delete</td>
				<td></td></tr>";
			}else{
				fileinfo($file);
				$file =~ /(.*)\/(.*\..*)/;
				$path = $1;
				$file = $2;
				$return .= "<tr><td><img src=\"$dataurl/site-images/unknown.jpg\"><br>
				<small>$file</small></td>
				<td width=100%><input type=checkbox name=\"delete##$file\">delete</td>
				<td></td></tr>";
			}
		}
		$return .= "</table>
		</td></tr></table>
		";
	}
	$return .= "<input type=submit value=\"Save Changes\"></form>";
	return "$warnings$return";
}

##################################################################
# move a file from one directory to another
# $_[0] = original album
# $_[1] = original file 
# $_[2] = new album
# $_[3] = new file 
##################################################################
sub move_file($$$$){
	my $return;
	my $original_dir = $_[0]; 
	my $original_file= $_[1];
	my $new_dir = $_[2];
	my $new_file= $_[3];
	my @watermarked_pictures;
	my $found = "no";
	
	$status = "<font color=green>[OK]</font>";
	if(-e "$photoroot/$original_dir/$original_file"){
		if(rename("$photoroot/$original_dir/$original_file","$photoroot/$new_dir/$new_file")){
		# if the photo is properly renamed lets rename all the data files
			if(-e "$dataroot/$original_dir/thumbnails/$original_file"){
				rename("$dataroot/$original_dir/thumbnails/$original_file","$dataroot/$new_dir/thumbnails/$new_file")|| ($status = "[FAILED] thumbnail");
			}
			if(-e "$dataroot/$original_dir/320/$original_file"){
				rename("$dataroot/$original_dir/320/$original_file","$dataroot/$new_dir/320/$new_file")|| ($status = "[FAILED] 320");
			}
			if(-e "$dataroot/$original_dir/640/$original_file"){
				rename("$dataroot/$original_dir/640/$original_file","$dataroot/$new_dir/640/$new_file")|| ($status = "[FAILED] 640");
			}
			if(-e "$dataroot/$original_dir/800/$original_file"){
				rename("$dataroot/$original_dir/800/$original_file","$dataroot/$new_dir/800/$new_file")|| ($status = "[FAILED] 800");
			}
			if(-e "$dataroot/$original_dir/1024/$original_file"){
				rename("$dataroot/$original_dir/1024/$original_file","$dataroot/$new_dir/1024/$new_file")|| ($status = "[FAILED] 1024");
			}
			if(-e "$dataroot/$original_dir/descriptions/$original_file.txt"){
				rename("$dataroot/$original_dir/descriptions/$original_file.txt","$dataroot/$new_dir/descriptions/$new_file.txt")|| ($status = "[FAILED] description");
			}
			if(-e "$dataroot/$original_dir/comments/$original_file.txt"){
				rename("$dataroot/$original_dir/comments/$original_file.txt","$dataroot/$new_dir/comments/$new_file.txt")|| ($status = "[FAILED] comments");
			}

			#just in case we watermarked this picture we need to note that
			if($watermark_file){
				$found = "no";
				if(open(WATERMARK_LOG,"<../watermark.log")){
					flock(2,WATERMARK_LOG);
					while(<WATERMARK_LOG>){
						chomp;
						push(@watermarked_pictures,$_);
						if($_ eq "$photoroot/$original_dir/$original_file"){$found = "yes";}
					}
					flock(8,WATERMARK_LOG);
					close(WATERMARK_LOG);
				}
				if($found =~ /yes/i){ # this picture has already been watermarked
					if(open(WATERMARK_LOG,">../watermark.log")){
						flock(2,WATERMARK_LOG);
						foreach $watermarked_picture (sort @watermarked_pictures){
							unless($watermarked_picture eq "$photoroot/$original_dir/$original_file"){
								print WATERMARK_LOG "$watermarked_picture\n";
							}
						}
						print WATERMARK_LOG "$photoroot/$new_dir/$new_file\n";
						flock(8,WATERMARK_LOG);
						close(WATERMARK_LOG);
					}
				}
			}


		}else{
			$status .= "<font color=red>[FAILED] $!.  I couldn't move main file so I quit!  I tried to move $photoroot/$original_dir/$original_file to $photoroot/$new_dir/$new_file.</font>";
		}
	}
	$return .= "<font color=green>moving $original_dir/$original_file to $new_dir/$new_file</font> $status<br>";

	return $return;
}

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

	$directory =~ s/^$photoroot\///i;
	if($directory){
		@steps = split (/\//,"$directory");
		#$return .= "checking data directories for $directory<br>";
		foreach $step (@steps){
			$step =~ s/\///gi;
			if($sofar){
				$sofar = "$sofar/$step";
			}else{
				$sofar = "$dataroot/$step";
			}
			unless(-e "$sofar"){
				#$return .= "creating $sofar with 755 permissions\n";
				mkdir("$sofar",0755)||dienice("Could not create data directory! $!");
				$count_new_directories++;
			}
		}
	}else{
		$sofar = $dataroot;
	}
	unless(-e "$sofar/thumbnails"){
		mkdir("$sofar/thumbnails",0755)||dienice("$directory Could not create thumbnails directory $sofar/thumbnails! $!");
		$count_new_directories++;
	}
	unless(-e "$sofar/comments"){
		mkdir("$sofar/comments",0755)||dienice("Could not create comments directory! $!");
		$count_new_directories++;
	}
	unless(-e "$sofar/descriptions"){
		mkdir("$sofar/descriptions",0755)||dienice("Could not create descriptions directory! $!");
		$count_new_directories++;
	}
	unless(-e "$sofar/320"){
		mkdir("$sofar/320",0755)||dienice("Could not create 320 directory! $!");
		$count_new_directories++;
	}
	unless(-e "$sofar/640"){
		mkdir("$sofar/640",0755)||dienice("Could not create 640 directory! $!");
		$count_new_directories++;
	}
	unless(-e "$sofar/800"){
		mkdir("$sofar/800",0755)||die("Could not create 800 directory! $!");
		$count_new_directories++;
	}
	unless(-e "$sofar/1024"){
		mkdir("$sofar/1024",0755)||die("Could not create 1024 directory! $!");
		$count_new_directories++;
	}
	#$return .= "I made $count_new_directories directories<br>";
	return $return;
}

sub tools_menu{
	my $return;
	$return .= "<table width=700><tr><td>";
	$return .= "<p>This section is currently empty.</p>";
	$return .= "<p>If you want to create all of your thumbnails
		and/or resized images before-hand instead of dynamically
	the first time someone clicks on that size image, you can use
	the createfiles.pl script included with this application.  That
	script will create all of the files that haven't already been
	created.  It will take a long time the first time it is run, but
	after that it is quick.</p>";
	$return .= "<p>To run it, simply run:<br>
	./createfiles.pl<br><br>
	some options for fine tunning:<br>
	-c config_file (to specify a config file if it cannot find it,
	e.g. cron jobs)<br>
	-s size,size,size (specify more than just the default of thumbnails
		and 640)<br>
	-q quite mode for running in a cron job .. this mode doesn't ask you
	to verify anything</p>";
	$return .= "<p>Example:<br>
	./createfiles.pl -s 320,640,800<br>
	This will create thumbs and the three specified sizes.<br><br>
	./createfiles.pl -q -c /var/www/cgi-bin/photo/sitevariables.pl<br>
	This is a good way to run the script  nightly with cron.  It runs in quite
	mode without asking for any input and is given the full path to the
	config file since cron cannot use relative paths.</p>";
	$return .= "</td></tr></table>";
	return $return;
}
sub save_config{
	my $return;
	my $photoroot = $cur->param('photoroot');
	my $htmlroot = $cur->param('htmlroot');
	my $dataroot = $cur->param('dataroot');
	my $dataurl = $cur->param('dataurl');
	my $title = $cur->param('title');
	my $prettypicturenames = $cur->param('prettypicturenames');
	my $remove_leading_numbers= $cur->param('remove_leading_numbers');
	my $allowcomments = $cur->param('allowcomments');
	my $indicate_comments = $cur->param('indicate_comments');
	my $smiley = $cur->param('smiley');
	my $allowshutterfly = $cur->param('allowshutterfly');
	my $allowclubphoto = $cur->param('allowclubphoto');
	my $shadow = $cur->param('shadow');
	my $mag_on_thumbnails = $cur->param('mag_on_thumbnails');
	my $email_comments_to_admin = $cur->param('email_comments_to_admin');
	my $admin_email = $cur->param('admin_email');
	my $sendmail = $cur->param('sendmail');
	my $rowcolor = $cur->param('rowcolor');
	my $descriptioncolor = $cur->param('descriptioncolor');
	my $commentcolor1 = $cur->param('commentcolor1');
	my $commentcolor2 = $cur->param('commentcolor2');
    my $thumbnails_per_page = $cur->param('thumbnails_per_page');
	my $thumbnails_per_row = $cur->param('thumbnails_per_row');
	my $previewWidth = $cur->param('previewWidth');
	my $resize_quality = $cur->param('resize_quality');
	my $exif = $cur->param('exif');
	my $jpegtran = $cur->param('jpegtran');
	my $watermark_file = $cur->param('watermark_file');
	my $directory_selection = $cur->param('directory_selection');

	if($prettypicturenames){$prettypicturenames="yes";}
	if($remove_leading_numbers){$remove_leading_numbers="yes";}
	if($allowcomments){$allowcomments="yes";}
	if($indicate_comments){$indicate_comments="yes";}
	if($smiley){$smiley="yes";}
	if($allowshutterfly){$allowshutterfly="yes";}
	if($allowclubphoto){$allowclubphoto="yes";}
	if($shadow){$shadow="yes";}
	if($mag_on_thumbnails){$mag_on_thumbnails="yes";}
	if($email_comments_to_admin){$email_comments_to_admin="yes";}
	unless ($thumbnails_per_page =~ /^[+-]?\d+$/) { $thumbnails_per_page = 'off'; }
    if($exif){$exif="yes";}
    

	$time = localtime(time());


	open(CONFIG,">../sitevariables.pl")||dienice("Could not open config file for writing!: $!");
	print CONFIG <<TEXT;
############################################################
# My Photo Gallery configuration file
# 
# Please read this file completely and edit the variables
# to suit your site.  It just takes a minute and makes sure
# you are up and running smoothly. 
# 
# config file updated using manage script on $time
############################################################

# First, lets get the hard stuff out of the way.  Let's tell
# the script where you installed it.

#Title of your photo gallery
\$title = "$title";

# This is the path to the root directory containing your albums.
# Each album should be stored as a subdirectory under this. 
# please no trailing /
#example - \$photoroot ='/var/www/html/photo';
\$photoroot ='$photoroot';

#URL that points to \$photoroot.  This should be how the browser gets to it.
#please no trailing /
#example - \$htmlroot = 'http://pancakes.darktech.org/photo';
\$htmlroot = '$htmlroot';

#Path to a directory were we can store the thumbnails, site-images/icons,
#descriptions, etc.
\$dataroot ='$dataroot';

#URL that points to a directory for storing the thumbnails, site-images/icons,
#descriptions, etc.  This should be a directory that your webserver can serve
#images from (normally under www/ or html/)
\$dataurl = '$dataurl';

\$prettypicturenames = '$prettypicturenames'; # eliminate spaces and drop off extensions?
\$remove_leading_numbers = '$remove_leading_numbers'; # remove leading numbers from filenames?
\$allowcomments = '$allowcomments'; # allow comments on each pictures?
\$indicate_comments = '$indicate_comments'; # put a small icon next to thumbnails with comments?
\$smiley = '$smiley'; # replace things like :-) with a smiley face?
\$allowshutterfly = '$allowshutterfly'; # enable visitors to order prints via shutterfly.com?
\$allowclubphoto = '$allowclubphoto'; # enable visitors to order prints via clubphoto.com?
\$shadow = '$shadow'; # shadow around picture? (only works with white background)
\$mag_on_thumbnails = '$mag_on_thumbnails'; # overlay thumbnails with "zoom" icon?

# Set this to YES if you would like your users to be able to post comments
# on each picture.  The comments will be stored in plain text files in
# a subdirectory in each album called comments/

# Set this to YES to have each comment emailed to you when it is placed.
# In order for this to work properly, you must also specify an email
# address and path to your sendmail
\$email_comments_to_admin = '$email_comments_to_admin';
\$admin_email = '$admin_email';
\$sendmail = '$sendmail';

# This is a list of all the discriptions used to as verbs for the comments
# I suggest leaving them how they are :-)
\@says=("says","blurted","screamed","burped","commented","thought","exclaimed","hinted");

#########################################################################
# Colors, look, and feel
#
# NOTE: most of the look of your page is designated in template.html
# please edit this file to change the look of your gallery.  It is a 
# simple .html file with a special tag <!-- content --> in place of
# what this script will generate.
#########################################################################
# row color is used as the background for the menu and the title bar for each
# picture/item
\$rowcolor         = '$rowcolor';
# descriptioncolor is the background color for the table around each picture
\$descriptioncolor = '$descriptioncolor';
# alternating colors for rows of comments
\$commentcolor1 = "$commentcolor1";
\$commentcolor2 = "$commentcolor2";
\$directory_selection = '$directory_selection';

###########################################################
# below are the options that you rarely need to change.
# but you may want to
###########################################################

# Default number of images to be displayed per page. "$paging_off" for no paging.
\$thumbnails_per_page = '$thumbnails_per_page';

# How many thumbnails do you want displayed per row?  Default is 4
\$thumbnails_per_row = $thumbnails_per_row;

# Width (in pixels) for thumbnail images (default is 100)
\$previewWidth = '$previewWidth';

# Quality of resized pictures (60-100) 
\$resize_quality = '$resize_quality';

# This is a list of known filetypes and the icon that should be show for each
# one.  If you wish to add your own, simply add another line with the format
# show and make sure your icon image (jpg or gif format) is in the site-images/
# folder
\%filetypes = (
    ".aiff", "aiff.jpg",
    ".avi" , "avi.jpg",
    ".bmp" , "bmp.jpg",
    ".mov" , "mov.jpg",
    ".mp3" , "mp3.jpg",
    ".mpg" , "mpg.jpg",
    ".mpeg", "mpg.jpg",
    ".pdf" , "pdf.jpg",
    ".php" , "php.jpg",
    ".ppm" , "ppm.jpg",
    ".ps"  , "ps.jpg",
    ".ra"  , "real.jpg",
    ".ram" , "real.jpg",
    ".rm"  , "real.jpg",
    ".sql" , "sql.jpg",
    ".txt" , "text.jpg",
    ".txt" , "text.jpg",
    ".pl"  , "text.jpg",
    ".cgi" , "text.jpg",
    ".ttf" , "ttf.jpg",
    ".tiff", "ttf.jpg",
    ".wav" , "wav.jpg");

# set this to no if you do not want the script to display the embedded exif
# data from your pictures.  Most digital cameras store the ISO,
# shutterspeed, etc here.
\$exif = "$exif";

# FULL path to jpegtran executable for lossless rotation
\$jpegtran = '$jpegtran';

# Want to watermark all of the pictures that you upload?  Provide the full
# path to a readable image format that image magick can read and it will
# be overlayed on the lower corner of all pictures uploaded.
\$watermark_file = '$watermark_file';

# Leave this here or it breaks the script
return 1;

TEXT
	close(CONFIG);

	$return .= "<b>Configuration saved in sitevariables.pl</b><br>";

	return $return;
}
sub print_config_form{
	my ($prettypicturenames_checked,$remove_leading_numbers_checked,$allowcomments_checked,$smiley_checked,$allowshutterfly_checked,$shadow_checked,$mag_on_thumbnails_checked,$email_comments_checked,$exif_checked);
	my $ok = "<font color=green><b>[OK]</b></font>";
	my $failed = "<font color=red><b>[FAILED]</b></font>";

	my $check_photoroot;
	if(-d "$photoroot"){
		$check_photoroot=$ok;
	}else{
		$check_photoroot="$failed<br>directory does not exist";
	}
	my $check_sendmail;
	if(-e "$sendmail"){
		$check_sendmail=$ok;
	}else{
		$check_sendmail=$failed;
	}
	my $check_dataroot;
	if(-d "$dataroot"){
		if(-w "$dataroot"){
			$check_dataroot=$ok;
		}else{
			$check_dataroot="$failed<br>directory exists but is not writable";
		}
	}else{
		$check_dataroot="$failed<br>directory does not exist";
	}
	my $check_dataurl;
	if($dataurl =~ /https?:\/\/.*/){
		$check_dataurl = $ok;
	}else{
		$check_dataurl = "$failed<br>does not look like a valid URL";
	}
	my $check_htmlroot;
	if($htmlroot=~ /https?:\/\/.*/){
		$check_htmlroot= $ok;
	}else{
		$check_htmlroot= "$failed<br>does not look like a valid URL";
	}
	if(-r $watermark_file){
		$watermark_file_checked = "$ok";
	}else{
		$watermark_file_checked = "$failed (optional but must checkout OK in order to work)";
	}



	if($prettypicturenames =~ /yes/i){$prettypicturenames_checked="checked";}
	if($remove_leading_numbers =~ /yes/i){$remove_leading_numbers_checked="checked";}
	if($allowcomments =~ /yes/i){$allowcomments_checked="checked";}
	if($indicate_comments =~ /yes/i){$indicate_comments_checked="checked";}
	if($smiley =~ /yes/i){$smiley_checked="checked";}
	if($allowshutterfly =~ /yes/i){$allowshutterfly_checked="checked";}
	if($allowclubphoto =~ /yes/i){$allowclubphoto_checked="checked";}
	if($shadow =~ /yes/i){$shadow_checked="checked";}
	if($mag_on_thumbnails =~ /yes/i){$mag_on_thumbnails_checked ="checked";}
	if($email_comments_to_admin =~ /yes/i){$email_comments_checked ="checked";}
	if($exif =~ /yes/i){$exif_checked ="checked";}

	if(-x $jpegtran){
		$jpegtran_checked = $ok;
	}else{
		$jpegtran_checked=$failed;
	}
	unless($resize_quality){$resize_quality="90";}

	my $directory_selection_text ="<table>";;
	my @directory_images = glob("$dataroot/site-images/directory*.*");
	foreach my $dir_img (@directory_images){
		unless($dir_img =~ /locked/i){
			$dir_img =~ s/$dataroot\/site-images\///i;
			$dir_img_locked = $dir_img;
			$dir_img_locked =~ s/(.*)\.(.*)/$1-locked.$2/;
			unless(-e "$dataroot/site-images/$dir_img_locked"){
				$dir_img_locked = "no locked image available";
			}
			if($dir_img eq $directory_selection){
				$checked = 'CHECKED';
			}else{
				$checked = "";
			}
			$directory_selection_text .= "
				<tr>
					<td><input type=\"radio\" name=\"directory_selection\" value=\"$dir_img\" $checked></td>
					<td><img src=\"$dataurl/site-images/$dir_img\" alt=\"$dir_img\"></td>
					<td><img src=\"$dataurl/site-images/$dir_img_locked\" alt=\"$dir_img_locked\"></td>
				</tr>
					";
		}
	}	
	$directory_selection_text .="</table>";;
	
	my $return .= "
	<br>
	<form method=post>
	<input type=hidden name=mode value=setup>
	<input type=hidden name=saveconfig value=yes>
	<table bgcolor=#000000 cellpadding=0 border=0 cellspacing=1><tr><td>
	<table width=600 bgcolor=#FFFFFF width=100% cellspacing=0>
	<tr bgcolor=#000000><td colspan=3 align=center><font color=white>sitevariables.pl configuration variables</font></td></tr>
	<tr><td>Title of your gallery</td><td><input type=text name=title value=\"$title\"></td><td></td></tr>

	<tr bgcolor=#000000><td colspan=3 align=center><font color=white>Directory Information</font></td></tr>
	<tr><td>Path to root directory containing your pictures</td><td><input type=text name=photoroot value=\"$photoroot\" size=40></td><td>$check_photoroot</td></tr>
	<tr bgcolor=#000000><td colspan=3 align=center height=1></td></tr>
	<tr><td>URL to root directory containing your pictures (should be the URL that points to the above directory)</td><td><input type=text name=htmlroot value=\"$htmlroot\" size=40></td><td>$check_htmlroot</td></tr>
	<tr bgcolor=#000000><td colspan=3 align=center height=1></td></tr>
	<tr><td>Path to root directory where we can store thumbnails, site-images/icons, descriptions, etc</td><td><input type=text name=dataroot value=\"$dataroot\" size=40></td><td>$check_dataroot</td></tr>
	<tr bgcolor=#000000><td colspan=3 align=center height=1></td></tr>
	<tr><td>URL to root directory where we can store thumbnails, site-images/icons, descriptions, etc (should be the URL to the above path)</td><td><input type=text name=dataurl value=\"$dataurl\" size=40></td><td>$check_dataurl</td></tr>


	<tr bgcolor=#000000><td colspan=3 align=center><font color=white>Operational Options</font></td></tr>
	<tr><td colspan=2>Eliminate spaces, weird characters, and extensions when displaying filenames?</td><td><input type=checkbox name=prettypicturenames $prettypicturenames_checked></td></tr>
	<tr><td colspan=2>Remove leading numbers from filenames when displaying?  <i>(This can be used to force certain pictures to the top of a gallery by putting a leading 0 in front or numbering them)</i>.</td><td><input type=checkbox name=remove_leading_numbers $remove_leading_numbers_checked></td></tr>
	<tr><td colspan=2>Allow visitors to post comments for each picture?</td><td><input type=checkbox name=allowcomments $allowcomments_checked></td></tr>
	<tr><td colspan=2>Place a small icon (<img src=\"$dataurl/site-images/comments.gif\">) next to thumbnails with comments?</td><td><input type=checkbox name=indicate_comments $indicate_comments_checked></td></tr>
	<tr><td colspan=2>Replace smilies with a picture of a smiley?</td><td><input type=checkbox name=smiley $smiley_checked></td></tr>
	<tr><td colspan=2>Allow visitors to order copies through integrated Shutterfly.com support?</td><td><input type=checkbox name=allowshutterfly $allowshutterfly_checked></td></tr>
	<tr><td colspan=2>Allow visitors to order copies through integrated ClubPhoto.com support?</td><td><input type=checkbox name=allowclubphoto $allowclubphoto_checked></td></tr>
	<tr><td colspan=2>Place a shadow around each pictures?</td><td><input type=checkbox name=shadow $shadow_checked></td></tr>
	<tr><td colspan=2>Place a magnifying glass over the corner of each thumbnail?</td><td><input type=checkbox name=mag_on_thumbnails $mag_on_thumbnails_checked></td></tr>
    <tr><td colspan=2>Thumbnails/Icons per page (number of thumbnails displayed per page of an album.  Leave blank or enter \"off\" for no paging.)</td><td><input type=text name=thumbnails_per_page value=\"$thumbnails_per_page\" size=4></td></tr>
	<tr><td colspan=2>Thumbnails/Icons per row (default of 4 should be fine)</td><td><input type=text name=thumbnails_per_row value=\"$thumbnails_per_row\" size=4></td></tr>
	<tr><td colspan=2>Width (in pixels) of generated thumbnails</td><td><input type=text name=previewWidth value=\"$previewWidth\" size=5></td></tr>
	<tr><td colspan=2>Quality of resized pictures (60-100).  Larger means better quality but takes up more space.</td><td><input type=text name=resize_quality value=\"$resize_quality\" size=5></td></tr>
	<tr><td colspan=2>Enable displaying of embedded EXIF camera data from pictures taken with supported cameras?</td><td><input type=checkbox name=exif $exif_checked></td></tr>
	<tr><td colspan=2 valign=top>Specify the FULL path to the jpegtran executable if you wish to use it for lossless jpeg rotation</td><td valign=top><input type=text name=jpegtran value=\"$jpegtran\"> $jpegtran_checked</td></tr>
	<tr><td colspan=2 valign=top>Do you want a watermark in the lower corner of all of your pics?  Specify the full path to an image that you would like to use as your watermark.  Leave this blank to disable.</td><td valign=top><input type=text name=watermark_file value=\"$watermark_file\"> $watermark_file_checked</td></tr>

	<tr bgcolor=#000000><td colspan=3 align=center><font color=white>Email options</font></td></tr>
	<tr><td>Email comments to admin?</td><td><input type=checkbox name=email_comments_to_admin $email_comments_checked></td></tr>
	<tr><td>Admin email address</td><td><input type=text name=admin_email value=\"$admin_email\" size=40></td><td></td></tr>
	<tr><td>Full path to sendmail (only needed if email is enabled)</td><td><input type=text name=sendmail value=\"$sendmail\" size=40></td><td>$check_sendmail</td></tr>

	<tr bgcolor=#000000><td colspan=3 align=center><font color=white>Look and Feel</font></td></tr>
	<tr><td colspan=3>NOTE: Most of the look and feel comes from the template.html file in the same directory as index.cgi.  This is a normal .html file with anything you like in it with a special tag as a plceholder for where you want the main content that this script generates.  Also, don't forget that you can drastically alter the look and feel of your site by specifying a font in your template file.  By placing style=\"font-family: Verdana;\" in the BODY tag of your template it will change the font of all text.</td></tr>
	<tr bgcolor=#000000><td colspan=3 align=center height=1></td></tr>
	<tr><td>Row color is used as the background for the title and other highighting when displaying the picture</td><td><input type=text name=rowcolor value=\"$rowcolor\" size=10></td><td bgcolor=$rowcolor>test</td></tr>
	<tr bgcolor=#000000><td colspan=3 align=center height=1></td></tr>
	<tr><td>background color for the table around each picture</td><td><input type=text name=descriptioncolor value=\"$descriptioncolor\" size=10></td><td bgcolor=$descriptioncolor>test</td></tr>
	<tr bgcolor=#000000><td colspan=3 align=center height=1></td></tr>
	<tr><td>Alternating colors for each row of comments.</td><td><input type=text name=commentcolor1 value=\"$commentcolor1\" size=10></td><td bgcolor=$commentcolor1>test</td></tr>
	<tr><td></td><td><input type=text name=commentcolor2 value=\"$commentcolor2\" size=10></td><td bgcolor=$commentcolor2>test</td></tr>

	<tr><td colspan=3><p>Select an image to use for the directories:<br>
	<font color=red>(To make your own images available, put them into $dataroot/site-images/ and name them directory-yourtext.gif and make a corresponding directory-yourtext-locked.gif.)</font><br>
	$directory_selection_text</p></td></tr>
	<tr><td colspan=3 align=center><input type=submit value=\"Save Configuration\"></td></tr>

	</table>
	</td></tr></table>
	</form>
	";

	return $return;
}
sub check_deps{
	my $return;
	my $check_im = `/usr/bin/perl -e "use Image::Magick" 2>&1`;
	my $check_ii = `/usr/bin/perl -e "use Image::Info" 2>&1`;
	my ($check_jpg,$check_png,$check_gif);
	my $ok = "<font color=green><b>[OK]</b></font>";
	my $failed = "<font color=red><b>[FAILED]</b></font>";

	if($check_im){$check_im="$failed<br>Could not load Image::Magick perl module.  If it is installed, your \@INC may not include the path to it.  Please check the FAQ for more information on adding a 'use lib \"/path/\"' line to the top of index.cgi and manage.cgi.  Even after you add the 'use lib' line this script will still say failed.  If you added the correct line you may safely ignore this warning.";}else{$check_im="$ok";}
	if($check_ii){$check_ii="$failed";}else{$check_ii="$ok";}

	$return .= "
	<table width=600 bgcolor=#000000 cellpadding=0 border=0 cellspacing=1><tr><td>
	<table bgcolor=#FFFFFF width=100% cellspacing=0 cellpadding=10>
	<tr bgcolor=#000000><td colspan=3 align=center><font color=white>Check For Requirements</font></td></tr>
	<tr>
		<td><a href=\"http://www.imagemagick.org\" target=newwindow>Image Magick</a></td>
		<td>needed for resizing images and overlaying watermarks</td>
		<td width=200>$check_im</td>
	</tr>
	<tr><td colspan=2>&nbsp;&nbsp;&nbsp;supports .jpg files</td><td>$check_jpg</td></tr>
	<tr><td colspan=2>&nbsp;&nbsp;&nbsp;supports .png files</td><td>$check_png</td></tr>
	<tr><td colspan=2>&nbsp;&nbsp;&nbsp;supports .gif files</td><td>$check_gif</td></tr>
	<tr>
		<td><a href=\"http://search.cpan.org/CPAN/authors/id/G/GA/GAAS/Image-Info-1.12.tar.gz\" target=newwindow>Image Info</a> (this perl module reqires <a href=\"http://search.cpan.org/CPAN/authors/id/G/GA/GAAS/IO-String-1.02.tar.gz\" target=newwindow>IO::String</a>)</td>
		<td>Needed to extract embedded EXIF data from images.  This provides information such as the camera model and settings.</td>
		<td>$check_ii</td>
	</tr>
	</table>
	</td></tr></table>
	";

	return $return;
}
sub printmenu{
	my $color_highlighted = "#FCFCFC";
	my ($color_setup,$color_manage,$color_tools);
	if($mode eq "setup"){
		$color_setup="bgcolor=$color_highlighted";
	}elsif($mode eq "manage"){
		$color_manage="bgcolor=$color_highlighted";
	}elsif($mode eq "tools"){
		$color_tools="bgcolor=$color_highlighted";
	}
	
	my $return ="
	<table width=80%>
	<tr>
	<td align=center $color_setup><a href=\"manage.cgi?mode=setup\">Setup and Configuration</a></td>
	<td align=center $color_manage><a href=\"manage.cgi?mode=manage\">Manage Albums and Pictures</a></td>
	<td align=center $color_tools><a href=\"manage.cgi?mode=tools\">Tools</a></td>
	</tr></table>";
	return $return;
}

sub uploadpictures {
	my $return;	
	my $album = $cur->param("album");
	if($album =~ /\.\./) {dienice("Please do NOT do that!");}
	my $basedir = "$photoroot/$album";

	## Do you wish to allow all file types?  yes/no (no capital letters)
	$allowall = "yes";

	## If the above = "no"; then which is the only extention to allow?
	## Remember to have the LAST 4 characters i.e. .ext
	$theext = ".jpg";

	$onnum = 1;
	while ($onnum != 11) {
		my $file = $cur->param("FILE$onnum");
		if ($file ne "") {
			my $fileName = $file; 
			$fileName =~ s!^.*(\\|\/)!!; 
			$newmain = $fileName;
			$fileName =~ s/&/and/ig;
			$fileName =~ s/#/no/ig;
			$fileName =~ s///ig;
			$fileName =~ s///ig;
			$fileName =~ s/[\!\@\#\$\%\^\&\|\\\/*\'\"\`]//ig;
			#if (($fileName =~ /\.jpg$/i)||($fileName =~ /\.gif$/i)||($fileName =~ /\.png$/i)||($fileName =~ /\.zip$/i)) { 
			if (1) { 
				open (OUTFILE, ">$basedir/$fileName"); 
				#$return .= "<font color=green>Uploaded file : $fileName</font><br>";
				while (my $bytesread = read($file, my $buffer, 1024)) { 
					binmode OUTFILE;
					print OUTFILE $buffer; 
				} 
				close (OUTFILE); 
				if($fileName =~ /\.zip$/i){
					`unzip "$basedir/$fileName" -d "$basedir/"`;
					#$return .= "$fileName was unziped using unzip $basedir/$fileName -d $basedir/";
					unlink("$basedir/$fileName");
				}
			}else{
				$return .= "<font color=red>$fileName was not uploaded.  Only .jpg, .png, .gif, and .zip files allowed.</font><br>";
			}
		}
		$onnum++;
	}

	return $return;
}



