#!/bin/bash
CASE='/ifs/loni/faculty/dong/mcp/registration2d/vsi2tiff/uncurated/SW150116-04A/'
for f in $CASE*;
do 
    echo $f
	figure=${f##*/}
	figure=${figure%%.*}
	figure=$figure'.tif'
	mv $f'/Layer0/CH.tif' $CASE$figure;
done;

# This script is used to rename multi-channel tif output from VS-DESKTOP 