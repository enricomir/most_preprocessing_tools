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


  GA(const uint pop_size, const double mut_chance, const double cross_rate,
     const Polygon &p1, const Polygon &p2);

  std::vector<correspondence> run();

  // private:
  struct individual {
    std::vector<correspondence> correspondences;
    double performance;
  };

  const uint pop_size;
  const double mut_chance, cross_rate;
  const Polygon &p1;
  const Polygon &p2;

  std::vector<individual> pop;

  correspondence get_random_correspondence(const individual &ind);

  void mutate(individual &ind, bool force_grow=false);

  void cross(individual &ind1, individual &ind2);

  double eval(const individual &ind);

	void fix(individual& ind);
};

#endif // GA_HPP_
