#ffmpeg -i IMG_%04d.JPG -b 2000k -vf scale=640:-1  $1.mpg
#ffmpeg -i IMG_%04d.JPG -b 4000k -vf "crop=2936:2200:1168:408,scale=1024:-1"  $1.mpg
ffmpeg -i IMG_%04d.JPG -b 4000k -vf scale=1024:-1  $1.mpg
