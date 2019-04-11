#include "ga.hpp"
#include <fstream>
#include <iostream>

int main() {
  std::ifstream f1("0s.pof");
  Polygon p1(f1);
  std::ifstream f2("1s.pof");
  Polygon p2(f2);

  GA g(2000, 0.4, 0.1, p1, p2);

  GA::individual ind1, ind2;

  /*for (uint i = 0; i < 5; ++i) {
    correspondence c;
    c.first = i * 100;
    c.second = i * 100 + 10;
    ind1.correspondences.push_back(c);

    correspondence c2;
    c2.first = i * 100 + 20;
    c2.second = i * 100 + 30;
    ind2.correspondences.push_back(c2);
  }*/

  /** Mutation test
  for (int i = 0; i < 3; ++i) {
    g.mutate(ind1);
    for (auto c: ind1.correspondences)
        std::cout << "(" << c.first << "," << c.second << ") ";
    std::cout << std::endl;
  }*/

  /** Crossover test
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
  std::cout << std::endl;*/

  //(49,330) (234,577) (465,634)
  // Eval test
  /*ind1.correspondences.push_back(std::make_pair(49, 330));
  ind1.correspondences.push_back(std::make_pair(234, 577));
  ind1.correspondences.push_back(std::make_pair(300, 600));
  std::cout << g.eval(ind1) << std::endl;

  ind1.correspondences.push_back(std::make_pair(465, 634));
  std::cout << g.eval(ind1) << std::endl;*/

  
  std::vector<correspondence> c = g.run();

  for (auto corr: c) {
    std::cout << "P1(" << p1.points[corr.first].x << "," <<
  p1.points[corr.first].y << ") "; std::cout << "P2(" <<
  p2.points[corr.second].x << "," << p2.points[corr.second].y << ")\n";
  }
  std::cout << std::endl;
  return 0;
}