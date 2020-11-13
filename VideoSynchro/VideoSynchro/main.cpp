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
#include <chrono>   // hours, minutes, duration_cast
#include <stdio.h>
#include <cstdlib>

#include <cmath>
using namespace std;
using namespace cv;

int main(int argc, const char * argv[]) {
    // insert code here...
    namedWindow( "hello", WINDOW_AUTOSIZE );
    namedWindow( "hi", WINDOW_AUTOSIZE );
    
    struct stat t_stat_D;
    struct stat t_stat_G;
    stat("/Users/davidhuard/Desktop/IMG_1289.MOV", &t_stat_D);
    stat("/Users/davidhuard/Desktop/IMG_0015.MOV", &t_stat_G);
    struct tm * timeinfo_D = localtime(&t_stat_D.st_ctime);
    struct tm * timeinfo_G = localtime(&t_stat_G.st_ctime);
    printf("Image D time and date: %s", asctime(timeinfo_D));
    printf("Image G time and date: %s", asctime(timeinfo_G));
    
    auto durationD = std::chrono::hours(timeinfo_D->tm_hour) + std::chrono::minutes(timeinfo_D->tm_min) + std::chrono::seconds(timeinfo_D->tm_sec);
    auto durationG = std::chrono::hours(timeinfo_G->tm_hour) + std::chrono::minutes(timeinfo_G->tm_min) + std::chrono::seconds(timeinfo_G->tm_sec);
    auto sum = durationD - durationG;
    
    // output
    auto hours   = std::chrono::duration_cast<std::chrono::hours>(sum);
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(sum - hours);
    auto secondes = std::chrono::duration_cast<std::chrono::seconds>(sum - hours - minutes);

    tm  timeinfo_Diff;
    timeinfo_Diff.tm_hour = int(hours.count());
    timeinfo_Diff.tm_min  = int(minutes.count());
    timeinfo_Diff.tm_sec  = int(secondes.count());

    cout << "hour diff: " << timeinfo_Diff.tm_hour << endl;
    cout << "min diff: " << timeinfo_Diff.tm_min << endl;
    cout << "sec diff: " << timeinfo_Diff.tm_sec << endl;
    
    cv::VideoCapture cap_D("/Users/davidhuard/Desktop/IMG_1286.MOV");
          if(!cap_D.isOpened()) {
            std::cout << "Unable to open the camera D\n";
             std::exit(-1);
         }
    cv::VideoCapture cap_G("/Users/davidhuard/Desktop/IMG_0015.MOV");
          if(!cap_G.isOpened()) {
            std::cout << "Unable to open the camera G\n";
             std::exit(-1);
         }
    
    double fpsD = cap_D.get(CAP_PROP_FPS);
    cout << "Frames per second using video.get(CV_CAP_PROP_FPS) : " << fpsD << endl;
    double fpsG = cap_G.get(CAP_PROP_FPS);
    cout << "Frames per second using video.get(CV_CAP_PROP_FPS) : " << fpsG << endl;
    
    cv::Mat image_D;
    cv::Mat image_G;
    vector<Mat> images_D;
    vector<Mat> images_G;
 
    if(timeinfo_Diff.tm_sec >= 0)
    {
        
        for(int i =0; i<fpsD;i++)
        {
            cap_D >> image_D;
            if(image_D.empty()) {
                break;
            }
            if(i == int(fpsD/2))
            images_D.push_back(image_D.clone());
        }
   
        for(int i =0; i<fpsG+(fpsG*timeinfo_Diff.tm_sec);i++)
        {
            cap_G >> image_G;
            if(image_G.empty()) {
                break;
            }
            if( i >= (fpsG*timeinfo_Diff.tm_sec))
            {
                images_G.push_back(image_G.clone());
            }
          //  imshow( "hello", image_G );
        // if (waitKey(100) >= 0)
          //   break;
        }
        
        cout << images_G.size() << endl;
        cout << images_D.size() << endl;
        
        cout << images_G[0].size() << endl;
        cout << images_D[0].size() << endl;
        
        if(images_G[0].size() != images_D[0].size())
        {
            cout << "Not the same size" << endl;
            cv::resize(images_D[0], images_D[0], images_G[0].size());
        }
        
        cout << images_G[0].size() << endl;
        cout << images_D[0].size() << endl;
        
        Rect region_of_interest = Rect(images_D[0].cols/8, images_D[0].rows/8, images_D[0].cols*0.75, images_D[0].rows*0.75);
        Mat images_D_roi = images_D[0](region_of_interest);
        
        double maxScore = 0;
        int imageID = 0;
        
        for(size_t i = 0 ;i<images_G.size();i++)
        {
            int result_cols =  images_G[i].cols - images_D_roi.cols + 1;
            int result_rows = images_G[i].rows - images_D_roi.rows + 1;
            Mat result;
            result.create( result_rows, result_cols, CV_32FC1 );
            Mat imgG = images_G[i].clone();
            matchTemplate( imgG, images_D_roi, result, 5);
            //normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
            double minVal; double maxVal; Point minLoc; Point maxLoc;
            minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
            Point matchLoc = maxLoc;
            cout << "result " << i << " : " << maxVal << " - X: " << matchLoc.x << " - Y: " << matchLoc.y << endl;
            if(maxVal>maxScore)
            {
                maxScore=maxVal;
                imageID = int(i);
            }
        }
        cout << "MaxAtID: " << imageID << endl;
        //rectangle( images_G[i], matchLoc, Point( matchLoc.x + images_D_roi.cols , matchLoc.y + images_D_roi.rows ), Scalar::all(255), 2, 8, 0 );
           // imshow( "hello", images_G[i] );
           // if (waitKey(200) >= 0)
           //     break;
           // string filename = "/Users/davidhuard/Desktop/test/Image" + to_string(i) + ".png";
           // cv::imwrite(filename.c_str(), images_G[i]);
        //}
        // imshow( "hi", images_D_roi);
        // cout << images_D_roi.cols << " - " << images_D_roi.rows << endl;
        double timeCut = imageID*fpsG/1000;
        
        double timeCutrounded = std::floor((timeCut * 100) + .5) / 100;
        
        cout << "MaxAtID: " << timeCutrounded << endl;
        
        string time = "/usr/local/bin/ffmpeg -i /Users/davidhuard/Desktop/IMG_0015.MOV -ss 00:00:"+ to_string(timeCutrounded) +" -async 1 /Users/davidhuard/Desktop/IMG_0015_out.MOV";
        
        system(time.c_str());
    }
    
    
    
   //system("/usr/local/bin/ffmpeg -i /Users/davidhuard/Desktop/IMG_1289.MOV -ss 00:00:01.100 -t 00:00:01.500 -async 1 /Users/davidhuard/Desktop/IMG_1289_out.MOV");
   // waitKey(0);
    
    //system("/usr/local/bin/ffmpeg -i /Users/davidhuard/Desktop/IMG_0015.MOV -vf \"[in] scale=iw/2:ih/2, pad=2*iw:ih [left]; movie=/Users/davidhuard/Desktop/IMG_1286.MOV, scale=iw/3:ih/3, fade=out:300:30:alpha=1 [right]; [left][right] overlay=main_w/2:0 [out]\" -b:v 768k /Users/davidhuard/Desktop/sbs.MOV");
    
   // system("/usr/local/bin/ffmpeg -i /Users/davidhuard/Desktop/IMG_0015.MOV -vf \"movie=/Users/davidhuard/Desktop/IMG_0015.MOV [in1]; [in]pad=3*iw:ih[in0]; [in0][in1] overlay=main_w/2:0 [out]\" /Users/davidhuard/Desktop/sbs.MOV");
    waitKey(100);
    
    //system("/usr/local/bin/ffmpeg -i /Users/davidhuard/Desktop/IMG_1286.MOV -vf scale=720:960 /Users/davidhuard/Desktop/IMG_1286_res.MOV");
    
    waitKey(100);
    
    system("/usr/local/bin/ffmpeg -i /Users/davidhuard/Desktop/IMG_0015_out.MOV -i /Users/davidhuard/Desktop/IMG_1286_res.MOV -filter_complex '[0:v]pad=iw*2:ih[int];[int][1:v]overlay=W/2:0[vid]' -map [vid] -c:v libx264 -crf 23 -preset veryfast /Users/davidhuard/Desktop/sbsSync.MOV");
    
    return 0;
}
