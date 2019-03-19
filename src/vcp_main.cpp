#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <utility> //for pair

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

typedef std::pair<double, double> turning_step; // Length, angle
typedef std::vector<turning_step> turning_function;

turning_function get_turning_function(const std::vector<Point> &poly) {
  turning_function ret;
  double p = 0.0;

  Point last = poly[poly.size() - 1]; // Close the loop

  for (const Point pt : poly) {
    double dx = last.x - pt.x, dy = last.y - pt.y;

    double l = std::sqrt(dx * dx + dy * dy); // Length of segment
    double alpha = std::atan2(dy, dx);
    alpha += (alpha < 0 ? 2 * M_PI : 0); // No negative angles

    ret.emplace_back(l, alpha);
    p += l; // updates perimeter
    last = pt;
  }

  // Fixes length by perimeter, angle by degrees
  std::for_each(ret.begin(), ret.end(), [p](turning_step &st) {
    st.first /= p;
    st.second *= (180 / M_PI);
  });

  double acc_length = 0;

  std::for_each(ret.begin(), ret.end(), [&acc_length](turning_step &st) {
    acc_length += st.first;
    st.first = acc_length;
  });

  return ret;
}

/** Aligns the destination function with the source function by theta*
 * */
void align_functions(const turning_function &src, turning_function &dst,
                     double shift = 0.0) {
  double area_src = 0.0, area_dst = 0.0;
  double lastx = 0.0;

  for (const auto p : src) {
    double dx = std::abs(p.first - lastx);
    double dy = std::abs(p.second);

    area_src += dx * dy;

    lastx = p.first;
  }

  lastx = 0;
  for (const auto p : dst) {
    double dx = std::abs(p.first - lastx);
    double dy = std::abs(p.second);

    area_dst += dx * dy;

    lastx = p.first;
  }

  double theta_star = area_src - area_dst + 2 * M_PI * shift;

  std::for_each(dst.begin(), dst.end(), [theta_star](turning_step t) {
    t.second += theta_star;
    while (t.second >= 360)
      t.second -= 360;
  });

  return;
}

/** Returns the indexes of the best matching points (lowest area)
 * */
std::pair<size_t, size_t> best_fit(const std::vector<Point> &f1,
                                   const std::vector<Point> &f2,
                                   double *out_area = nullptr) {

  std::pair<size_t, size_t> ret;
  double min_area = 50000;

  std::vector<Point> src = f1;
  std::vector<Point> dst = f2;

  for (size_t i = 0; i < src.size(); ++i) {
    turning_function src_f = get_turning_function(src);
    std::rotate(src.begin(), src.begin() + 1, src.end());

    for (size_t j = 0; j < dst.size(); ++j) {
      turning_function dst_f = get_turning_function(dst);
      std::rotate(dst.begin(), dst.begin() + 1, dst.end());

      align_functions(src_f, dst_f);

      size_t src_i = 0, dst_i = 0;
      double last_src_step, last_dst_step;

      last_src_step = 0;

      last_dst_step = 0;

      double area = 0;

      while (src_i < src_f.size() || dst_i < dst_f.size()) {
        if (src_i == src_f.size()) { // Just advance destination
          double dx =
              dst_f[dst_i].first - std::max(last_src_step, last_dst_step);
          double dy = std::abs(src_f[src_i - 1].second - dst_f[dst_i].second);

          area += dx * dy;

          // std::cout << "\tDst running alone. Added area: " << dx * dy
          //          << std::endl;

          dst_i++;
        } else if (dst_i == dst_f.size()) { // Just advance source
          double dx =
              src_f[src_i].first - std::max(last_src_step, last_dst_step);
          double dy = std::abs(src_f[src_i].second - dst_f[dst_i - 1].second);

          area += dx * dy;

          // std::cout << "\tSrc running alone. Added area: " << dx * dy
          //          << std::endl;

          src_i++;
        } else {
          // std::cout << "Deciding first (src, dst) = " << src_f[src_i].first
          //          << ", " << dst_f[dst_i].first << ")\n";
          if (src_f[src_i].first <
              dst_f[dst_i].first) { // Add a area do ultimo até aqui, e avança
            // src polygon
            double dx =
                src_f[src_i].first - std::max(last_src_step, last_dst_step);
            double dy = std::abs(src_f[src_i].second - dst_f[dst_i].second);

            area += dx * dy;

            // std::cout << "\tSrc first. Added area: " << dx * dy << std::endl;

            last_src_step = src_f[src_i].first;
            src_i++;
          } else {
            double dx =
                dst_f[dst_i].first - std::max(last_src_step, last_dst_step);
            double dy = std::abs(src_f[src_i].second - dst_f[dst_i].second);

            area += dx * dy;

            // std::cout << "\tDst first. Added area: " << dx * dy << std::endl;

            last_dst_step = dst_f[dst_i].first;
            dst_i++;
          }
        }
      }
      if (area < min_area) {
        min_area = area;
        ret.first = i;
        ret.second = j;
      }
      // std::cout << "Final area: " << area << "\n";
    }
  }
  std::cout << "Returning " << ret.first << "," << ret.second << " for area "
            << min_area << std::endl;

  if (out_area != nullptr)
    *out_area = min_area;
  return ret;
}

/** Reduces the red_poly by 1 vertex by minimizing the Arkin Area difference to
 * ref_poly.
 * Attention: this might induce reduction of significant points in the
 * red_poly
 */
std::vector<Point> reduce_best_fit(const std::vector<Point> &ref_poly,
                                   const std::vector<Point> &red_poly) {
  std::vector<Point> result;
  double min_area = 100000; //"Infinite" constant TODO

  for (size_t i = 0; i < red_poly.size(); ++i) {
    double area;
    std::vector<Point> current_iteration = red_poly;

    current_iteration.erase(current_iteration.begin() + i);

    best_fit(ref_poly, current_iteration, &area);

    if (area < min_area) {
      area = min_area;
      result = current_iteration;
    }
  }

  // TODO: Rotation

  return result;
}

int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 2) {
    std::cout << "Error. Usage: ./vcp <pof file 1> <pof file 2> to create "
                 "correlations file\n";
    std::cout
        << "              ./vcp <pof file> to create turning function csv\n";
    return 1;
  }

  std::vector<Point> polys[2];
  {
    Point i;
    std::ifstream fs(argv[1]);
    int x, y;

    while (fs >> x >> y) {
      polys[0].emplace_back(x, y);
    }
  }

  if (argc == 2) {
    turning_function t = get_turning_function(polys[0]);
    // double last_x = 0.0;
    for (turning_step cur : t) {
      // std::cout << last_x << " " << cur.second << "\n";
      // last_x = cur.first;
      std::cout << cur.first << " " << cur.second << "\n";
    }
  } else if (argc == 3) {
    {
      Point i;
      std::ifstream fs(argv[2]);
      int x, y;

      while (fs >> x >> y) {
        polys[1].emplace_back(x, y);
      }
    }
    best_fit(polys[0], polys[1]);

    while (polys[0].size() < polys[1].size()) {
      polys[1] = reduce_best_fit(polys[0], polys[1]);
    }

    while (polys[1].size() < polys[0].size()) {
      polys[0] = reduce_best_fit(polys[1], polys[0]);
    }

    std::pair<size_t, size_t> bests = best_fit(polys[0], polys[1]);
    std::rotate(polys[0].begin(), polys[0].begin() + bests.first,
                polys[0].end());
    std::rotate(polys[1].begin(), polys[1].begin() + bests.second,
                polys[1].end());

    {
      int i = 0;
      std::ofstream fs("corr_1.pof");
      for (auto p : polys[0]) {
        fs << ++i << " " << p.x << " " << p.y << "\n";
      }
    }

    {
      int i = 0;
      std::ofstream fs("corr_2.pof");
      for (auto p : polys[1]) {
        fs << ++i << " " << p.x << " " << p.y << "\n";
      }
    }
  }
}
