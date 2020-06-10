############################################################
# My Photo Gallery configuration file
# 
# Please read this file completely and edit the variables
# to suit your site.  It just takes a minute and makes sure
# you are up and running smoothly. 
# 
# config file updated using manage script on Thu Jun  3 22:32:04 2004
############################################################

# First, lets get the hard stuff out of the way.  Let's tell
# the script where you installed it.

#Title of your photo gallery
$title = "My Gallery";

# This is the path to the root directory containing your albums.
# Each album should be stored as a subdirectory under this. 
# please no trailing /
#example - $photoroot ='/var/www/html/photo';
$photoroot ='/home/user/www/photo';

#URL that points to $photoroot.  This should be how the browser gets to it.
#please no trailing /
#example - $htmlroot = 'http://pancakes.darktech.org/photo';
$htmlroot = 'http://yourdomain.com/photo';

#Path to a directory were we can store the thumbnails, site-images/icons,
#descriptions, etc.
$dataroot ='/home/user/www/photodata';

#URL that points to a directory for storing the thumbnails, site-images/icons,
#descriptions, etc.  This should be a directory that your webserver can serve
#images from (normally under www/ or html/)
$dataurl = 'http://yourdomain.com/photodata';

$prettypicturenames = 'yes'; # eliminate spaces and drop off extensions?
$remove_leading_numbers = 'yes'; # remove leading numbers from filenames?
$allowcomments = 'yes'; # allow comments on each pictures?
$indicate_comments = 'yes'; # put a small icon next to thumbnails with comments?
$smiley = 'yes'; # replace things like :-) with a smiley face?
$allowshutterfly = ''; # enable visitors to order prints via shutterfly.com?
$allowclubphoto = ''; # enable visitors to order prints via clubphoto.com?
$shadow = ''; # shadow around picture? (only works with white background)
$mag_on_thumbnails = 'yes'; # overlay thumbnails with "zoom" icon?

# Set this to YES if you would like your users to be able to post comments
# on each picture.  The comments will be stored in plain text files in
# a subdirectory in each album called comments/

# Set this to YES to have each comment emailed to you when it is placed.
# In order for this to work properly, you must also specify an email
# address and path to your sendmail
$email_comments_to_admin = '';
$admin_email = 'user@domain.com';
$sendmail = '/usr/lib/sendmail';

# This is a list of all the discriptions used to as verbs for the comments
# I suggest leaving them how they are :-)
@says=("says","blurted","screamed","burped","commented","thought","exclaimed","hinted");

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
$rowcolor         = '#B6B6B6';
# descriptioncolor is the background color for the table around each picture
$descriptioncolor = '#CCCCCC';
# alternating colors for rows of comments
$commentcolor1 = "#FFFFFF";
$commentcolor2 = "#edf2f4";
$directory_selection = 'directory-simpleblue.gif';

###########################################################
# below are the options that you rarely need to change.
# but you may want to
###########################################################

# Default number of images to be displayed per page. "" for no paging.
$thumbnails_per_page = '16';

# How many thumbnails do you want displayed per row?  Default is 4
$thumbnails_per_row = 4;

# Width (in pixels) for thumbnail images (default is 100)
$previewWidth = '100';

# Quality of resized pictures (60-100) 
$resize_quality = '95';

# This is a list of known filetypes and the icon that should be show for each
# one.  If you wish to add your own, simply add another line with the format
# show and make sure your icon image (jpg or gif format) is in the site-images/
# folder
%filetypes = (
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
    ".pl"  , "text.jpg",
    ".cgi" , "text.jpg",
    ".ttf" , "ttf.jpg",
    ".tiff", "ttf.jpg",
    ".wav" , "wav.jpg");

# set this to no if you do not want the script to display the embedded exif
# data from your pictures.  Most digital cameras store the ISO,
# shutterspeed, etc here.
$exif = "yes";

# FULL path to jpegtran executable for lossless rotation
$jpegtran = '/usr/bin/jpegtran';

# Want to watermark all of the pictures that you upload?  Provide the full
# path to a readable image format that image magick can read and it will
# be overlayed on the lower corner of all pictures uploaded.
$watermark_file = '';

# Leave this here or it breaks the script
return 1;

