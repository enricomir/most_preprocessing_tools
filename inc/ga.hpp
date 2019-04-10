#ifndef GA_HPP_
#define GA_HPP_

#include "polygon.hpp"
#include <cstddef> //for size_t
#include <random>
#include <utility> //for pair
#include <vector>

typedef std::pair<size_t, size_t> correspondence;

class GA {
public:


  GA(const uint pop, const double mut_chance, const double cross_rate,
     const Polygon &p1, const Polygon &p2)
      : pop(pop), mut_chance(mut_chance), cross_rate(cross_rate), p1(p1),
        p2(p2) {}

  std::vector<correspondence> run();

  // private:
  struct individual {
    std::vector<correspondence> correspondences;
    double performance;
  };

  const uint pop;
  const double mut_chance, cross_rate;
  const Polygon &p1;
  const Polygon &p2;

  correspondence get_random_correspondence(const individual &ind);

  void mutate(individual &ind);

  void cross(individual &ind1, individual &ind2);

  double eval(const individual &) { return 0; }
};

#endif // GA_HPP_