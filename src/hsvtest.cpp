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

const char *W_NAME = "HSV";
int l1 = 0, l2 = 0, l3 = 0;
int u1 = 180, u2 = 255, u3 = 255;
int iter = 0;

Mat src;

void recalcTolerance(int, void *) {
  Mat poly;
  cvtColor(src, poly, COLOR_BGR2HSV);
  Scalar l(l1, l2, l3), u(u1, u2, u3);
  inRange(poly, l, u, poly);
  // cvtColor(poly, poly, COLOR_HSV2BGR);

  if (iter != 0) {
    erode(poly, poly, Mat(), Point(-1, 1), iter);
    dilate(poly, poly, Mat(), Point(-1, 1), iter);
  }

  poly.convertTo(poly, CV_8UC1);
	cvtColor(poly, poly, COLOR_GRAY2BGR);
	addWeighted(src, 0.5, poly, 0.5, 0, poly, CV_8UC3);
  imshow(W_NAME, poly);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Wrong usage! Correct usages:\n"
                 "  ./simplifier <source points file (.pof)> <source image>\n"
                 "  ./simplifier <source points file (.pof)> <step> "
                 "<iterations> for chart generation\n";
    exit(1);
  }

  src = imread(argv[1]);

  namedWindow(W_NAME, WINDOW_NORMAL);

  createTrackbar("Lower Hue", W_NAME, &l1, 180, recalcTolerance);
  createTrackbar("Lower Sat", W_NAME, &l2, 255, recalcTolerance);
  createTrackbar("Lower Light", W_NAME, &l3, 255, recalcTolerance);

  createTrackbar("Upper Hue", W_NAME, &u1, 180, recalcTolerance);
  createTrackbar("Upper Sat", W_NAME, &u2, 255, recalcTolerance);
  createTrackbar("Upper Light", W_NAME, &u3, 255, recalcTolerance);

  createTrackbar("Iterations", W_NAME, &iter, 100, recalcTolerance);

  recalcTolerance(0, nullptr);

  char c;
  while ((c = waitKey()) != 'q') {
  }
}