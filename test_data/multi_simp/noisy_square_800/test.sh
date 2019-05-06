#!/bin/sh

for i in $(LC_NUMERIC=en_US seq 0.05 0.05 0.95); do
	mkdir simp_$i
	cd simp_$i
	cp ../sq1.pof .
	cp ../sq2.pof .
	~/most/most_preprocessing_tools/build/msimp p $i sq1.pof sq2.pof 

	~/most/most_preprocessing_tools/build/msimp $i sq1.pof
	~/most/most_preprocessing_tools/build/msimp $i sq2.pof

	cd ..
	echo comp h simp_$i/simp_sq1.pof simp_$i/simp_sq2.pof >> ../commands_s.txt
	echo comp h simp_$i/msimp_sq1.pof simp_$i/msimp_sq2.pof >> ../commands_m.txt
done;

../../../build/terminal < commands_s.txt
../../../build/terminal < commands_m.txt

rm commands_s.txt
rm commands_m.txt

