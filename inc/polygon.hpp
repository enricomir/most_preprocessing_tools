#ifndef POLYGON_HPP
#define POLYGON_HPP

#include <vector>
#include <opencv2/core.hpp>
#include <iostream>
#include "simplepoint.hpp"

class Polygon {
    public:
        std::vector<SimplePoint> points;

        enum class FileType {
            FILE_POF,
            FILE_WKT
        };
        Polygon(std::istream& input_file, FileType ftype = FileType::FILE_POF);
        Polygon() {};
        

        void save(std::ostream& output_file, FileType ftype = FileType::FILE_POF);

        void get_feature_points(); //TODO: return vector of size_t

        Polygon get_slice(size_t begin, size_t end);

        //Static methods
        static double hausdorff(const Polygon& pol1, const Polygon& pol2);
        static double chamfer(const Polygon& pol1, const Polygon& pol2);
        static double polis(const Polygon& pol1, const Polygon& pol2);
};

#endif //POLYGON_HPP