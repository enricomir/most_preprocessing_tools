#!/bin/sh

for i in $(LC_NUMERIC=en_US seq 0.05 0.05 0.95); do
	mkdir simp_$i
	cd simp_$i
	cp ../*.pof .
	../../../build/msimp p $i 1.pof 2.pof 3.pof 4.pof 5.pof 6.pof 7.pof 8.pof
	../../../build/msimp $i 1.pof
	../../../build/msimp $i 2.pof
	../../../build/msimp $i 3.pof
	../../../build/msimp $i 4.pof
	../../../build/msimp $i 5.pof
	../../../build/msimp $i 6.pof
	../../../build/msimp $i 7.pof
	../../../build/msimp $i 8.pof
	cd ..

	echo comp h simp_$i/simp_1.pof simp_$i/simp_2.pof >> commands_s.txt
	echo comp h simp_$i/msimp_1.pof simp_$i/msimp_2.pof >> commands_m.txt
done;

../../../build/terminal < commands_s.txt
../../../build/terminal < commands_m.txt

rm commands_s.txt
rm commands_m.txt
