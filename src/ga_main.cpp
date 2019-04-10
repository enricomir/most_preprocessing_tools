#include "ga.hpp"
#include <fstream>
#include <iostream>

int main() {
  std::ifstream f1("0.pof");
  Polygon p1(f1);
  std::ifstream f2("1.pof");
  Polygon p2(f2);

  GA g(100, 0.05, 0.1, p1, p2);

  GA::individual ind1, ind2;

  for (uint i = 0; i < 5; ++i) {
    correspondence c;
    c.first = i * 100;
    c.second = i * 100 + 10;
    ind1.correspondences.push_back(c);

    correspondence c2;
    c2.first = i * 100 + 20;
    c2.second = i * 100 + 30;
    ind2.correspondences.push_back(c2);
  }

  /*for (int i = 0; i < 3; ++i) {
    g.mutate(ind1);
    for (auto c: ind1.correspondences)
        std::cout << "(" << c.first << "," << c.second << ") ";
    std::cout << std::endl;
  }*/

    std::cout << "Pre-crossover: \n";
  for (auto c : ind1.correspondences)
    std::cout << "(" << c.first << "," << c.second << ") ";
  std::cout << std::endl;
  for (auto c : ind2.correspondences)
    std::cout << "(" << c.first << "," << c.second << ") ";
  std::cout << std::endl;

  for (int i = 0; i < 1; ++i) {
    g.cross(ind1, ind2);
  }

  std::cout << "\n\n\nPos-crossover\n";
  for (auto c : ind1.correspondences)
    std::cout << "(" << c.first << "," << c.second << ") ";
  std::cout << std::endl;
  for (auto c : ind2.correspondences)
    std::cout << "(" << c.first << "," << c.second << ") ";
  std::cout << std::endl;

  return 0;
}