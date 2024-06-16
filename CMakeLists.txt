cmake_minimum_required(VERSION 3.17)
project(Facerecognizer)
find_package(OpenCV CONFIG REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_STANDARD 20)

add_executable(Facerecognizer main.cpp FaceDetection.cpp VideoCapture.cpp FaceDetection.h VideoCapture.h)
target_link_libraries(Facerecognizer ${OpenCV_LIBS})