#ifndef POLYGON_HPP
#define POLYGON_HPP

#include <vector>
#include <opencv2/core.hpp>
#include <iostream>
#include <geos_c.h>
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
        

        void save(std::ostream& output_file, FileType ftype = FileType::FILE_POF) const;

        void get_feature_points() const; //TODO: return vector of size_t

        Polygon get_slice(size_t begin, size_t end) const;

        //Static methods
        static double hausdorff(const Polygon& pol1, const Polygon& pol2);
        static double chamfer(const Polygon& pol1, const Polygon& pol2);
        static double polis(const Polygon& pol1, const Polygon& pol2);
				static double jaccard(const Polygon& pol1, const Polygon& pol2);
				static double frechet(const Polygon& pol1, const Polygon& pol2); //Not implemented
				//Needs GEOS 3.8+, Linux default is 3.6. Harder to get dependencies.
			private:
				GEOSGeom get_GEOSGeom(GEOSContextHandle_t geos) const;
				void print(GEOSContextHandle_t geos, char* prefix = nullptr);
};

#endif //POLYGON_HPP
