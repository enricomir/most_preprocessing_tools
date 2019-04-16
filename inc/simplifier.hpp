#ifndef SIMPLIFIER_HPP_
#define SIMPLIFIER_HPP_
#include "polygon.hpp"
#include "simplepoint.hpp"
#include <limits>

typedef Polygon (*simplification_tolerance_function)(const Polygon, double);

namespace Simplifier {

	/*Polygon simplify(const Polygon &origin, double min_area) {
		// We will only be dealing with area squared to simplify computational
		// resources
		min_area = min_area * min_area;

		// Every triangle will be defined by their first point id. The vertexes are
		// P(i), P(i+1), P(i+2).
		Polygon reduced(origin);

		// Create 2 new points at the end to close triangles
		reduced.points.push_back(reduced.points[0]);
		reduced.points.push_back(reduced.points[1]);

		for (unsigned int i = 0;
				i < (reduced.points.size() -
					2);) { // Increment will be at end of loop due to logic needed

			double d01, d02, d12, p; // p = semiperimeter

			d01 = SimplePoint::norm(reduced.points[i + 0], reduced.points[i + 1]);
			d02 = SimplePoint::norm(reduced.points[i + 0], reduced.points[i + 2]);
			d12 = SimplePoint::norm(reduced.points[i + 1], reduced.points[i + 2]);

			p = (d01 + d02 + d12) / 2;

			double area_sq = p * (p - d01) * (p - d02) * (p - d12);

			// if the area is smaller than the min acceptable, remove and
			if (area_sq < min_area) {
				reduced.points.erase(reduced.points.begin() +
						(i + 1) %
						(reduced.points.size() - 2)); // Erase the point

				i = std::max(0, static_cast<int>(i) -
						2); // We might need to recalc 2 points back

				reduced.points.pop_back();
				reduced.points.pop_back();
				reduced.points.push_back(reduced.points[0]);
				reduced.points.push_back(reduced.points[1]);
				// Current point is a new point, so we won't iterate.
			} else { // Otherwise, just loop as usual
				++i;
			}
		}

		reduced.points.pop_back();
		reduced.points.pop_back();

		return reduced;
	}*/

	void tetrahedral_until_n_points(std::vector<Polygon> polys, float red_percentage); 
}; // namespace Simplifier

#endif //SIMPLIFIER_HPP_
