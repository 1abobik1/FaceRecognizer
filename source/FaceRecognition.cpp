#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>

#include <libpq-fe.h>

#include <iostream>
#include <fstream>


#include "../header/DataBase.h"
#include "../header/FaceRecognition.h"

using namespace cv;
using namespace cv::face;
using namespace std;

void faceRecognition()
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

    Ptr<LBPHFaceRecognizer> model = LBPHFaceRecognizer::create();
    model->read("face_model.xml");


    string window_name = "Face Recognition";
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

        Mat frameGray;
        cvtColor(frame, frameGray, COLOR_BGR2GRAY);
        equalizeHist(frameGray, frameGray);

        vector<Rect> faces;
        faceCascade.detectMultiScale(frameGray, faces, 1.1, 10, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

        for (size_t i = 0; i < faces.size(); i++)
        {
            Mat faceROI = frameGray(faces[i]);
            int label;
            double confidence;
            model->predict(faceROI, label, confidence);

            if (confidence < 50) // ѕороговое значение уверенности
            {
                // Ќайдено лицо в базе данных
                cout << "Recognized ID: " << label << " with confidence: " << confidence << endl;
            }
            else
            {
                // Ћицо не найдено в базе данных, добавление нового пользовател€
                string name, surname;
                cout << "Enter name: ";
                cin >> name;
                cout << "Enter surname: ";
                cin >> surname;
                Database::getDatabase()->getUserTable()->saveFaceToDB(faceROI, name, surname);
            }

            Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
            ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(0, 0, 255), 2);
        }

        imshow(window_name, frame);

        if (waitKey(10) == 27) // ESC key to exit
        {
            cout << "ESC key is pressed by user. Exiting the program" << endl;
            break;
        }
    }

}
