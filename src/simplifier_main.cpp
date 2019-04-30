#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

const double RATIO = 10.0;
using namespace cv;
int gui_area;

const char *W_NAME = "Simplification";
std::vector<Point> points;
std::vector<std::vector<Point>> polys;

Mat src;

/** @brief Simplifies a polygon using Visvalingam Algorithm.

The function simplify simplifies a given polygin (vector of points) using
Visvalingam algorithm in Line generalisation by repeated elimination of the
smallest area.

@param origin Original polygon.
@param min_area Minimun area in square pixels. Triangles with less than this
area are removed.
@return Polygon (vector of points) simplified.
 */
std::vector<Point> simplify(const std::vector<Point> &origin, float min_area) {
  // We will only be dealing with area squared to simplify computational
  // resources
  min_area = min_area * min_area;

  // Every triangle will be defined by their first point id. The vertexes are
  // P(i), P(i+1), P(i+2).
  std::vector<Point> reduced = origin;

  // Create 2 new points at the end to close triangles
  reduced.push_back(reduced[0]);
  reduced.push_back(reduced[1]);

  for (unsigned int i = 0;
       i < (reduced.size() -
            2);) { // Increment will be at end of loop due to logic needed

    float dx01, dx02, dx12, dy01, dy02,
        dy12;               // DcAB = distance between A and B in coord c
    float d01, d02, d12, p; // p = semiperimeter

    dx01 = reduced[i + 0].x - reduced[i + 1].x;
    dy01 = reduced[i + 0].y - reduced[i + 1].y;
    d01 = std::sqrt(dx01 * dx01 + dy01 * dy01);

    dx02 = reduced[i + 0].x - reduced[i + 2].x;
    dy02 = reduced[i + 0].y - reduced[i + 2].y;
    d02 = std::sqrt(dx02 * dx02 + dy02 * dy02);

    dx12 = reduced[i + 1].x - reduced[i + 2].x;
    dy12 = reduced[i + 1].y - reduced[i + 2].y;
    d12 = std::sqrt(dx12 * dx12 + dy12 * dy12);

    p = (d01 + d02 + d12) / 2;

    float area_sq = p * (p - d01) * (p - d02) * (p - d12);

    /*std::cout << "Triangle (" << reduced[i + 0].x << ", " << reduced[i + 0].y
              << ") (" << reduced[i + 1].x << ", " << reduced[i + 1].y << ") ("
              << reduced[i + 2].x << ", " << reduced[i + 2].y
              << ") "
                 "with area "
              << area_sq << std::endl;

    if (area_sq == 0) {
      std::cout << "ERROR area 0. d01 d02 d12 = " << d01 << ", " << d02 << ", "
    << d12 << std::endl; std::cout << "ERROR p = " << p << std::endl;
    }*/

    // if the area is smaller than the min acceptable, remove and
    if (area_sq < min_area) {
      reduced.erase(reduced.begin() +
                    (i + 1) % (reduced.size() - 2)); // Erase the point

      i = std::max(0, static_cast<int>(i) -
                          2); // We might need to recalc 2 points back

      reduced.pop_back();
      reduced.pop_back();
      reduced.push_back(reduced[0]);
      reduced.push_back(reduced[1]);
      // Either way, current point is a new point, so we won't iterate.
    } else { // Otherwise, just loop as usual
      ++i;
    }
  }

  reduced.pop_back();
  reduced.pop_back();

  return reduced;
}

std::vector<Point> simplify_n(const std::vector<Point>& origin, size_t to_remove) {
  std::vector<Point> reduced = origin;
	for (size_t i = 0; i < to_remove; ++i) {
		float min_area = std::numeric_limits<float>::max();
		size_t to_remove = 0;

		reduced.push_back(reduced[0]);
		reduced.push_back(reduced[1]);

		for (unsigned int i = 0;i < (reduced.size() -	2); ++i) {

			float dx01, dx02, dx12, dy01, dy02,
						dy12;               // DcAB = distance between A and B in coord c
			float d01, d02, d12, p; // p = semiperimeter

			dx01 = reduced[i + 0].x - reduced[i + 1].x;
			dy01 = reduced[i + 0].y - reduced[i + 1].y;
			d01 = std::sqrt(dx01 * dx01 + dy01 * dy01);

			dx02 = reduced[i + 0].x - reduced[i + 2].x;
			dy02 = reduced[i + 0].y - reduced[i + 2].y;
			d02 = std::sqrt(dx02 * dx02 + dy02 * dy02);

			dx12 = reduced[i + 1].x - reduced[i + 2].x;
			dy12 = reduced[i + 1].y - reduced[i + 2].y;
			d12 = std::sqrt(dx12 * dx12 + dy12 * dy12);

			p = (d01 + d02 + d12) / 2;

			float area_sq = p * (p - d01) * (p - d02) * (p - d12);

			// if the area is smaller than the current min area acceptable, mark as to remove
			if (area_sq < min_area) {
				to_remove = i;
				min_area = area_sq;
			}

      reduced.erase(reduced.begin() +
                    (to_remove + 1) % (reduced.size() - 2)); // Erase the point
			reduced.pop_back();
			reduced.pop_back();
		}
	}
	return reduced;
}

void recalcTolerance(int, void *) {
	Mat poly;
	src.copyTo(poly);
	polys.clear();
	polys.push_back(points);
	drawContours(poly, polys, 0, Scalar(0, 255, 0));

	std::vector<Point> simplified;
	simplified = simplify(points, gui_area / RATIO);
	polys.push_back(simplified);

	std::cout << "Simplified " << points.size() << " points with "
		<< gui_area / RATIO << " tolerance. Result: " << simplified.size()
		<< "points." << std::endl;

	drawContours(poly, polys, 1, Scalar(0, 0, 255));
	imshow(W_NAME, poly);
}

int main(int argc, char *argv[]) {
	if (argc != 3 && argc != 4 && argc != 5) {
		std::cout << "Wrong usage! Correct usages:\n"
			"  ./simplifier <source points file (.pof)> <source image>\n"
			"  ./simplifier <source points file (.pof)> <step> "
			"<iterations> for chart generation\n"
			"  ./simplifier <source points file (.pof)> <output file "
			"(.pof)> t <tolerance> for auto simplification\n";
		exit(1);
	}

	std::fstream fs(argv[1], std::fstream::in);

	if (!fs.is_open()) {
		std::cout << "Error, could not load file " << argv[1] << std::endl;
		exit(2);
	}

	int x, y;
	while (fs >> x >> y) {
		points.emplace_back(x, y);
	}

	std::string fname(argv[1]);
	fname.pop_back();
	fname.pop_back();
	fname.pop_back();
	fname.pop_back();

	if (argc == 3) {
		src = imread(argv[2]);

		namedWindow(W_NAME, WINDOW_NORMAL);
		createTrackbar("Tolerance (px squared*10)", W_NAME, &gui_area, 2000,
				recalcTolerance);
		recalcTolerance(0, nullptr);

		char c;
		while ((c = waitKey()) != 'q') {
			if (c == 's') {
				std::vector<Point> simp;
				simp = simplify(points, gui_area / RATIO);
				std::fstream fs2(fname + "_simplified.pof",
						std::fstream::out | std::fstream::trunc);

				for (Point p : simp) {
					fs2 << p.x << " " << p.y << "\n";
				}
			}
		}
	} else if (argc == 4) {
		std::fstream fs2(fname + "_simplification_chart.csv",
				std::fstream::out | std::fstream::trunc);

		unsigned int step = std::stoi(argv[2]);
		unsigned int iterations = std::stoi(argv[3]);

		fs2 << "0 " << points.size() << "\n";
		for (float f = step; f < (step * iterations); f += step) {
			std::vector<Point> simp;
			simp = simplify(points, f);
			fs2 << f << " " << simp.size() << "\n";
		}
	} else if (argc == 5) {
		float n = std::stof(argv[4]);
		std::vector<Point> simp;
		simp = simplify_n(points, static_cast<int>(n*points.size()));
		std::fstream fs2(argv[2],
				std::fstream::out | std::fstream::trunc);

		for (Point p : simp) {
			fs2 << p.x << " " << p.y << "\n";
		}
	}
}
