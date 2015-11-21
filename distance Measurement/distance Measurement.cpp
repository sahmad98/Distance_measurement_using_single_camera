// distance Measurement.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cv.h"
#include "highgui.h"
#include "opencv2\imgproc\imgproc_c.h"
#include<fstream>
#include "math.h"

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;
	using namespace cv;

	Mat img, img_gray, channel[3];
	VideoCapture cam(1);
	double distance = 0;
	
	//FILE *data;
	//data = fopen("data320.csv","a");
	
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	cam.set(CV_CAP_PROP_CONVERT_RGB, 1);
	namedWindow("Frame", WINDOW_AUTOSIZE);
	while(waitKey(10) != 'a')
	{
		cam >> img;
		cvtColor(img, img_gray, COLOR_RGB2GRAY);
		split(img, channel);
		subtract(channel[2], img_gray, img_gray);
		//convertScaleAbs(img, img);
		threshold(img_gray, img_gray, 90, 255, THRESH_BINARY);

		erode(img_gray, img_gray, Mat(), Point(-1,-1), 4);
		dilate(img_gray, img_gray, Mat(), Point(-1,-1), 4);

		vector<vector<Size>> contors;
		vector<Vec4i> heirarcy;
		findContours(img_gray, contors, heirarcy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

		vector<Rect> boundRect(contors.size());
		vector<vector<Point>> contor_poly(contors.size());
	
		for(int i =0; i< contors.size(); i++)
		{
			approxPolyDP(Mat(contors[i]), contor_poly[i], 3, true);
			boundRect[i] = boundingRect(Mat(contor_poly[i]));
		}
		int max_index = 0,max_area = 0;
		for(int i =0; i< boundRect.size(); i++)
		{
			int a = boundRect[i].area(); 
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(255,255,0), 2, 8, 0);
			if( a > max_area)
			{ 
				max_area = a;
				max_index = i;
			}
		}
		int confidence = 0;
		for(int i=0; i< boundRect.size(); i++)
		{
			if((boundRect[i].x < boundRect[max_index].x + boundRect[max_index].width && boundRect[i].x > boundRect[max_index].x - int(0.1*boundRect[max_index].width)) && (boundRect[i].y > boundRect[max_index].y) )
				confidence += 45; 
			
		}
		if(boundRect.size() > 0)
		{	
			if(confidence > 99)
				confidence = 0;
			//try{
			//Mat sub_image = Mat(img, Rect(max(boundRect[max_index].x-30, 0), max(boundRect[max_index].y-30, 0), min(int(boundRect[max_index].width*1.75), img.cols - boundRect[max_index].x+30), min(boundRect[max_index].height*3, img.rows - boundRect[max_index].y+30)));
			//imshow("Frame", sub_image);
			//}catch(int e){
			//	cout<<"Error occured"<<endl;
			//}
			rectangle(img, boundRect[max_index].tl(), boundRect[max_index].br(), Scalar(0,255,0), 2, 8, 0);
			
			//fprintf(data,"%d , %d , %d\n", boundRect[max_index].width, boundRect[max_index].height, boundRect[max_index].area());
			distance = 8414.7*pow(boundRect[max_index].area(), -0.468);
			cout << distance << " cm." <<" Confidence: "<<confidence<<endl;
			imshow("Frame", img);
			
		}
		
		else
			imshow("Frame", img);
	}
	

	
	//fflush(data);
	//fclose(data);
	cam.release();
	return 0;
}

