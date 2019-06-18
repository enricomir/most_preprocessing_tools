#!/bin/bash

for dir in $(ls -d */)
	do
	cd $dir

	declare -a array=$(ls *.pof)
	../../../../build/msimp p 0.5 $(ls *.pof)
	for file in $array
	do
		../../../../build/msimp 0.5 $file
	done
	cd ..
done
