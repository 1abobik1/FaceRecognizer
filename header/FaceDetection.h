#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/objdetect.hpp>


void detectAndDisplay(const cv::Mat& frame,cv::CascadeClassifier& faceCascade);

void faceDetRealtime();
