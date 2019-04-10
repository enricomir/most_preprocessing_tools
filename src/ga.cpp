#include "ga.hpp"

correspondence GA::get_random_correspondence(const individual &ind) {

  std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<size_t> dis(0, p1.points.size() - 1);

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

  // std::cout << "Min/max to pick for " << pick_1 << ": " << min_rand << ","
  //         << max_rand << std::endl;

  std::uniform_int_distribution<size_t> dis2(min_rand, max_rand);
  size_t pick_2 = dis2(gen);

  correspondence c;

  c.first = pick_1;
  c.second = pick_2;

  return c;
}

/** Mutation for lists: Generative, destructive. No swap because of required
 * continuity
 * */
void GA::mutate(individual &ind) {
  if (ind.correspondences.size() < 2)
    return; // We need at least 2 items to mutate

  std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> dis(0, 1);
  int picked_mutation = dis(gen);

  if (picked_mutation ==
      0) { // Generative mutation, add a new valid correspondence

    correspondence c = get_random_correspondence(ind);

    if (c.first > ind.correspondences[ind.correspondences.size() - 1]
                      .first) { // Insert as last element
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

  if (ind1.correspondences.size() < 2 || ind2.correspondences.size() < 2)
    return; // We need individuals with at least size 2 to cross

  std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<size_t> place_1(
      1, ind1.correspondences.size() -
             2); // Must keep at least 1 item on each side

  size_t c1 = place_1(gen); // Place to cut ind1
  while (ind1.correspondences[c1].first < ind2.correspondences[0].first ||
         ind1.correspondences[c1].first >
             ind2.correspondences[ind2.correspondences.size() - 2].first)
    c1 = place_1(
        gen); // Cannot leave empty correspondence on ind2 on either side.

  // By now we know: 1 - we got a cutoff point on [1, sz1-1] that is valid on
  // some subset of ind2 on [1, sz2-1] considering first argument. We now check
  // for second
  size_t min_rand = 10000;

  for (size_t i = 0; i < (ind2.correspondences.size() - 1); ++i) {
    if (ind1.correspondences[c1].first > ind2.correspondences[i].first &&
        ind1.correspondences[c1].second > ind2.correspondences[i].second) {
      min_rand = i + 1;
    }
  }

  std::uniform_int_distribution<size_t> place_2(
      min_rand, ind2.correspondences.size() -
                    2); // Must keep at least 1 item on each side

  size_t c2 = place_2(gen);

  std::cout << c2 << "(" << ind2.correspondences[c2].first << ","
            << ind2.correspondences[c2].second << ")" << std::endl;

    /*Crossover operation: 
     * 1 - hold v1 from [c1 end] in temporary
     * 2 - clear v1 from c1
     * 3 - copy v2 from [c2 end] into v1
     * 4 - clear v2
     * 5 - copy temporary into v2
     * */
    std::vector<correspondence> temp_hold;

    for (size_t i = c1; i < ind1.correspondences.size(); ++i)
        temp_hold.push_back(ind1.correspondences[i]);
    
    ind1.correspondences.erase(ind1.correspondences.begin() + c1, ind1.correspondences.end());

    for (size_t i = c2; i < ind2.correspondences.size(); ++i)
        ind1.correspondences.push_back(ind2.correspondences[i]);
    
    ind2.correspondences.erase(ind2.correspondences.begin() + c2, ind2.correspondences.end());

    for (auto c: temp_hold)
        ind2.correspondences.push_back(c);

}