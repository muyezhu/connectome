#!/bin/bash
CONVERT='/usr/local/ImageMagick-6.7.8-2_64bit/bin/convert'
IDENTIFY='/usr/local/ImageMagick-6.7.8-2_64bit/bin/identify'
DIR='/ifs/loni/faculty/dong/mcp/muye/projects/cell_count/test/'
MOGRIFY='/usr/local/ImageMagick-6.7.8-2_64bit/bin/mogrify'

# the script takes rgb rabies scan image for preprocessing.
# (1) remove scale, magnification information output by VS120
# (2) adjust level and transform to gray scale
# (3) perform erosion with ring struct, where axons are more strongly eroded than cell bodies
# (4) threshold into a binary image, to be the input to cell_count.py
for img in $DIR*"jpg";
do
    imgname=${img%%.*}
	# remove magnification and scale output from scope
	$CONVERT -fill black -opaque white -fuzz 50% -opaque "rgb (255, 255, 50)" -colorspace gray -compress None $img $imgname'.tif' 
	# level adjustment, transform to gray scale
    mean=$($IDENTIFY -format %[fx:255*mean] $imgname'.tif')
    mean=${mean%%.*}
    std=$($IDENTIFY -format %[standard-deviation] $imgname'.tif')
    std=${std%%.*}
    std=$(($std/255))
    level=$(($(($mean+6*$std))*100/255))
	$CONVERT -level $level%,100% -colorspace gray $imgname'.tif' $imgname'.tif'
    # obtain representation of cell bodies.
    # apply erosion and threshold
	$CONVERT -morphology Erode Ring:4,4.5 -threshold 10% $imgname'.tif' $imgname'_erode.tif'
done













