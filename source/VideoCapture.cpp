#include "../header/VideoCapture.h"

using namespace cv;
using namespace std;

void detectAndDisplay(const Mat& frame,CascadeClassifier& faceCascade)
{
    std::vector<Rect> faces; // Вектор для хранения обнаруженных лиц
    Mat frameGray; // Матрица для хранения кадра в оттенках серого

    cvtColor(frame, frameGray, COLOR_BGR2GRAY); // Преобразование кадра в оттенки серого
    equalizeHist(frameGray, frameGray); // Выравнивание гистограммы для улучшения контрастности

    faceCascade.detectMultiScale(frameGray, faces, 1.1, 10, 0 | CASCADE_SCALE_IMAGE, Size(30, 30)); // Обнаружение лиц

    for (size_t i = 0; i < faces.size(); i++)
    {
        Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2); // Центр лица
        ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(0, 0, 255), 2); // Рисование эллипса вокруг лица
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

    String window_name = "ShakalKamera";
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

        if (waitKey(10) == 27) // 27 its ESC key to exit
        {
            cout << "ESC key is pressed by user. Exiting the program" << endl;
            break;
        }
    }
}