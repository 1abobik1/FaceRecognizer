#include "../header/VideoCapture.h"

using namespace cv;
using namespace std;

void detectAndDisplay(Mat frame, CascadeClassifier faceCascade)
{
    std::vector<Rect> faces;
    Mat frameGray;

    cvtColor(frame, frameGray, COLOR_BGR2GRAY);
    equalizeHist(frameGray, frameGray);

    faceCascade.detectMultiScale(frameGray, faces, 1.1, 10, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

    for (size_t i = 0; i < faces.size(); i++)
    {
        Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 2);
    }

    imshow("Capture - Face detection", frame);
}

void faceDetRealtime()
{
    VideoCapture cap(0);

    if (!cap.isOpened())
    {
        cout << "Cannot open the video camera" << endl;
        return;
    }

    CascadeClassifier faceCascade;
    if (!faceCascade.load("C:/ResourcesCV/haarcascade_frontalface_default.xml"))
    {
        cout << "Error loading face cascade file" << endl;
        return;
    }

    String window_name = "Face Detection in Real-Time";
    namedWindow(window_name, WINDOW_NORMAL);

    while (true)
    {
        Mat frame;
        bool bSuccess = cap.read(frame);

        if (!bSuccess)
        {
            cout << "Video camera is disconnected" << endl;
            break;
        }

        if (frame.empty())
        {
            cout << "No captured frame" << endl;
            break;
        }

        detectAndDisplay(frame, faceCascade);

        if (waitKey(10) == 27) // ESC key to exit
        {
            cout << "ESC key is pressed by user. Exiting the program" << endl;
            break;
        }
    }
}