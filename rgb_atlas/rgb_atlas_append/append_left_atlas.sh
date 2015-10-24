#!/bin/bash
DIR='/ifs/loni/faculty/dong/mcp/muye/projects/rgb_atlas/rgb_atlas_append/'
NEWDIR='/ifs/loni/faculty/dong/mcp/muye/rgb_atlas_test/'
CONVERT='/usr/local/ImageMagick-6.7.8-2_64bit/bin/convert'
MOGRIFY='/usr/local/ImageMagick-6.7.8-2_64bit/bin/mogrify'
for img in $DIR*"rgb"*"tif";
do
    imgroll=${img%%.*}'-roll.tif'
	imgshave=${imgroll%%.*}'-shave.tif'
    imgflop=${imgshave%%.*}'-flop.tif'
	imgappendleft=${img%%.*}'-append.tif'
	./read_offset $img
	val=$(cat "offset.txt")
	# roll
	$CONVERT -roll +$val+0 $img $imgroll
	# shave
	$CONVERT -composite $imgroll $DIR"mask.tif" $imgshave
	# flop
    $CONVERT -flop $imgshave $imgflop
	# transparent white in flop
	$MOGRIFY -transparent white $imgflop 
	# composite for final image
	$CONVERT -composite $imgroll $imgflop $imgappendleft
	echo "composite done"
	mv $imgappendleft $NEWDIR;
done

# This script is used to flop the right side only rgb atlas, and compose the two sides to a complete atlas image.