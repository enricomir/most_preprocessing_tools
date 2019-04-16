#include "polygon.hpp"
#include "simplifier.hpp"
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

typedef std::pair<size_t, size_t> correspondence;

void print(Polygon p) {
	for (auto c : p.points)
		std::cout << c.x << " " << c.y << "/";
	std::cout << std::endl;
}

std::vector<correspondence> get_correspondences(Polygon &p1,
		const Polygon &corr_1,
		Polygon &p2,
		const Polygon &corr_2) {
	std::vector<correspondence> ret;

	size_t rot;

	// Rotate first polygon to match correspondences
	rot = 0;
	while (corr_1.points[0].x != p1.points[rot].x ||
			corr_1.points[0].y != p1.points[rot].y) {
		rot++;
	}
	std::rotate(p1.points.begin(), p1.points.begin() + rot, p1.points.end());

	// Rotate second polygon to match correspondences
	rot = 0;
	while (corr_2.points[0].x != p2.points[rot].x ||
			corr_2.points[0].y != p2.points[rot].y) {
		rot++;
	}
	std::rotate(p2.points.begin(), p2.points.begin() + rot, p2.points.end());

	for (size_t i = 0; i < corr_1.points.size(); ++i) {
		// Finds point in p1
		size_t pos1 = 0;

		while (corr_1.points[i].x != p1.points[pos1].x ||
				corr_1.points[i].y != p1.points[pos1].y)
			pos1++;

		size_t pos2 = 0;
		while (corr_2.points[i].x != p2.points[pos2].x ||
				corr_2.points[i].y != p2.points[pos2].y)
			pos2++;

		if (corr_1.points[i].x == 803)
			std::cout << "We friggin have 803 in the corr1\n";

		ret.push_back(std::make_pair(pos1, pos2));
	}

	return ret;
}

void match_all(std::vector<correspondence> cs, Polygon &p1, Polygon &p2) {
	size_t min1, max1, min2, max2;

	std::vector<size_t> to_remove_p1;
	std::vector<size_t> to_remove_p2;

	// TODO: end of polygon case
	for (size_t i = 0; i < cs.size(); ++i) {
		min1 = cs[i].first + 1;
		max1 = (i == (cs.size() - 1) ? p1.points.size() : cs[i + 1].first);
		// No -1 because of loops. For last iteration, until the end of polygon

		min2 = cs[i].second + 1;
		max2 = (i == (cs.size() - 1) ? p2.points.size() : cs[i + 1].second);
		// No -1 because of loops. For last iteration, until the end of polygon

		if ((max1 - min1) > (max2 - min2)) {
			size_t to_remove = (max1 - min1) - (max2 - min2);
			size_t remove_every = (max1 - min1) / to_remove;

			for (size_t i = min1; i < max1; ++i) {
				if ((i - min1) % remove_every == 0 && to_remove > 0) {
					if (to_remove_p1.size() == 224) {
						std::cout << "Error\n";
						std::cout << "Point to remove: " << p1.points[i].x << ","
							<< p1.points[i].y << "\n";
						std::cout << "Min1, max1, i: " << min1 << ", " << max1 << ", " << i
							<< "\n";

						std::cout << "Existe ponto 803 nas correspos? ";
						for (auto p : cs) {
							if (p1.points[p.first].x == 803)
								std::cout << "s";
						}
						std::cout << "\n";
						// exit(-1);
					} else {
					}

					to_remove_p1.push_back(i);
					to_remove--;
				}
			}
		} else { // interval 1 is smaller
			size_t to_remove = (max2 - min2) - (max1 - min1);

			if (to_remove != 0) {
				size_t remove_every = (max2 - min2) / to_remove;

				for (size_t i = min2; i < max2; ++i) {
					if ((i - min2) % remove_every == 0 && to_remove > 0) {
						to_remove_p2.push_back(i);
						to_remove--;
					}
				}
			} else {
				std::cout << std::endl;
			}
		}
	}

	// Consistency check
	std::cout << "Check: p1 size, p2 size, p1_new_size, p2_new_size\n";
	std::cout << p1.points.size() << " " << p2.points.size() << " ";
	std::cout << p1.points.size() - to_remove_p1.size() << " ";
	std::cout << p2.points.size() - to_remove_p2.size() << std::endl;

	// Erases
	// Back-to-front so we won't invalidate future iterators
	for (size_t i = to_remove_p1.size() - 1; i != 0; --i) {
		SimplePoint p = p1.points[to_remove_p1[i]];
		if (p.x == 803) {
			std::cout << "ERROR removing at i=" << i << "critical point\n";
			// exit(-1);
		}
		p1.points.erase(p1.points.begin() + to_remove_p1[i]);
	}
	p1.points.erase(p1.points.begin() + to_remove_p1[0]);

	for (size_t i = to_remove_p2.size() - 1; i != 0; --i) {
		p2.points.erase(p2.points.begin() + to_remove_p2[i]);
	}
	p2.points.erase(p2.points.begin() + to_remove_p2[0]);

	std::cout << "Real sizes: " << p1.points.size() << " " << p2.points.size()
		<< std::endl;
}

int main(int argc, char* argv[]) {
	// Old main. Not useful anymore.
	/**
		std::ifstream f("0.pof");
		Polygon p0(f);

		Polygon p0s = Simplifier::simplify(p0, 20);

		std::ifstream f2("1.pof");
		Polygon p1(f2);
		Polygon p1s = Simplifier::simplify(p1, 20);

		{
		std::ofstream s1("temp_0s.pof");
		p0s.save(s1);
		std::ofstream s2("temp_1s.pof");
		p1s.save(s2);
		}

		if (system("./vcp temp_0s.pof temp_1s.pof") != 0) {
		std::cout << "Error.\n";
		return 1;
		}

	// Now we have the simplified correspondences.
	std::ifstream c1("corr_1.pof");
	std::ifstream c2("corr_2.pof");

	Polygon corr_0(c1), corr_1(c2);

	for (auto p : corr_0.points) {
	if (p.x == 803)
	std::cout << "Main we have\n";
	}
	std::vector<correspondence> corrs =
	get_correspondences(p0, corr_0, p1, corr_1);

	for (auto c : corrs) {
	std::cout << c.first << ", " << c.second << std::endl;
	}

	match_all(corrs, p0, p1);

	{
	std::ofstream o0("0smatch.pof");
	p0.save(o0);

	std::ofstream o1("1smatch.pof");
	p1.save(o1);
	}*/

	if (argc < 4) {
		std::ifstream if1("msimp1.pof");
		std::ifstream if2("msimp2.pof");

		Polygon p1(if1), p2(if2);
		
		std::vector<Polygon> polis;
		polis.push_back(p1);
		polis.push_back(p2);

		Simplifier::tetrahedral_until_n_points(polis, 0.875);

		std::cout << "Error - too few arguments. Usage:\n"
			<< "  ./msimp <t/n/p> <value> <file1> <file2> ...\n";
			std::cout << argv[1];
		return -1;
	}

	return 0;
}
