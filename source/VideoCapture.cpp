#include "../header/VideoCapture.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/objdetect.hpp>

using namespace cv;
using namespace std;

void playVideo()
{
    //open the video file for reading
    VideoCapture cap("C:/ResourcesCV/eblan.mp4");

    // if not success, exit program
    if (cap.isOpened() == false)
    {
        cout << "Cannot open the video file" << endl;
        cin.get(); //wait for any key press
    }

    double fps = cap.get(CAP_PROP_FPS);
    cout << "Frames per seconds : " << fps << endl;

    String window_name = "My First Video";

    namedWindow(window_name, WINDOW_NORMAL); //create a window

    while (true)
    {
        Mat frame;
        bool bSuccess = cap.read(frame); // read a new frame from video 

        //Breaking the while loop at the end of the video
        if (bSuccess == false)
        {
            cout << "Found the end of the video" << endl;
            break;
        }

        //show the frame in the created window
        imshow(window_name, frame);

        if (waitKey(10) == 27)
        {
            cout << "Esc key is pressed by user. Stoppig the video" << endl;
            break;
        }
    }


}