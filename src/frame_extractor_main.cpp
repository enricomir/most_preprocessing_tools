#include <iostream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

using namespace cv;

void save(const Mat& frame, bool bw = false, double resize_factor = 1.0, std::string fname = "");

const char* WHNDL = "Video";

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 3) {
        std::cout << "Error! Usage: ./frame_extractor <vid> or ./frame_extractor <vid> <n_frames>\n";
        exit(1);
    }
    VideoCapture vid(argv[1]);
    double max_frames = vid.get(CAP_PROP_FRAME_COUNT);
    std::cout << "Succesfully loaded " << argv[1] << " with " << max_frames
              << " frames.\n";

    if (argc == 2) {
        namedWindow(WHNDL, WINDOW_NORMAL);

        Mat frame;

        // cvtColor(frame, frame, COLOR_BGR2GRAY);
        // resize(frame, frame, Size(0, 0), 0.25, 0.25, INTER_AREA);

        int cur_frame = 0;

        vid.set(CAP_PROP_POS_FRAMES, cur_frame);
        vid >> frame;
        imshow(WHNDL, frame);

        double size_factor;
        char c;
        while ((c = waitKey())) {
            switch (c) {
            case 'q':
                return 1;
            case 'n':
                cur_frame += 10;
                break;
            case 'N':
                cur_frame += 60;
                break;
            case 'm':
                cur_frame += 180;
                break;
            case 'M':
                cur_frame += 360;
                break;
            case 'p':
                cur_frame -= 10;
                break;
            case 'P':
                cur_frame -= 60;
                break;
            case 'o':
                cur_frame -= 180;
                break;
            case 'O':
                cur_frame -= 360;
                break;

            case 'c':
                save(frame);
                break;
            case 'b':
                save(frame, true);
                break;
            case 'z':
                std::cout << "Please enter resize factor: ";
                std::cin >> size_factor;
                save(frame, false, size_factor);
                break;
            case 'Z':
                std::cout << "Please enter resize factor: ";
                std::cin >> size_factor;
                save(frame, true, size_factor);
                break;
            default:
                break;
            }

            if (cur_frame < 0)
                cur_frame = 0;
            if (cur_frame >= max_frames)
                cur_frame = max_frames - 1;
            std::cout << static_cast<int>(cur_frame / max_frames * 100) << "\%" << std::endl;
            vid.set(CAP_PROP_POS_FRAMES, cur_frame);
            vid >> frame;
            imshow(WHNDL, frame);
        }
    } else if (argc == 3) {
        Mat frame;
        int each = max_frames / std::stoi(argv[2]);
        int cur_frame = 0;
        std::string fname;
        int i = 0;

        while (cur_frame < max_frames) {
            vid >> frame;

            if ((cur_frame % each) == 0) {
                fname = argv[1];
                fname.pop_back();
                fname.pop_back();
                fname.pop_back();
                fname.pop_back();
                fname += "_";
                fname += std::to_string(i);
                fname += ".png";
                std::cout << "Writing with " << fname << "\n";
                save(frame, false, 1.0, fname);
                i++;
            }
            cur_frame++;
        }
    }
    return 0;
}

void save(const Mat& frame, bool bw, double resize_factor, std::string fname)
{
    Mat local;

    if (fname == "") {
        std::cout << "Please input filename: ";
        std::cin >> fname;
    }

    if (resize_factor != 1.0) {
        resize(frame, local, Size(0, 0), resize_factor, resize_factor, INTER_AREA);
    } else {
        local = frame;
    }

    if (bw == true) {
        cvtColor(local, local, COLOR_BGR2GRAY);
    }

    imwrite(fname, local);
    std::cout << "Written succesfully" << std::endl;
}