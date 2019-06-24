#!/bin/bash

for dir in $(ls -d */)
	do
	
	cd $dir
	for file in *.png
	do
		../../../../build/auto_segmenter w $file
	done
	cd ..
done
