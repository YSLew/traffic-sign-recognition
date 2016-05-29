#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

void drawStuff();
void drawAllTriangles(Mat&, const vector< vector<Point> >&);

int thresh_min = 0, thresh_max = 30;
Mat img,edges,gblur,canny_output,drawing;

int main(){
	VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;
	do{
		cap >> img;
		cvtColor(img, edges, COLOR_BGR2GRAY);
		GaussianBlur(edges, gblur, Size(7, 7), 1.5, 1.5);
		drawStuff();
		//imshow("Video", gblur);
	} while (waitKey(100) != 'x');
}

void drawStuff(){
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Canny(gblur, canny_output, thresh_min, thresh_max, 3);
	//imshow("Video",img);
	imshow("Canny", canny_output);
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	
	drawAllTriangles(drawing, contours);
	imshow("Triangles", drawing);
}

void drawAllTriangles(Mat& img, const vector< vector<Point> >& contours){
	vector<Point> approxTriangle;
	for (size_t i = 0; i < contours.size(); i++){
		approxPolyDP(contours[i], approxTriangle, arcLength(Mat(contours[i]), true)*0.05, true);
		if (approxTriangle.size() == 3){
			drawContours(img, contours, i, Scalar(0, 255, 255), CV_FILLED); // fill GREEN
			vector<Point>::iterator vertex;
			for (vertex = approxTriangle.begin(); vertex != approxTriangle.end(); ++vertex){
				circle(img, *vertex, 3, Scalar(0, 0, 255), 1);
			}
		}
	}
}