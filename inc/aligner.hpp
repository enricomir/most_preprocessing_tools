#ifndef ALIGNER_HPP
#define ALIGNER_HPP

#include "polygon.hpp"

namespace Aligner {
	void translate_centroids(Polygon& to_align, const Polygon& target);
}

#endif
