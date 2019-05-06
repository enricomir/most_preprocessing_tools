#ifndef SIMPLE_POINT_
#define SIMPLE_POINT_

struct SimplePoint {
  double x, y;
	double linear;
  SimplePoint(double x, double y) : x(x), y(y), linear(0) {};

  static double norm(const SimplePoint &p1, const SimplePoint &p2, double dz = 0.0) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return std::sqrt(dx * dx + dy * dy + dz*dz);
  }
};

#endif // SIMPLE_POINT_
