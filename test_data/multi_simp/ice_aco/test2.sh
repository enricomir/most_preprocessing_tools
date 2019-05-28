#!/bin/sh

for i in $(LC_NUMERIC=en_US seq 0.9 0.05 0.9); do
	mkdir simp2_$i
	cd simp2_$i
	cp ../*.pof .
	../../../../build/msimp p $i 2.pof 3.pof
	../../../../build/msimp p $i 4.pof 5.pof 
	../../../../build/msimp p $i 6.pof 7.pof
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

