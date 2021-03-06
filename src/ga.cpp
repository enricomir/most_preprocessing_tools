#include "ga.hpp"

#define MIN_SIZE 2

correspondence GA::get_random_correspondence(const individual &ind) {

  std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<size_t> dis(0, p1.points.size() - 1);

hack:
  // Gets valid first point
  bool found = false;
  size_t pick_1;
  do {
    pick_1 = dis(gen);
    found = false;
    for (correspondence c : ind.correspondences) {
      if (c.first == pick_1)
        found = true;
    }
  } while (found == true);

  // Gets valid second point
  size_t min_rand, max_rand;

  // std::cout << "First pick: " << pick_1 << ". First pick and last pick are:
  // "; std::cout << ind.correspondences[0].first << " and "
  //         << ind.correspondences[ind.correspondences.size() - 1].first
  //         << std::endl;

  if (pick_1 < ind.correspondences[0].first) { // Place pick as first
    min_rand = 0;
    max_rand = ind.correspondences[0].second - 1;
  } else if (pick_1 > ind.correspondences[ind.correspondences.size() - 1]
                          .first) { // Place pick as last
    min_rand = ind.correspondences[ind.correspondences.size() - 1].second + 1;
    max_rand = p2.points.size() - 1;
  } else { // Normal case - Break when found the point
    for (size_t i = 0; i < (ind.correspondences.size() - 1); ++i) {
      if (pick_1 < ind.correspondences[i + 1].first) {
        // std::cout << "Got normal case.\n";
        // std::cout << "  pick_1=" << pick_1 << "\n";
        // std::cout << "  ind[i].f=" << ind.correspondences[i].first << "\n";

        min_rand = ind.correspondences[i].second + 1;
        max_rand = ind.correspondences[i + 1].second - 1;
        break;
      }
    }
  }

  std::uniform_int_distribution<size_t> dis2(min_rand, max_rand);
  size_t pick_2 = dis2(gen);

  correspondence c;

  if (pick_2 > 1000) {
    goto hack;
  }
  c.first = pick_1;
  c.second = pick_2;

  return c;
}

/** Mutation for lists: Generative, destructive. No swap because of required
 * continuity
 * */
void GA::mutate(individual &ind, bool force_grow) {
  if (ind.correspondences.size() < MIN_SIZE && !force_grow)
    return; // We need at least 2 items to mutate

  std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> dis(0, 1);
  int picked_mutation = dis(gen);

  if (picked_mutation == 0 ||
      force_grow) { // Generative mutation, add a new valid correspondence

    correspondence c = get_random_correspondence(ind);

    if (c.first > ind.correspondences[ind.correspondences.size() - 1]
                      .first) { // Insert as last element
      if (c.second > 1000) {
        std::cout << "Ferrou2" << std::endl;
        exit(-1);
      }
      ind.correspondences.push_back(c);
    } else { // Base case
      for (auto it = ind.correspondences.begin();
           it < ind.correspondences.end(); ++it) {
        if (it->first > c.first) {
          ind.correspondences.insert(it, c);
          break;
        }
      }
    }

  } else if (picked_mutation == 1) { // Destructive mutation
    std::uniform_int_distribution<size_t> place(0, ind.correspondences.size());
    ind.correspondences.erase(ind.correspondences.begin() + place(gen));
  }
}

/** Crossover: 1 pt. Find an exchange place and be done.
 * */
void GA::cross(individual &ind1, individual &ind2) {

  if (ind1.correspondences.size() < MIN_SIZE ||
      ind2.correspondences.size() < MIN_SIZE)
    return; // We need individuals with at least size 2 to cross

  std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<size_t> place_1(
      1, ind1.correspondences.size() -
             2); // Must keep at least 1 item on each side

  size_t c1;
  if (ind1.correspondences.size() == 2) {
    c1 = 1;
  } else {
    c1 = place_1(gen); // Place to cut ind1
    uint tries = 0;
    while (ind1.correspondences[c1].first < ind2.correspondences[0].first ||
           ind1.correspondences[c1].first >
               ind2.correspondences[ind2.correspondences.size() - 2].first) {
      c1 = place_1(
          gen); // Cannot leave empty correspondence on ind2 on either side.
      tries++;
      if (tries == 255)
        return;
    }
  }
  // By now we know: 1 - we got a cutoff point on [1, sz1-1] that is valid on
  // some subset of ind2 on [1, sz2-1] considering first argument. We now check
  // for second

  size_t c2;
  if (ind2.correspondences.size() == 2) {
    c2 = 1;
  } else {
    size_t min_rand = 10000;

    for (size_t i = 0; i < (ind2.correspondences.size() - 1); ++i) {
      if (ind1.correspondences[c1].first > ind2.correspondences[i].first &&
          ind1.correspondences[c1].second > ind2.correspondences[i].second) {
        min_rand = i + 1;
      }
    }

    if (min_rand > (ind2.correspondences.size() - 2))
      return;
    std::uniform_int_distribution<size_t> place_2(
        min_rand, ind2.correspondences.size() -
                      2); // Must keep at least 1 item on each side

    c2 = place_2(gen);
  }

  /*Crossover operation:
   * 1 - hold v1 from [c1 end] in temporary
   * 2 - clear v1 from c1
   * 3 - copy v2 from [c2 end] into v1
   * 4 - clear v2
   * 5 - copy temporary into v2
   * */
  std::vector<correspondence> temp_hold;

  for (size_t i = c1; i < ind1.correspondences.size(); ++i) {
    temp_hold.push_back(ind1.correspondences[i]);
    if (temp_hold.size() > 100)
      std::cout << "temphold issue" << std::endl;
  }

  ind1.correspondences.erase(ind1.correspondences.begin() + c1,
                             ind1.correspondences.end());

  for (size_t i = c2; i < ind2.correspondences.size(); ++i) {
    ind1.correspondences.push_back(ind2.correspondences[i]);
  }

  ind2.correspondences.erase(ind2.correspondences.begin() + c2,
                             ind2.correspondences.end());

  for (correspondence c : temp_hold) {
    ind2.correspondences.push_back(c);
  }
}

double GA::eval(const individual &ind) {
  std::vector<double> distances;
  std::vector<double> penalization;

  if (ind.correspondences.size() < MIN_SIZE)
    return -std::numeric_limits<double>::max();

  // TODO add ultimo ponto
  for (size_t i = 0; i < (ind.correspondences.size() - 1); ++i) {

    /*std::cout << "  p1 slice:" << ind.correspondences[i].first << "/"
              << ind.correspondences[i + 1].first << "\n";

    std::cout << "  p2 slice:" << ind.correspondences[i].second << "/"
              << ind.correspondences[i + 1].second << std::endl;

    std::cout << "  cost: "
              << Polygon::chamfer(
                     p1.get_slice(ind.correspondences[i].first,
                                  ind.correspondences[i + 1].first),
                     p2.get_slice(ind.correspondences[i].second,
                                  ind.correspondences[i + 1].second))
              << std::endl;*/

    distances.push_back(
        Polygon::hausdorff(p1.get_slice(ind.correspondences[i].first,
                                      ind.correspondences[i + 1].first),
                         p2.get_slice(ind.correspondences[i].second,
                                      ind.correspondences[i + 1].second)));

    penalization.push_back(
        SimplePoint::norm(p1.points[ind.correspondences[i].first],
                          p1.points[ind.correspondences[i + 1].first]) +
        SimplePoint::norm(p2.points[ind.correspondences[i].second],
                          p2.points[ind.correspondences[i + 1].second]));
  }

  double ret = 0;
  for (double item : distances) {
    ret += item;
  }

  double pen = 0;
  for (double item : penalization) {
    pen -= (item * 0.0001);
  }

  double score = (-ret / distances.size()) + (pen / penalization.size());
  std::cout << "Distance score: " << (-ret / distances.size()) << ". Penalization score: " << (pen / penalization.size()) << std::endl;
  return score;
}

GA::GA(const uint pop_size, const double mut_chance, const double cross_rate,
       const Polygon &p1, const Polygon &p2)
    : pop_size(pop_size), mut_chance(mut_chance), cross_rate(cross_rate),
      p1(p1), p2(p2) {
  // Todo: Initialize and first pop
  for (uint i = 0; i < pop_size; ++i) {
    // Initialize random
    individual ind;

    size_t x1, x2, y1, y2;

    std::random_device rd;
    // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

    std::uniform_int_distribution<size_t> dis1(0, p1.points.size() - 1);
    std::uniform_int_distribution<size_t> dis2(0, p2.points.size() - 1);

    x1 = dis1(gen);
    x2 = dis1(gen);

    y1 = dis2(gen);
    y2 = dis2(gen);

    size_t xm, xM, ym, yM;

    xm = (x1 < x2 ? x1 : x2);
    xM = (x1 > x2 ? x1 : x2);

    ym = (y1 < y2 ? y1 : y2);
    yM = (y1 > y2 ? y1 : y2);

    ind.correspondences.push_back(std::make_pair(xm, ym));
    ind.correspondences.push_back(std::make_pair(xM, yM));

    if (ym > 1000 || yM > 1000 || ind.correspondences.size() > 100) {
      std::cout << "Ferrou1" << std::endl;
      exit(-1);
    }

    while (ind.correspondences.size() < MIN_SIZE) {
      mutate(ind, true);
    }

    std::uniform_int_distribution<uint> flip(0, 1);
    while (flip(gen) == 1)
      mutate(ind, true);
    pop.push_back(ind);
  }
}

std::vector<correspondence> GA::run() {
  std::vector<correspondence> ret;
  for (uint i = 0; i < 3; ++i) { // Generations
    double best = -std::numeric_limits<double>::max();
    size_t size = 0;
    individual best_i;

    for (auto c : pop) {
      c.performance = eval(c);
      if (c.performance > best) {
        best = c.performance;
        ret = c.correspondences;
        size = c.correspondences.size();
        best_i = c;
      }
    }
    std::cout << best << "(" << size << ")" << std::endl;

    std::vector<individual> new_pop;

    std::sort(pop.begin(), pop.end(), [](const individual &i1, individual &i2) {
      return i1.performance < i2.performance;
    });

    size_t t1, t2; // tournament for the individual

    std::random_device rd;
    // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

    std::uniform_int_distribution<size_t> dis(0, pop.size() - 1);

    // Perform crossover - increase 2 because we insert 2 in pop
    for (size_t j = 0; j < pop.size() * cross_rate; j += 2) {
      individual i1, i2;
      t1 = dis(gen);
      t2 = dis(gen);
      if (pop[t1].performance > pop[t2].performance) { // Tournament
        i1 = pop[t1];
      } else {
        i1 = pop[t2];
      }

      t1 = dis(gen);
      t2 = dis(gen);
      if (pop[t1].performance > pop[t2].performance) { // Tournament
        i2 = pop[t1];
      } else {
        i2 = pop[t2];
      }

      cross(i1, i2);
      new_pop.push_back(i1);
      new_pop.push_back(i2);
    }

    // Fill rest of population
    while (new_pop.size() < (pop_size + 1)) {
      t1 = dis(gen);
      t2 = dis(gen);
      if (pop[t1].performance > pop[t2].performance) { // Tournament
        new_pop.push_back(pop[t1]);
      } else {
        new_pop.push_back(pop[t2]);
      }
    }

    // Move new pop and mutate
    pop.clear();
    for (individual ind : new_pop) {
      individual i1;
      i1 = ind;

      std::uniform_real_distribution<double> dis(0, 1);
      double roll = dis(gen);
      // std::cout << "Mutation roll: " << roll << std::endl;

      if (roll < mut_chance)
        mutate(i1);

      pop.push_back(i1);
    }

    pop.push_back(best_i);
  }
  return ret;
}
