#include <iostream>
#include <string>
#include <mutex>
#include <memory>

#include <opencv2/opencv.hpp>

#ifndef HAND_MODULE_CUSTOM_HPP
#define HAND_MODULE_CUSTOM_HPP

#include "mediapipe/framework/calculator_framework.h"
#include <mediapipe/framework/calculator_graph.h>

#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/landmark.pb.h"

#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/file_helpers.h"

#endif // HAND_MODULE_CUSTOM_HPP


class DetectHands{
    private:
        mediapipe::CalculatorGraph graph;
        std::vector<int> fTipsId = {4, 8, 12, 16, 20};          //mappings to mediapipes 3d coordinates
        std::vector<mediapipe::NormalizedLandmarkList> handLandmarks;
    public:
        DetectHands();
        void findHands(cv::Mat& img);
        void DisplayFrame(const cv::Mat& frame, std::mutex& mutexImshow);
        std::vector<int> numFingersUp();
};
void DetectHands::DisplayFrame(const cv::Mat& frame, std::mutex& mutexImshow) {
    std::lock_guard<std::mutex> lock(mutexImshow);
    cv::imshow("frameIn", frame);
}
DetectHands::DetectHands() {
    // Initialize the graph
    const std::string kGraphConfigFile = "mediapipe/graphs/hand_tracking/hand_tracking_desktop_live.pbtxt";
    mediapipe::StatusOr<mediapipe::CalculatorGraphConfig> config_status =
        mediapipe::ParseTextProto<mediapipe::CalculatorGraphConfig>(
            mediapipe::file::GetContents(kGraphConfigFile));
    graph.Initialize(config_status.ValueOrDie());
    graph.StartRun({});
}

void DetectHands::findHands(cv::Mat& img){
    // Convert the image from BGR to RGB
    cv::Mat imgRGB;
    cv::cvtColor(img, imgRGB, cv::COLOR_BGR2RGB);

    // Process the image with MediaPipe
    mediapipe::StatusOr<std::unique_ptr<mediapipe::OutputStreamPoller>> poller_status =
        graph.AddOutputStreamPoller("hand_landmarks");
    auto& poller = poller_status.ValueOrDie();
    graph.AddPacketToInputStream("input_video", mediapipe::Adopt(imgRGB).At(mediapipe::Timestamp(0)));

    mediapipe::Packet packet;
    if (poller.Next(&packet)) {
        auto& output_landmarks = packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();
        handLandmarks = output_landmarks;
    }
};

std::vector<int> DetectHands::numFingersUp() {
    std::vector<int> fingers;
    if (handLandmarks.empty()) return fingers;

    for (const auto& landmarks : handLandmarks) {
        std::vector<int> handFingers(5, 0);
        for (int i = 0; i < fTipsId.size(); ++i) {
            if (i == 0) {
                handFingers[i] = landmarks.landmark(fTipsId[i]).y() < landmarks.landmark(fTipsId[i] - 2).y() ? 1 : 0;
            } else {
                handFingers[i] = landmarks.landmark(fTipsId[i]).y() < landmarks.landmark(fTipsId[i] - 2).y() ? 1 : 0;
            }
        }
        fingers.push_back(handFingers);
    }
    return fingers;
}
