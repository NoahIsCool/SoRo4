# ===================================================================================
#  aruco CMake configuration file
#
#             ** File generated automatically, do not modify **
#
#  Usage from an external project:
#    In your CMakeLists.txt, add these lines:
#
#    FIND_PACKAGE(aruco REQUIRED )
#    TARGET_LINK_LIBRARIES(MY_TARGET_NAME )
#
#    This file will define the following variables:
#      - aruco_LIBS          : The list of libraries to links against.
#      - aruco_LIB_DIR       : The directory where lib files are. Calling LINK_DIRECTORIES
#                                with this path is NOT needed.
#      - aruco_VERSION       : The  version of this PROJECT_NAME build. Example: "1.2.0"
#      - aruco_VERSION_MAJOR : Major version part of VERSION. Example: "1"
#      - aruco_VERSION_MINOR : Minor version part of VERSION. Example: "2"
#      - aruco_VERSION_PATCH : Patch version part of VERSION. Example: "0"
#
# ===================================================================================
INCLUDE_DIRECTORIES("/home/reza/Desktop/SoRo/aruco-3.1.2/aruco_src/include")
INCLUDE_DIRECTORIES("/home/reza/Desktop/SoRo/aruco-3.1.2/aruco_src/include/aruco")
SET(aruco_INCLUDE_DIRS "/home/reza/Desktop/SoRo/aruco-3.1.2/aruco_src/include")

LINK_DIRECTORIES("/home/reza/Desktop/SoRo/aruco-3.1.2/aruco_src/lib")
SET(aruco_LIB_DIR "/home/reza/Desktop/SoRo/aruco-3.1.2/aruco_src/lib")

SET(aruco_LIBS opencv_calib3d;opencv_core;opencv_dnn;opencv_features2d;opencv_flann;opencv_highgui;opencv_imgcodecs;opencv_imgproc;opencv_ml;opencv_objdetect;opencv_photo;opencv_shape;opencv_stitching;opencv_superres;opencv_video;opencv_videoio;opencv_videostab;opencv_viz aruco) 

SET(aruco_FOUND YES)
SET(aruco_FOUND "YES")
SET(aruco_VERSION        3.1.2)
SET(aruco_VERSION_MAJOR  3)
SET(aruco_VERSION_MINOR  1)
SET(aruco_VERSION_PATCH  2)
