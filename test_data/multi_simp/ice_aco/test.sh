#!/bin/sh

for i in $(LC_NUMERIC=en_US seq 0.9 0.05 0.9); do
	mkdir simp_$i
	cd simp_$i
	cp ../*.pof .
	../../../../build/msimp p $i 1.pof 2.pof
	../../../../build/msimp p $i 3.pof 4.pof 
	../../../../build/msimp p $i 5.pof 6.pof
	../../../../build/msimp p $i 7.pof 8.pof
	../../../../build/msimp $i 1.pof
	../../../../build/msimp $i 2.pof
	../../../../build/msimp $i 3.pof
	../../../../build/msimp $i 4.pof
	../../../../build/msimp $i 5.pof
	../../../../build/msimp $i 6.pof
	../../../../build/msimp $i 7.pof
	../../../../build/msimp $i 8.pof
	cd ..
done;

