#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include "Target.h"
#include "Target.cpp"

//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 400;
const int MAX_NUM_OBJECTS=50;
const int MIN_OBJECT_AREA = 30*30;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";

string intToString(int number){
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void drawObject(vector<Target> Targets,Mat &frame){
	for(int i =0; i<Targets.size(); i++){
		if (Targets.at(i).getXPos() >= 310 && Targets.at(i).getXPos() <= 330 && Targets.at(i).getYPos() >= 190 && Targets.at(i).getYPos() <= 210 )
		{
			cv::rectangle(frame,cv::Point(Targets.at(i).getXPos()-10,Targets.at(i).getYPos()-10),cv::Point(Targets.at(i).getXPos()+10,Targets.at(i).getYPos()+10),cv::Scalar(0,0,255));
			cv::putText(frame,intToString(Targets.at(i).getXPos())+ " , " + intToString(Targets.at(i).getYPos()),cv::Point(Targets.at(i).getXPos(),Targets.at(i).getYPos()+20),0,0.3,Scalar(0,0,255));
			cv::putText(frame,Targets.at(i).getType(),cv::Point(Targets.at(i).getXPos(),Targets.at(i).getYPos()-30),2,0.5,Scalar(0,0,255));
			cv::putText(frame,"Target Locked: "+ Targets.at(i).getType(),cv::Point(10,20),0,0.5,Scalar(0,0,255));
		}
		else{
			cv::rectangle(frame,cv::Point(Targets.at(i).getXPos()-10,Targets.at(i).getYPos()-10),cv::Point(Targets.at(i).getXPos()+10,Targets.at(i).getYPos()+10),cv::Scalar(0,255,0));
			cv::putText(frame,intToString(Targets.at(i).getXPos())+ " , " + intToString(Targets.at(i).getYPos()),cv::Point(Targets.at(i).getXPos(),Targets.at(i).getYPos()+20),0,0.3,Scalar(0,255,0));
			cv::putText(frame,Targets.at(i).getType(),cv::Point(Targets.at(i).getXPos(),Targets.at(i).getYPos()-30),2,0.5,Targets.at(i).getColour());
		}
	}
}
void drawLine(Mat &frame){
    cv::line(frame,cv::Point(0,200),cv::Point(640,200),cv::Scalar(0,0,0));
    cv::line(frame,cv::Point(320,0),cv::Point(320,120),cv::Scalar(0,0,0),3);
    cv::line(frame,cv::Point(0,200),cv::Point(240,200),cv::Scalar(0,0,0),3);
    cv::line(frame,cv::Point(400,200),cv::Point(640,200),cv::Scalar(0,0,0),3);
    cv::line(frame,cv::Point(320,0),cv::Point(320,400),cv::Scalar(0,0,0));
    int Range = 0;
    for (int i = 200; i < 400; i += 15.5)
    {

        cv::line(frame,cv::Point(313,i),cv::Point(327,i),cv::Scalar(0,0,0));
	if(Range % 100 ==0 && Range!=0){
	     cv::putText(frame,intToString(Range)+"m.",cv::Point(330,i),2,0.25,Scalar(0,0,0));
	}
	Range += 50;
    }

}
void morphOps(Mat &thresh){

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);


	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);



}
void trackFilteredObject(Mat threshold,Mat HSV, Mat &cameraFeed){

	vector <Target> orange_cans;

	Mat temp;
	threshold.copyTo(temp);
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;
				if(area>MIN_OBJECT_AREA){

					Target orange_can;
					
					orange_can.setXPos(moment.m10/area);
					orange_can.setYPos(moment.m01/area);
					

					orange_cans.push_back(orange_can);

					objectFound = true;

				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				//draw object location on screen
				drawObject(orange_cans,cameraFeed);}

		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}
void trackFilteredObject(Target thePerson,Mat threshold,Mat HSV, Mat &cameraFeed){

	
	vector <Target> orange_cans;
	drawLine(cameraFeed);
	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA){

					Target orange_can;
					
					orange_can.setXPos(moment.m10/area);
					orange_can.setYPos(moment.m01/area);
					orange_can.setType(thePerson.getType());
					orange_can.setColour(thePerson.getColour());

					orange_cans.push_back(orange_can);

					objectFound = true;

				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				//draw object location on screen
				drawObject(orange_cans,cameraFeed);}

		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}
int main(int argc, char* argv[])
{
	Mat cameraFeed;
	Mat threshold;
	Mat HSV;

	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open(0);
	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while(1){
		//store image to matrix
		capture.read(cameraFeed);
		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
		//create some temp fruit objects so that
		//we can use their member functions/information
		Target orange_can("orange can"), blue_can("blue can");

		
		//first find orange_cans
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
		inRange(HSV,orange_can.getHSVmin(),orange_can.getHSVmax(),threshold);
		morphOps(threshold);
		trackFilteredObject(orange_can,threshold,HSV,cameraFeed);
		//then blue_cans
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
		inRange(HSV,blue_can.getHSVmin(),blue_can.getHSVmax(),threshold);
		morphOps(threshold);
		trackFilteredObject(blue_can,threshold,HSV,cameraFeed);

		//show frames 
		//imshow(windowName2,threshold);
		imshow(windowName,cameraFeed);
		//imshow(windowName1,HSV);


		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}






	return 0;
}
