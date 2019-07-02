#include "polygon.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "Error. Usage: ./pof2wkt pof_file\n";
		return 1;
	}

	std::ifstream in(argv[1]);
	Polygon p(in);

	p.save(std::cout, Polygon::FileType::FILE_WKT);
	return 0;
}

