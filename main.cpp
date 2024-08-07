#include <iostream>
#include <chrono>
#include <cmath>
#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include <WinDNS.h>

#include "hand_module_custom.hpp"
#include "absl/status/status.h"




int main(int argc, char** argv) {
    cv::VideoCapture cap(0);

    if (!cap.isOpened()) {
        std::cerr << "Error opening video stream or file" << std::endl;
        return -1;
    }

    DetectHands detector;

    while (true) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        detector.findHands(frame);

        std::vector<int> fingers = detector.numFingersUp();
        for (int i = 0; i < fingers.size(); ++i) {
            std::cout << "Finger " << i << ": " << (fingers[i] ? "Up" : "Down") << std::endl;
        }
        cv::imshow("Frame", frame);
        if (cv::waitKey(1) == 27 || cv::waitKey(1) == 8) break; // Exit on 'Esc' or 'Backspace' key
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}


