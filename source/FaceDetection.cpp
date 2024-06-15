#include <opencv2/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

#include "../header/FaceDetection.h"

using namespace cv;

void faceSearch()
{
	Mat image = imread("C:/ResourcesCV/dvoe2.jpg");

	CascadeClassifier faceCascade;
	faceCascade.load("C:/ResourcesCV/haarcascade_frontalface_default.xml");

	if (faceCascade.empty()) {
		std::cout << "xml file not loaded.." << std::endl;
	}

	std::vector<Rect> faces;

	faceCascade.detectMultiScale(image, faces, 1.1, 10);

	for (int i = 0; i < faces.size(); ++i) {
		rectangle(image,faces[i].tl(), faces[i].br(), Scalar(0, 0, 255), 2);
	}

	String imageName = "human";
	namedWindow(imageName, WINDOW_NORMAL);
	imshow(imageName, image);
	waitKey(0);
}
