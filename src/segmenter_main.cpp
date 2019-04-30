#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using std::string;
using std::vector;

Mat image, mask, segmented, blurred;
vector<Point> obj;
vector<Point> background;
int cur_obj = 0;
const char *WHNDL = "Segmenter (press q to quit)";
string filename;
const int blur_sz = 5;

void drawMask() {
  mask = Mat::zeros(image.size(), CV_32SC1);

  // Interest points as convex hull
  /*if (obj.size() >= 3) {
          std::vector<Point> hull;
          convexHull(obj, hull);
          fillConvexPoly(mask, hull, Scalar(255, 255, 255));
  }*/

  // Interest points as points
  for (Point p : obj) {
    mask.at<int>(p) = 255;
  }

  for (Point p : background) {
    mask.at<int>(p) = 127;
  }
}

void genOverlay() {
  Mat m;
  mask.convertTo(m, CV_8UC3);
  cvtColor(m, m, COLOR_GRAY2BGR);

  addWeighted(image, 0.5, m, 0.5, 0, segmented);

  imshow(WHNDL, segmented);
  // imshow("mask", m);
  // imshow("blurred", blurred);
}

void generateContour() {
  Mat binary;
  mask.convertTo(binary, CV_32FC1);
  threshold(binary, binary, 200, 255, THRESH_BINARY);
  binary.convertTo(binary, CV_8UC1);
   imshow("Binary", binary);
  std::vector<std::vector<Point>> vertexes;
  // CHAIN_APPROX_NONE, CHAIN_APPROX_SIMPLE, CHAIN_APPROX_TC89_L1,
  // CHAIN_APPROX_TC89_KCOS

  /*findContours(binary, vertexes, RETR_EXTERNAL, CHAIN_APPROX_NONE);
  std::cout << "NONE (" << vertexes[0].size() << "): \n";
  for (Point p: vertexes[0]) {
          std::cout << p.x << "," << p.y << "  ";
  }

  findContours(binary, vertexes, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  std::cout << "\n\nSIMPLE (" << vertexes[0].size() << "): \n";
  for (Point p: vertexes[0]) {
          std::cout << p.x << "," << p.y << "  ";
  }

  findContours(binary, vertexes, RETR_EXTERNAL, CHAIN_APPROX_TC89_L1);
  std::cout << "\n\nL1 (" << vertexes[0].size() << "): \n";
  for (Point p: vertexes[0]) {
          std::cout << p.x << "," << p.y << "  ";
  }*/

  findContours(binary, vertexes, RETR_EXTERNAL, CHAIN_APPROX_TC89_KCOS);
  std::cout << "\n\nKC0S (" << vertexes[0].size() << "): \n";

  std::fstream fs(filename + ".pof",
                  std::fstream::in | std::fstream::out | std::fstream::trunc);
  std::cout << "Writing to " << (filename + ".pof") << std::endl;
  if (!fs.is_open()) {
    std::cout << "Error not open\n";
    exit(3);
  }
  for (Point p : vertexes[0]) {
    std::cout << p.x << " " << p.y << "\n ";
    fs << p.x << " " << p.y << "\n";
  }
}

static void onMouse(int event, int x, int y, int, void *) {
  if (event != EVENT_LBUTTONDOWN)
    return;
  if (x < 0 || y < 0 || x > segmented.cols || y > segmented.rows) {
    std::cout << "Detected click outside image. Ignoring.\n";
    return;
  }
  std::cout << "Clicked. (x, y, cur_obj) = (" << x << ", " << y << ", "
            << cur_obj << ")" << std::endl;

  if (cur_obj == 1) {
    obj.push_back(Point(x, y));
  } else {
    background.push_back(Point(x, y));
  }

  drawMask();

   blur(image, blurred, {blur_sz, blur_sz});
  // image.copyTo(blur);
  //blurred = image.clone();

  watershed(blurred, mask);
  genOverlay();
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Wrong usage! Correct usage: ./segmenter <source image>\n";
    exit(1);
  }

  image = imread(argv[1]);

  if (image.empty()) {
    std::cout << "Error - could not read file " << argv[1] << "as image.\n";
    exit(2);
  }

  filename = argv[1];
  filename.pop_back();
  filename.pop_back();
  filename.pop_back();
  filename.pop_back();

  namedWindow(WHNDL, WINDOW_GUI_NORMAL);

  mask = Mat::zeros(image.size(), CV_32SC1);
  // blur(image, blurred, {blur_sz, blur_sz});
  blurred = image.clone();

  genOverlay();

  unsigned char c;

  setMouseCallback(WHNDL, onMouse, 0);
  createTrackbar("Objeto (0 = fundo, 1 = interesse): ", WHNDL, &cur_obj, 1);

  std::cout << "Finished loading" << std::endl;

  while ((c = waitKey(0)) != 'q') {
    if (c == 'b') {
      generateContour();
    }
  }
  return 0;
}
