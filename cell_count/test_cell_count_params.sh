# test parameters
# $CONVERT -fill black -opaque white -fuzz 50% -opaque "rgb (255, 255, 50)" -depth 8 -compress None $img $imgname'.tif' 
# try values to use in -level
# return image mean gray value in 0, 255 range
mean=$($IDENTIFY -format %[fx:255*mean] gray.tif)
mean=${mean%%.*}
# return image std in 0, 65525 range (2^16)
std=$($IDENTIFY -format %[standard-deviation] gray.tif)
# bash doesn't support floating poitn division
std=${std%%.*}
std=$(($std/255))
# generate level adjusted image with mean + [2, 6]sigma
for i in 0 {2..6};
do
    # calculate percent to use in -level: u + 2 sigma
    level=$(($(($mean+$i*$std))*100/255))
    $CONVERT -level $level%,100% gray.tif "gray_"$i"sigma.tif"
    $CONVERT -morphology Erode Ring:4,4.5 "gray_"$i"sigma.tif" "gray_"$i"sigma_erode.tif"
    $CONVERT -threshold 10% "gray_"$i"sigma_erode.tif" "gray_"$i"sigma_thres.tif"
done;