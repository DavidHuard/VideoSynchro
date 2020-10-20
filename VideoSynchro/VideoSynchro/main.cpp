//
//  main.cpp
//  VideoSynchro
//
//  Created by David Huard on 2020-10-20.
//

#include <iostream>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
using namespace std;
using namespace cv;

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    struct stat t_stat;
        stat("/Users/davidhuard/Desktop/IOL image/Patient1 Stereo/patient1.mov", &t_stat);
        struct tm * timeinfo = localtime(&t_stat.st_ctime); // or gmtime() depending on what you want
        printf("File time and date: %s", asctime(timeinfo));
    
    cout << "hour :" << timeinfo->tm_hour << endl;
    cout << "min :" << timeinfo->tm_min << endl;
    cout << "sec :" << timeinfo->tm_sec << endl;
    
    
    // Open the first camera attached to your computer
    cv::VideoCapture cap("/Users/davidhuard/Desktop/IOL image/Patient1 Stereo/patient1.mov");
          if(!cap.isOpened()) {
            std::cout << "Unable to open the camera\n";
             std::exit(-1);
         }

     cv::Mat image;
     vector<Mat> images;
     double FPS = 30.0;
     // Read camera frames (at approx 24 FPS) and show the result
    int j =0;
    while(true && j < 1000) {
         cap >> image;
         if(image.empty()) {
             std::cout << "Can't read frames from your camera\n";
             break;
         }
         images.push_back(image);
        j++;
    }

    for(int i =0; i< images.size(); i++)
    {
        cv::imshow("Camera feed", images[i]);

        // Stop the camera if users presses the "ESC" key
        if(cv::waitKey(1000.0/FPS) == 27) break;
    }
    
    
    return 0;
}
