#include "aligner.hpp"
#include <geos_c.h>

void Aligner::translate_centroids(Polygon& to_align, const Polygon& target) {
	GEOSContextHandle_t geos;
	geos = GEOS_init_r();

	GEOSCoordSequence *cs1 = GEOSCoordSeq_create_r(geos, to_align.points.size() + 1, 2); // 2 dimensional shapes

	for (size_t i = 0; i < to_align.points.size(); ++i) {
		GEOSCoordSeq_setX_r(geos, cs1, i, to_align.points[i].x);
		GEOSCoordSeq_setY_r(geos, cs1, i, to_align.points[i].y);
	}
	GEOSCoordSeq_setX_r(geos, cs1, to_align.points.size(), to_align.points[0].x);
	GEOSCoordSeq_setY_r(geos, cs1, to_align.points.size(), to_align.points[0].y);

	GEOSGeom lring1 = GEOSGeom_createLinearRing_r(
			geos, cs1); // Creates the linear ring - I lose ownership of CS here
		GEOSGeom poly1 = GEOSGeom_createPolygon_r(
			geos, lring1, NULL,
			0); // Creates the polygon. I lose ownership of lring here.

	GEOSCoordSequence *cs2 = GEOSCoordSeq_create_r(geos, target.points.size() + 1, 2); // 2 dimensional shapes

	for (size_t i = 0; i < target.points.size(); ++i) {
		GEOSCoordSeq_setX_r(geos, cs2, i, target.points[i].x);
		GEOSCoordSeq_setY_r(geos, cs2, i, target.points[i].y);
	}
	GEOSCoordSeq_setX_r(geos, cs2, target.points.size(), target.points[0].x);
	GEOSCoordSeq_setY_r(geos, cs2, target.points.size(), target.points[0].y);

	GEOSGeom lring2 = GEOSGeom_createLinearRing_r(
			geos, cs2); // Creates the linear ring - I lose ownership of CS here
		GEOSGeom poly2 = GEOSGeom_createPolygon_r(
			geos, lring2, NULL,
			0); // Creates the polygon. I lose ownership of lring here.

	GEOSGeom cent1;
	double x1, y1;

	cent1 = GEOSGetCentroid_r(geos, poly1);
	GEOSGeomGetX_r(geos, cent1, &x1);
	GEOSGeomGetY_r(geos, cent1, &y1);
	std::cout << "Centroid for to_align: " << x1 << "  " << y1 << "\n";

	GEOSGeom cent2;
	double x2, y2;
	cent2 = GEOSGetCentroid_r(geos, poly2);
	GEOSGeomGetX_r(geos, cent2, &x2);
	GEOSGeomGetY_r(geos, cent2, &y2);
	std::cout << "Centroid for target: " << x2 << "  " << y2 << "\n";

	double dx = x2 - x1;
	double dy = y2 - y1;
	std::cout << "Delta: " << dx << " " << dy << "\n";

	for (SimplePoint& p: to_align.points) {
		p.x += dx;
		p.y += dy;
	}

	// cs1 is owned by GEOSGeom lring1.
	// lring1 is owned by GEOSGeom poly1
	GEOSGeom_destroy(poly1); // Destroy poly1
	GEOSGeom_destroy(poly2);
	GEOS_finish_r(geos);    // Destroy handler
}
