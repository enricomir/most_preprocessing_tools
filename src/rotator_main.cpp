#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

template <typename T, typename T2>
/** Rotates a point around angle degrees
 *
 */
T rotate_point(const T &point, const double &angle, const T2 &center) {
  T ret;

  ret.x = (point.x - center.x) * cos(M_PI / 180 * angle) -
          (point.y - center.y) * sin(M_PI / 180 * angle);
  ret.y = (point.y - center.y) * cos(M_PI / 180 * angle) +
          (point.x - center.x) * sin(M_PI / 180 * angle);

  ret.x += center.x;
  ret.y += center.y;

  return ret;
}

template <typename T, typename T2>
/** Rotate a vector of points around the origin by angle degrees
 * */
std::vector<T> rotate_vector(const std::vector<T> &source, const double &angle,
                             const T2 &center) {
  std::vector<T> ret;
  for (T p : source) {
    ret.push_back(rotate_point(p, angle, center));
  }
  return ret;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Usage: ./rotator <points file 1> <points file 2>\n";
    exit(1);
  }

  Mat img = Mat::zeros(1000, 1000, CV_8UC3); // TODO: size
  std::vector<std::vector<Point>> polys;

  polys.emplace_back();
  {
    std::fstream fs(argv[1]);
    int x, y;
    while (fs >> x >> y) {
      polys[0].emplace_back(x, y);
    }
  }

  drawContours(img, polys, 0, Scalar(0, 255, 0));
  RotatedRect r1;

  r1 = minAreaRect(polys[0]);
  Point2f vertexes[4];

  r1.points(vertexes);
  polys.push_back(std::vector<Point>());
  for (int i = 0; i < 4; ++i) {
    polys[1].push_back(vertexes[i]);
  }

  drawContours(img, polys, 1, Scalar(0, 127, 0));
  std::cout << "Angle of p1: " << r1.angle << "\n";

  polys.emplace_back();
  {
    std::fstream fs(argv[2]);
    int x, y;
    while (fs >> x >> y) {
      polys[2].emplace_back(x, y);
    }
  }

  drawContours(img, polys, 2, Scalar(0, 0, 255));
  RotatedRect r2;

  r2 = minAreaRect(polys[2]);

  r2.points(vertexes);
  polys.push_back(std::vector<Point>());
  for (int i = 0; i < 4; ++i) {
    polys[3].push_back(vertexes[i]);
  }

  drawContours(img, polys, 3, Scalar(0, 0, 127));
  std::cout << "Angle of p2: " << r2.angle
            << ". R2 size(H, W): " << r2.size.height << "," << r2.size.width
            << "\n";

  std::cout << "Angle to rotate: " << r1.angle - r2.angle << "\n";

  Mat rot = getRotationMatrix2D(r2.center, r2.angle - r1.angle,
                                r1.size.width / r2.size.width);

  std::cout << "rotation affine dimensions: " << rot.size()
            << ", cols=" << rot.cols << "\n";

  Mat img_to_rotate = Mat::zeros(img.size(), CV_8UC3);
  drawContours(img_to_rotate, polys, 2, Scalar(255, 0, 0));
  drawContours(img_to_rotate, polys, 3, Scalar(127, 0, 0));
  warpAffine(img_to_rotate, img_to_rotate, rot, img_to_rotate.size());

  add(img, img_to_rotate, img);

  /////////////////////////////////////
  /// Vector rotation test
  /////////////////////////////////////
  std::vector<Point2i> rot_vert;
  for (int i = 0; i < 4; ++i) {
    rot_vert.push_back(vertexes[i]);
  }

  std::cout << "\nBefore rot: ";
  for (auto p : rot_vert) {
    std::cout << "(" << p.x << ", " << p.y << ") ";
  }
  rot_vert = rotate_vector(rot_vert, /*90*/r1.angle - r2.angle, r2.center);
  std::cout << "\nAfter rot: ";
  for (auto p : rot_vert) {
    std::cout << "(" << p.x << ", " << p.y << ") ";
  }
  std::cout << "\n";

  polys.push_back(rot_vert);

  std::cout << "\nPolys: ";
  for (auto p : polys[4]) {
    std::cout << "(" << p.x << ", " << p.y << ") ";
  }
  std::cout << "\n";

  drawContours(img, polys, 4, Scalar(127, 127, 0));
  /////////////////////////////////////
  /// End vector rotation test
  /////////////////////////////////////

  Point2f srcpts[3], dstpts[3];

  srcpts[0] = polys[1][0];
  for (auto p : polys[1]) {
    if (p.x < srcpts[0].x)
      srcpts[0] = p;
  }
  srcpts[1] = Point(srcpts[0].x + 1, srcpts[0].y + 1);
  srcpts[2] = Point(srcpts[0].x, srcpts[0].y + 1);

  dstpts[0] = polys[4][0];
  for (auto p : polys[4]) {
    if (p.x < dstpts[0].x)
      dstpts[0] = p;
  }
  dstpts[0].y = srcpts[0].y;///TODO REMOVE
  dstpts[1] = Point(dstpts[0].x + 1, dstpts[0].y+1);
  dstpts[2] = Point(dstpts[0].x, dstpts[0].y + 1);

  std::cout << "Affine from " << srcpts[0].x << "," << srcpts[1].y << " to " << dstpts[0].x << "," << dstpts[0].y << "\n";
  Mat transl = getAffineTransform(dstpts, srcpts);
  std::cout << "translation matrix: " << transl << std::endl;
  transl.convertTo(transl, CV_8S);
  transl.convertTo(transl, CV_32F);
  warpAffine(img_to_rotate, img_to_rotate, transl, img_to_rotate.size());
  add(img, img_to_rotate, img);

  imshow("Source 1", img);
  imwrite("delme.png", img);
  waitKey();
}