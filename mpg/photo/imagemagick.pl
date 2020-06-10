##############################################################################
# Image Magic subroutines                                                   ##
#                                                                           ##
# by Michael Spiceland 
# licensed under the GPL.  This file should include the GPL LICENSE FILE.
# last updated with My Photo Gallery version 4.0
##############################################################################

use Image::Magick;

##############################################################################
# Resize an image to have a maximum of the given width.  If the width is
# less than what is given, it stays the same.
#
# $_[0] = full path to input image
# $_[1] = full path to output image 
# $_[2] = max width to allow pictures to be
# $_[3] = quality for resized jpeg file (1-100)
##############################################################################
sub resize($$$$){
	my $return;
	
	my($imageToScale) = Image::Magick->new;
	my($itemToDisplay) = "$_[0]";
	#$return .= "imtemToDisplay=$itemToDisplay\n";
	my($x) = $imageToScale->Read($itemToDisplay);
	warn "$x" if "$x";
	my($xSize, $ySize) = $imageToScale->Get('width', 'height');
	#$return .= "xSize=$xSize\nySize=$ySize\n";
	my $scaleFactor;
	my $new_width = $xSize;
	my $new_height = $ySize;

	if($xSize > $ySize){
		$scaleFactor = $xSize/$_[2];
		if($xSize > $_[2]){
			$new_width =$_[2];
			$new_height = $ySize/$scaleFactor;
		}
	}else{
		$scaleFactor = $ySize/$_[2];
		if($ySize > $_[2]){
			$new_height =$_[2];
			$new_width = $xSize/$scaleFactor;
		}
	}
	#return .= "\n$xSize x $ySize => $new_width x $new_height\n";
	$x = $imageToScale->Scale(width=>$new_width,height=>$new_height); 
	if($_[3] =~ /^[0-9]{1,3}$/){
		$x = $imageToScale->Set(quality=>$_[3]); 
	}
	warn "$x" if "$x";
	# get rid of EXIF data (including thumbnail) to save space
	$x = $imageToScale->Profile(name => '*', profile => '');
	warn "$x" if "$x";
	$x = $imageToScale->Write("$_[1]");
	#$return .= "imageToScale=$_[1]\n";
	warn "$x" if "$x";
	undef $imageToScale;

	return $return;
}

##############################################################################
# Rotate an image with the given number of degrees
#
# $_[0] = full path to input image
# $_[1] = degrees of rotation
# $_[2] = quality for resized jpeg file (1-100)
##############################################################################
sub rotate($$$){
	my $return;
	my $status;

	# if available, rotate with jpegtan
	if(-x $jpegtran){
		`$jpegtran -rotate $_[1] -copy all -outfile temp.jpg \"$_[0]\"`;
		rename("temp.jpg","$_[0]");
	}else{
	
		my $image = Image::Magick->new;

		$status = $image->Read("$_[0]");
		$return .= $status if $status;
		$status = $image->Set(quality=>$_[2]); 
		$return .= $status if $status;
		$image->Rotate(degrees=>$_[1]);
		$status = $image->Write("$_[0]");
		$return .= $status if $status;
		undef $image;
	}
	return $return;
}

##############################################################################
# overlay image with a logo, etc
#
# $_[0] = full path to input image
# $_[1] = full path to output image
# $_[2] = full path to watermark to put on top of image
##############################################################################
sub overlay($$$){
    if ($_[2] eq "") {
        return;
    }

	my($imageToScale) = Image::Magick->new;
	my($itemToDisplay) = "$_[0]";
	my($x) = $imageToScale->Read($itemToDisplay); # read in the picture
	warn "$x" if "$x";
	my $image_overlay = Image::Magick->new();
        $status = $image_overlay->Read("$_[2]");
	$status = $imageToScale->Composite(
        	compose=>Over,
                image=>$image_overlay,
                gravity=>SouthEast
	);
	#print "Error: $status" if "$status";
        undef $image_overlay;
	$x = $imageToScale->Write("$_[1]");
	warn "$x" if "$x";
	undef $imageToScale;
}

##############################################################################
# overlay image with a logo, etc
#
# $_[0] = full path to directory image
# $_[1] = full path to output image
# $_[2] = full path to image to put on top of image
##############################################################################
sub overlay_directory($$$){
	my($imageToScale) = Image::Magick->new;
	my($itemToDisplay) = "$_[0]";
	my($x) = $imageToScale->Read($itemToDisplay); # read in the picture
	warn "$x" if "$x";
	my $image_overlay = Image::Magick->new();
   $status = $image_overlay->Read("$_[2]");
	my($xSize, $ySize) = $image_overlay->Get('width', 'height');
	if($xSize > $ySize){ # landscape
		$newWidth = $ySize/4*3; # 1/2 width
		$status = $image_overlay->Crop(width=>$newWidth,height=>$ySize,x=>$newWidth/2);
	}
	$status = $image_overlay->Scale(width=>47,height=>71);
	$status = $imageToScale->Composite(
        	compose=>Over,
         image=>$image_overlay,
         gravity=>NorthWest,
			x=>4,
			y=>12,
	);
	#print "Error: $status" if "$status";
        undef $image_overlay;
	$x = $imageToScale->Write("$_[1]");
	warn "$x" if "$x";
	undef $imageToScale;
}


return 1;
