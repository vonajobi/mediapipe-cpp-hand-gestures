// MediaPipe Hand Gesture Solutions for C++ #
This repo is a starting guide for C++ MediaPipe Hand Landmarker on Windows OS.
# Getting Started
When following the installation for Windows instructions on the [MediaPipe Website](https://ai.google.dev/edge/mediapipe/framework/getting_started/#installing_on_windows)
if you already have MYSYS2 installed, you may not have pacman installed. Git for windows doesn't come with package manager built in.
refernce [StackOverflow](https://stackoverflow.com/questions/32712133/package-management-in-git-for-windows-git-bash/60611888#60611888)
### Pacman Installation
Refrence this [github repo] for pacman installation and unpacking if not already installed
### MediaPipe Hello world build
First make sure you can properly run hello world.
```
C:\Users\<Username>\mediapipe_repo>bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 --action_env PYTHON_BIN_PATH="C://<PATH_TO_PYTHON_PROGRAM>//python.exe" mediapipe/examples/desktop/hello_world

C:\Users\<Username>\mediapipe_repo>set GLOG_logtostderr=1

C:\Users\<Username>\mediapipe_repo>bazel-bin\mediapipe\examples\desktop\hello_world\hello_world.exe

//should print:
// I20200514 20:43:12.277598  1200 hello_world.cc:56] Hello World!
// I20200514 20:43:12.278597  1200 hello_world.cc:56] Hello World!
// I20200514 20:43:12.279618  1200 hello_world.cc:56] Hello World!
// I20200514 20:43:12.279618  1200 hello_world.cc:56] Hello World!
// I20200514 20:43:12.279618  1200 hello_world.cc:56] Hello World!
// I20200514 20:43:12.279618  1200 hello_world.cc:56] Hello World!
// I20200514 20:43:12.279618  1200 hello_world.cc:56] Hello World!
// I20200514 20:43:12.279618  1200 hello_world.cc:56] Hello World!
// I20200514 20:43:12.279618  1200 hello_world.cc:56] Hello World!
// I20200514 20:43:12.280613  1200 hello_world.cc:56] Hello World!
```
# MediaPipe Hand Tracking example build
If you want to try the legacy handtracking example build make sure to run as administrator or elevated platform for the bazel build of TensorFlow to be successful. YOU MUST CHANGE THE BUILD FILE IN UTILS FOR THIS TO BE SUCCESSFUL.
Also make sure to change the build config in opencv_Windows.BUILD to match your opencv version. For example,

**opencv_Windows.Build**
```
# The following build rule assumes that the executable "opencv-4.10-vc16.exe"
# is downloaded and the files are extracted to local.
# If you install OpenCV separately, please modify the build rule accordingly.
cc_library(
    name = "opencv",
    srcs = select({
        ":opt_build": [
            "x64/vc16/lib/opencv_world" + OPENCV_VERSION + ".lib",
            "x64/vc16/bin/opencv_world" + OPENCV_VERSION + ".dll",
        ],
        ":dbg_build": [
            "x64/vc16/lib/opencv_world" + OPENCV_VERSION + "d.lib",
            "x64/vc16/bin/opencv_world" + OPENCV_VERSION + "d.dll",
        ],
    }),
```
**..\mediapipe\util\BUILD**
```
# about line 235
# changed to "resource_util_default.cc" from "resource_util_windows.cc"
        "//mediapipe:windows": ["resource_util_default.cc"],
```
If using OpenCv 4.x, there will be some compatability issues. Make sure to replace line 88 in opencv_video_inc.h with `#include <opencv2/video/tracking.hpp>` and comment out lines 90-94. 

**mediapipe/.bazelrc**
```
# starts about line 23

# windows
build:windows --cxxopt=/std:c++17
build:windows --host_cxxopt=/std:c++17
build:windows --copt=/w
# added these configs
build:windows --copt=/DBOOST_ERROR_CODE_HEADER_ONLY						
build:windows --copt=/DMESA_EGL_NO_X11_HEADERS							
build:windows --copt=/DEGL_NO_X11
# For using M_* math constants on Windows with MSVC.
build:windows --copt=/D_USE_MATH_DEFINES
build:windows --host_copt=/D_USE_MATH_DEFINES
# added this config
build:windows --define MEDIAPIPE_DISABLE_GPU=1
```
## Now to run:
### First buld
```
Start-Process powershell -Verb runAs set GLOG_logtostderr=1 bazel build --config=windows //mediapipe/examples/desktop/hand_tracking:hand_tracking_cpu
``` 
### Then run
```
bazel-bin/mediapipe/examples/desktop/hand_tracking/hand_tracking_cpu  --calculator_graph_config_file=mediapipe/graphs/hand_tracking/hand_tracking_desktop_live.pbtxt
```
