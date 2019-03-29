#include <fstream>
#include <iostream>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

int main(int, char *argv[]) {
  VideoCapture vid(argv[1]);

  Mat frame, last_frame;
  Mat new_frame;

  vid >> last_frame;

  Scalar px, last_px;

  VideoWriter w("output.mp4", VideoWriter::fourcc('F', 'M', 'P', '4'),
                vid.get(CAP_PROP_FPS), last_frame.size());

  while (vid.get(CAP_PROP_POS_FRAMES) < vid.get(CAP_PROP_FRAME_COUNT)) {
    vid >> frame;

    new_frame = Mat::zeros(frame.size(), frame.type());

    for (size_t i = 0; i < frame.total(); ++i) {
      px = frame.at<Vec3b>(i);
      last_px = last_frame.at<Vec3b>(i);

      int r, last_r;

      r = std::max({px[0], px[1], px[2]}) - std::min({px[0], px[1], px[2]});
      last_r = std::max({last_px[0], last_px[1], last_px[2]}) -
               std::min({last_px[0], last_px[1], last_px[2]});

      Scalar n;
      n = (r < last_r ? last_px : px);

      new_frame.at<Vec3b>(i)[0] = n[0];
      new_frame.at<Vec3b>(i)[1] = n[1];
      new_frame.at<Vec3b>(i)[2] = n[2];
    }

    w << new_frame;

    if (static_cast<int>(vid.get(CAP_PROP_POS_FRAMES)) % 100 == 0)
      std::cout << vid.get(CAP_PROP_POS_FRAMES) * 100 /
                       vid.get(CAP_PROP_FRAME_COUNT)
                << "\% - " << vid.get(CAP_PROP_POS_FRAMES) << " of "
                << vid.get(CAP_PROP_FRAME_COUNT) << std::endl;

    vid.set(CAP_PROP_POS_FRAMES, vid.get(CAP_PROP_POS_FRAMES) + 1);
    last_frame = frame;
  }

  return 0;
}