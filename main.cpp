#include <iostream>
#include <chrono>
#include <cmath>
#include <mutex>
#include <memory>
#include <string>
#include <vector>

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/calculator_graph.h"

#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/landmark.pb.h"

#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "absl/status/status.h"

#include "serial/serial.h"

absl::Status InitializeGraph(const std::string& pathGraph, mediapipe::CalculatorGraph& graph) {
    std::string protoG;
    MP_RETURN_IF_ERROR(mediapipe::file::GetContents(pathGraph, &protoG));
    
    mediapipe::CalculatorGraphConfig config;
    if (!mediapipe::ParseTextProto(protoG, &config)) {
        return absl::InternalError("Cannot parse the graph config !");
    }

    MP_RETURN_IF_ERROR(graph.Initialize(config));
    return absl::OkStatus();
}

void DisplayFrame(const cv::Mat& frame, std::mutex& mutexImshow) {
    std::lock_guard<std::mutex> lock(mutexImshow);
    cv::imshow("frameIn", frame);
}

absl::Status RunGraph(mediapipe::CalculatorGraph& graph, std::atomic<bool>& flagStop, std::mutex& mutexImshow) {
    // Start the camera and check that it works
    cv::VideoCapture cap(cv::CAP_ANY);
    if (!cap.isOpened()) {
        return absl::NotFoundError("CAN'T OPEN CAMERA");
    }

    cv::Mat frameIn, frameInRGB;
    for (int i = 0; !flagStop; ++i) {
        cap.read(frameIn);
        if (frameIn.empty()) {
            return absl::NotFoundError("Can't open Camera");
        }

        std::cout << "SIZE_IN = " << frameIn.size() << std::endl;
        DisplayFrame(frameIn, mutexImshow);

        // Convert to a packet and send
        cv::cvtColor(frameIn, frameInRGB, cv::COLOR_BGR2RGB);
        auto inputFrame = std::make_unique<mediapipe::ImageFrame>(
            mediapipe::ImageFormat::SRGB, frameInRGB.cols, frameInRGB.rows, mediapipe::ImageFrame::kDefaultAlignmentBoundary);
        frameInRGB.copyTo(mediapipe::formats::MatView(inputFrame.get()));

        MP_RETURN_IF_ERROR(graph.AddPacketToInputStream("in", mediapipe::Adopt(inputFrame.release()).At(mediapipe::Timestamp(i))));

        if (27 == cv::waitKey(1) || 8 == cv::waitKey(1)) {
            std::cout << "It's time to quit!" << std::endl;
            flagStop = true;
        }
    }

    MP_RETURN_IF_ERROR(graph.CloseInputStream("in"));
    MP_RETURN_IF_ERROR(graph.WaitUntilDone());
    return absl::OkStatus();
}

absl::Status Run(const std::string& pathGraph) {
    mediapipe::CalculatorGraph graph;
    MP_RETURN_IF_ERROR(InitializeGraph(pathGraph, graph));

    std::mutex mutexImshow;
    std::atomic<bool> flagStop(false);

    auto cb = [&mutexImshow, &flagStop](const mediapipe::Packet& packet) -> absl::Status {
        const auto& outputFrame = packet.Get<mediapipe::ImageFrame>();
        cv::Mat ofMat = mediapipe::formats::MatView(&outputFrame);
        cv::Mat frameOut;
        cv::cvtColor(ofMat, frameOut, cv::COLOR_RGB2BGR);
        std::cout << packet.Timestamp() << ": RECEIVED VIDEO Packet size " << frameOut.size() << std::endl;
        DisplayFrame(frameOut, mutexImshow);
        return absl::OkStatus();
    };

    MP_RETURN_IF_ERROR(graph.ObserveOutputStream("out", cb));
    MP_RETURN_IF_ERROR(graph.StartRun({}));

    // Add poller for landmark_lists
    mediapipe::OutputStreamPoller poller;
    MP_RETURN_IF_ERROR(graph.AddOutputStreamPoller("landmark_lists", &poller));
    
    // Run the graph
    MP_RETURN_IF_ERROR(RunGraph(graph, flagStop, mutexImshow));
    
    // Get output packets
    mediapipe::Packet packet;
    while (poller.Next(&packet)) {
        const auto& landmarks = packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();
        for (const auto& landmarkList : landmarks) {
            for (const auto& landmark : landmarkList.landmark()) {
                std::cout << "Landmark: (" << landmark.x() << ", " << landmark.y() << ", " << landmark.z() << ")" << std::endl;
            }
        }
    }

    return absl::OkStatus();
}

int main(int argc, char** argv) {
    FLAGS_alsologtostderr = 1;
    google::SetLogDestination(google::GLOG_INFO, ".");
    google::InitGoogleLogging(argv[0]);

    std::cout << "My Hands" << std::endl;
    auto status = Run("C:/Users/3crow/coding_projects/mediapipe_repo/mediapipe/mediapipe/graphs/hand_tracking/hand_tracking_desktop_live.pbtxt");
    std::cout << "status = " << status << std::endl;
    std::cout << "status.ok() = " << status.ok() << std::endl;

    return 0;
}


