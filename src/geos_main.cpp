#include <iostream>
#include <geos_c.h>
#include "polygon.hpp"
#include <fstream>

int main() {
    std::ifstream i("0.pof");
    Polygon p1(i, Polygon::FileType::FILE_POF);

    p1.get_feature_points();

    return 0;
    
}