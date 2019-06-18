#!/bin/bash

for dir in $(ls -d */)
	do
	
	cd $dir
	for file in *.png
	do
		../../../../build/auto_segmenter b $file
	done
	cd ..
done
