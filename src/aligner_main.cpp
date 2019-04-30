#include <iostream>
#include <fstream>
#include "polygon.hpp"
#include "aligner.hpp"

int main(int, char* argv[]) {
	std::ifstream if1(argv[1]);
	Polygon p1(if1);
	std::ifstream if2(argv[2]);
	Polygon p2(if2);

	Aligner::translate_centroids(p1, p2);

	std::ofstream of1(argv[3]);
	p1.save(of1);

	return 0;
}
