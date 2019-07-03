#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using std::string;
using std::vector;

std::vector<Mat> frames;

int mode = 0;
int cur_obj = 0;
const char* WHNDL = "IntermediateProc";
string filename;

Mat color_normalize(const Mat& src)
{
    Mat lab_image;
    cvtColor(src, lab_image, COLOR_BGR2Lab);

    // Extract the L channel
    std::vector<Mat> lab_planes(3);
    split(lab_image, lab_planes); // now we have the L image in lab_planes[0]

    // apply the CLAHE algorithm to the L channel
    Ptr<CLAHE> clahe = createCLAHE();
    clahe->setClipLimit(4);
    Mat dst;
    clahe->apply(lab_planes[0], dst);

    // Merge the the color planes back into an Lab image
    dst.copyTo(lab_planes[0]);
    merge(lab_planes, lab_image);

    // convert back to RGB
    cv::Mat image_clahe;
    cv::cvtColor(lab_image, image_clahe, COLOR_Lab2BGR);

    return image_clahe;
}

/** Returns a mask after converting src image to HSV space, and auto-finding
 * parameters
 */
Mat drawMask(const Mat& src)
{
    Mat mask; // Mask to be returned
    Mat hsv_img, fire_bin, bg_bin, st_bin;
    mask = Mat::zeros(src.size(), CV_32SC1);

    cvtColor(src, hsv_img, COLOR_BGR2HSV); // Converts to HSV

    inRange(hsv_img, Scalar(122, 0, 0), Scalar(180, 17, 84), fire_bin);
    erode(fire_bin, fire_bin, Mat(), Point(-1, -1), 1);
    dilate(fire_bin, fire_bin, Mat(), Point(-1, -1), 1);
    add(mask, fire_bin, mask, noArray(), CV_32SC1);
    // Filter fire pixels.
    /*inRange(hsv_img, Scalar(0, 80, 200), Scalar(25, 255, 255), fire_bin);
  erode(fire_bin, fire_bin, Mat(), Point(-1, -1), 1);
  dilate(fire_bin, fire_bin, Mat(), Point(-1, -1), 1);
  add(mask, fire_bin, mask, noArray(), CV_32SC1);

  // Another side of the hue scale for fire
  inRange(hsv_img, Scalar(168, 70, 200), Scalar(180, 255, 255), fire_bin);
  erode(fire_bin, fire_bin, Mat(), Point(-1, -1), 1);
  dilate(fire_bin, fire_bin, Mat(), Point(-1, -1), 1);
  add(mask, fire_bin, mask, noArray(), CV_32SC1);*/

    // Background
    // Plants + SMoke
    //inRange(hsv_img, Scalar(0, 0, 0), Scalar(180, 127, 170), bg_bin);
    //erode(bg_bin, bg_bin, Mat(), Point(-1, -1), 2); // 2
    //dilate(bg_bin, bg_bin, Mat(), Point(-1, -1), 2);
    inRange(hsv_img, Scalar(17, 32, 24), Scalar(128, 255, 255), bg_bin);
    erode(bg_bin, bg_bin, Mat(), Point(-1, -1), 4); // 2
    dilate(bg_bin, bg_bin, Mat(), Point(-1, -1), 4);


    // Estradas
    //inRange(hsv_img, Scalar(0, 0, 140), Scalar(14, 70, 230), st_bin);
    //erode(st_bin, st_bin, Mat(), Point(-1, -1), 2);
    //dilate(st_bin, st_bin, Mat(), Point(-1, -1), 2);
    //add(bg_bin, st_bin, bg_bin);

    // Bg as color 127
    addWeighted(mask, 1, bg_bin, 0.5, 0, mask, CV_32SC1);

    return mask;
}

Mat get_segmented(const Mat& src)
{
    Mat mask; // Mask for seeds and watershed
    mask = drawMask(src);
    watershed(src, mask);

    // Generates the overlay
    Mat segmented; // Segmented image with the overlay
    mask.convertTo(mask, CV_8UC3);
    cvtColor(mask, mask, COLOR_GRAY2BGR);
    addWeighted(src, 0.5, mask, 0.5, 0, segmented);

    return segmented;
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cout
            << "Wrong usage! Correct usage: ./segmenter <i/v/b> <source image>\n";
        exit(1);
    }

    if (argv[1][0] == 'i') {
        mode = 1; // Software mode = 1 = image
        // const int blur_sz = 10; // Blur size, if using smoothing filters

        Mat image; // Original image
        image = imread(argv[2]);
        if (image.empty()) {
            std::cout << "Error - could not read file " << argv[2] << "as image.\n";
            exit(2);
        }

        namedWindow(WHNDL, WINDOW_NORMAL);

        // blur(image, blurred, {blur_sz, blur_sz});

        Mat mask; // Mask to hold the values
        mask = drawMask(image);

        // Shows pre-segmentation mask
        {
            Mat maskshow;
            namedWindow("Pre-segmentation mask", WINDOW_NORMAL);
            mask.convertTo(maskshow, CV_8UC3);
            cvtColor(maskshow, maskshow, COLOR_GRAY2BGR);
            addWeighted(image, 0.5, maskshow, 0.5, 0, maskshow, CV_8UC3);
            imshow("Pre-segmentation mask", maskshow);
        }

        watershed(image, mask);

        // Generates the overlay
        Mat segmented; // Segmented image with the overlay
        mask.convertTo(mask, CV_8UC3);
        cvtColor(mask, mask, COLOR_GRAY2BGR);
        addWeighted(image, 0.5, mask, 0.5, 0, segmented);

        imshow(WHNDL, segmented);

        unsigned char c;
        while ((c = waitKey(0)) != 'q') {
        }
    } else if (argv[1][0] == 'v') {
        Mat cur_frame;
        VideoCapture vid(argv[2]);
        double max_frames = vid.get(CAP_PROP_FRAME_COUNT);

        vid >> cur_frame; // Pre-read first frame to get sizes
        VideoWriter w("output.mp4", VideoWriter::fourcc('F', 'M', 'P', '4'),
            vid.get(CAP_PROP_FPS), cur_frame.size());
        vid.set(CAP_PROP_POS_FRAMES, 0);

        // Serial video
        auto start = std::chrono::system_clock::now();
        while (vid.get(CAP_PROP_POS_FRAMES) < max_frames) {
            // TODO: remove hardcap on 1000 frames
            if (vid.get(CAP_PROP_POS_FRAMES) == 100)
                break;
            vid >> cur_frame;

            // image = color_normalize(image); // If we need to normalize luminance

            Mat segmented = get_segmented(cur_frame); // Segmented image with the overlay

            w << segmented;

            std::cout << vid.get(CAP_PROP_POS_FRAMES) * 100 / max_frames << "\% - "
                      << vid.get(CAP_PROP_POS_FRAMES) << " of " << max_frames
                      << std::endl;
        }
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end - start;
        std::cout << "Serial finished with: " << diff.count() << " seconds." << std::endl;

        // Thread
        /*std::vector<Mat> frames;
    std::vector<std::future<Mat>> proc_frames;
    vid.set(CAP_PROP_POS_FRAMES, 0);
    auto start = std::chrono::system_clock::now();
    while (vid.get(CAP_PROP_POS_FRAMES) < max_frames) {
      // TODO: remove hardcap on 1000 frames
      if (vid.get(CAP_PROP_POS_FRAMES) == 100)
        break;
      vid >> cur_frame;
      frames.push_back(cur_frame);
    }

    for (auto p : frames) {
      proc_frames.push_back(std::async(get_segmented, p));
    }
    frames.clear();//////TODO

    for (size_t i = 0; i < proc_frames.size(); ++i) {
      w << (proc_frames[i].get());
    }
    auto end = std::chrono::system_clock::now();
    auto diff = end - start;
    std::cout << "Finished with: " << diff.count() << " seconds." << std::endl;
    */
    } else if (argv[1][0] == 'b') { //Generate contour for binary black image
        Mat image; // Original image
        image = imread(argv[2]);
        if (image.empty()) {
            std::cout << "Error - could not read file " << argv[2] << "as image.\n";
            exit(2);
        }

				std::vector<std::vector<Point>> vertexes;

				cvtColor(image, image, COLOR_BGR2GRAY);
				image.convertTo(image, CV_8UC1);

				image = 255 - image;

				findContours(image, vertexes, RETR_EXTERNAL, CHAIN_APPROX_TC89_KCOS);
				std::fstream fs(string(argv[2]) + ".pof",
						std::fstream::in | std::fstream::out | std::fstream::trunc);
				if (!fs.is_open()) {
					std::cout << "Error not open\n";
					exit(3);
				}
				for (Point p : vertexes[0]) {
					fs << p.x << " " << p.y << "\n";
				}
    } else if (argv[1][0] == 'w') { //Generate contour for binary white image
        Mat image; // Original image
        image = imread(argv[2]);
        if (image.empty()) {
            std::cout << "Error - could not read file " << argv[2] << "as image.\n";
            exit(2);
        }

				std::vector<std::vector<Point>> vertexes;

				cvtColor(image, image, COLOR_BGR2GRAY);
				image.convertTo(image, CV_8UC1);

				findContours(image, vertexes, RETR_EXTERNAL, CHAIN_APPROX_TC89_KCOS);
				std::fstream fs(string(argv[2]) + ".pof",
						std::fstream::in | std::fstream::out | std::fstream::trunc);
				if (!fs.is_open()) {
					std::cout << "Error not open\n";
					exit(3);
				}

				size_t biggest = 0;
				size_t biggest_size = 0;
				for (size_t i = 0; i < vertexes.size(); ++i) {
					if (vertexes[i].size() > biggest_size) {
						biggest = i;
						biggest_size = vertexes[i].size();
					}
				}

				for (Point p : vertexes[biggest]) {
					fs << p.x << " " << p.y << "\n";
				}
		}
		return 0;
}
