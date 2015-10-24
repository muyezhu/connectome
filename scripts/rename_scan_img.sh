#!/bin/bash
DIR='/ifs/loni/faculty/dong/mcp/Scans/PIR/SW150211-05A/'
for f in $DIR*;
do
    echo $f
    ff=${f##*/}
    case=${ff%%_0*}
	case=$case"A"
	level=${ff#*_0}
	echo $case'_'$level
	mv $f $DIR$case'_'$level;
done

# This script is used to rename scanned image files to conform to outspector requirements.