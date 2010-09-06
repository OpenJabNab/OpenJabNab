#!/bin/bash

for h in {0..23}
do
	if [ ! -d $h ]; then
		mkdir $h;
	fi
	cd $h;
	for i in {1..9}
	do
		wget http://broad.violet.net/broad/config/clock/fr/$h/$i.mp3
	done
	cd ..
	
done

