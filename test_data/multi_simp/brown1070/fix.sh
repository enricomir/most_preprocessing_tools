#!/bin/bash

for i in *.pgm
	do
		DIR=$(echo "$i" | cut -c1-3)
		mkdir "$DIR"
		mv "$i" "$DIR/$i"
done
