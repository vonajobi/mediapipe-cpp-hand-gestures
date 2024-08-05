cc_binary (
    name = "handTrackingCustom",
    srcs = ["main.cpp"],
    data = [ 
        "mediapipe/examples/handTrackingCustom/hand_tracking_custom.pbtxt",
        "mediapipe/examples/handTrackingCustom/hand_render_custom.pbtxt",
        ], 
    deps = [   
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework:calculator_graph",
        "//mediapipe/framework/formats:image_frame_opencv",
        "//mediapipe/framework/formats:image_frame",
         "//mediapipe/framework/formats:landmark_cc_proto",  # Use cc_proto_library
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:status",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:opencv_highgui",
        "//mediapipe/framework/port:opencv_imgproc",
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "@com_google_protobuf//:protobuf",
        "@com_google_absl//absl/status:status",
    ],
)
