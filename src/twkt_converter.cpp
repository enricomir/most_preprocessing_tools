#include "polygon.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
	if (argc < 4) {
		std::cout << "Error. Usage: ./twkt_converter <timestamp> <source file> <target file>\n";
		return 1;
	}

	std::ifstream in(argv[2]);
	std::ostringstream buf;
	std::ofstream out(argv[3]);
	double timestamp = std::stod(argv[1]);

	Polygon p(in);

	p.save(buf, Polygon::FileType::FILE_WKT);

	out << "SNAPSHOT(" << timestamp << ", " << buf.str() << ")\n";

	return 0;
}
