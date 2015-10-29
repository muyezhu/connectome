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
    # change format from jpg to tif
	$CONVERT $img $imgname'.tif'
	# substitute white from exports to black. 
	$CONVERT -fill black -opaque white $imgname'.tif' $imgname'.tif' 
	# substitute yellow from exports to black
	$CONVERT -fill black -fuzz 50% -opaque "rgb (255, 255, 50)" $imgname'.tif' $imgname'.tif' 
	# mask out highlighted edges
	$CONVERT -fill white -fuzz 5% +opaque black $imgname'.tif' $imgname'_mask.tif'    # generate mask
	# erode mask. transparent white. composite with scan image.
	# convert to gray scale and normalize
	$CONVERT -colorspace gray -normalize $imgname'.tif' $imgname'.tif' 
	# apply erosion and threshold
	$CONVERT -morphology Erode Ring:4,4.5 $imgname'.tif' $imgname'_erode.tif'
	$CONVERT -threshold 50% -compress None $imgname'_erode.tif' $imgname'_erode_th.tif' 
	# split left and right halves.
done
