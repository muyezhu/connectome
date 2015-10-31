#!/bin/bash
CONVERT='/usr/local/ImageMagick-6.7.8-2_64bit/bin/convert'
IDENTIFY='/usr/local/ImageMagick-6.7.8-2_64bit/bin/identify'
DIR='/ifs/loni/faculty/dong/mcp/muye/projects/cell_count/images/'

# the script takes rgb rabies scan image for preprocessing.
# (1) remove scale, magnification information output by VS120
# (2) convert to a normalized gray scale tif
# (3) perform erosion with ring struct, where axons are more strongly eroded than cell bodies
# (4) threshold into a binary image, to be the input to cell_count.py
for img in $DIR*;
do
    imgname=${img%%.*}
	
	# remove magnification and scale output from scope
	$CONVERT -fill black -opaque white -fuzz 50% -opaque "rgb (255, 255, 50)" -compress None $img $imgname'.tif' 
	# level adjustment, transform to gray scale
	$CONVERT -level 60%,100% -colorspace gray $imgname'.tif' $imgname'_level.tif' 
	# obtain representation of cell bodies.
	# apply erosion and threshold
	$CONVERT -morphology Erode Ring:4,4.5 -threshold 10% $imgname'_level.tif' $imgname'_erode.tif'
	# split left and right halves.
	
	# mask out highlighted edges
	y=$($IDENTIFY -format "%h" $imgname'.tif')  # image height
	halfy=$(($y/2))
	# floodfill areas surrounding sections with rgb(0,0,255)
	$CONVERT -fill "rgb(0,0,255)" -floodfill +10+$halfy black $imgname'.tif' $imgname'_mask.tif'    
	# fill areas within section with white
	$CONVERT -fill white +opaque "rgb(0,0,255)" -compress None -colorspace gray $imgname'_mask.tif' $imgname'_mask.tif'   
	# erode mask. transparent white (note to maintain 8 bit depth). composite with scan image.
	
	
done
