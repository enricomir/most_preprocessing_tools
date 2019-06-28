#include <fstream>
#include <iostream>
#include <polygon.hpp>
#include <sstream>
#include <string>
#include <vector>

int main(int, char **) {
  std::string cmd;

  while (getline(std::cin, cmd)) {
    if (cmd.size() == 0)
      continue;

    std::istringstream ss(cmd);
    std::string first_word;
    ss >> first_word;
    if (first_word == "quit") {
      return 0;
    } else if (first_word == "comp") {
      std::string next;
      std::vector<std::string> words;

      while (ss >> next)
        words.push_back(next);

      std::ifstream input(words[1]);
      if (!input.good()) {
        std::cout << "Error - file " << words[1] << " could not be open.\n";
        return 1;
      }
      Polygon p1(input);

      input.close();
      input.open(words[2]);
      if (!input.good()) {
        std::cout << "Error - file " << words[2] << " could not be open.\n";
        return 1;
      }
      Polygon p2(input);
      input.close();

      double dist = -1;
      if (words[0] == "h") { // Compute hausdorff distance
        dist = Polygon::hausdorff(p1, p2);
      } else if (words[0] == "c") { // Compute chamfer distance
        dist = Polygon::chamfer(p1, p2);
      } else if (words[0] == "p") { // Compute polis distance
        dist = Polygon::polis(p1, p2);
      } else if (words[0] == "j") { // Compute Jaccard index
				dist = Polygon::jaccard(p1, p2);
      } /*else if (words[0] == "f") { // Compute Frechet distance
				dist = Polygon::frechet(p1, p2);
			}*/

      std::cout << dist << std::endl;

      words.clear();
    } else {
      std::cout << "Error on command \"" << first_word << "\" from \"" << cmd
                << "\"\n";
    }
  }
}
