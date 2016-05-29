/**
* Simple shape detector program.
* It loads an image and tries to find simple shapes (rectangle, triangle, circle, etc) in it.
* This program is a modified version of `squares.cpp` found in the Open sample dir.
*/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <iostream>

using namespace std;
using namespace cv;

/**
* Helper function to find a cosine of angle between vectors
* from pt0->pt1 and pt0->pt2
*/

static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

/**
* Helper function to display text in the center of a contour
*/
void setLabel(Mat& im, const string label, vector<Point>& contour)
{
	int fontface = FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	Size text = getTextSize(label, fontface, scale, thickness, &baseline);
	Rect r = boundingRect(contour);

	Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	rectangle(im, pt + Point(0, baseline), pt + Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	putText(im, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}

int main()
{
	//Mat src = imread("polygon.png");
	Mat src;
	Mat gray;
	Mat bw;
	Mat dst;
	vector<vector<Point> > contours;
	vector<Point> approx;

	VideoCapture capture(0);
	int q;

	while (cvWaitKey(30) != 'q')
	{
		capture >> src;
		if (true)
		{
			// Convert to grayscale
			cvtColor(src, gray, CV_BGR2GRAY);

			// Use Canny instead of threshold to catch squares with gradient shading
			blur(gray, bw, Size(3, 3));
			Canny(gray, bw, 80, 240, 3);
			imshow("bw", bw);
			//bitwise_not(bw, bw);

			// Find contours
			findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

			src.copyTo(dst);

			for (int i = 0; i < contours.size(); i++)
			{
				// Approximate contour with accuracy proportional
				// to the contour perimeter
				approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

				// Skip small or non-convex objects 
				if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
					continue;

				if (approx.size() == 3)
				{
					setLabel(dst, "TRI", contours[i]);    // Triangles
				}
				else if (approx.size() >= 4 && approx.size() <= 6)
				{
					// Number of vertices of polygonal curve
					int vtc = approx.size();

					// Get the cosines of all corners
					vector<double> cos;
					for (int j = 2; j < vtc + 1; j++)
						cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

					// Sort ascending the cosine values
					sort(cos.begin(), cos.end());

					// Get the lowest and the highest cosine
					double mincos = cos.front();
					double maxcos = cos.back();

					// Use the degrees obtained above and the number of vertices
					// to determine the shape of the contour
					if (vtc == 4)
						setLabel(dst, "RECT", contours[i]);
					else if (vtc == 5)
						setLabel(dst, "PENTA", contours[i]);
					else if (vtc == 6)
						setLabel(dst, "HEXA", contours[i]);
				}
				else
				{
					// Detect and label circles
					double area = contourArea(contours[i]);
					Rect r = boundingRect(contours[i]);
					int radius = r.width / 2;

					if (abs(1 - ((double)r.width / r.height)) <= 0.2 &&
						abs(1 - (area / (CV_PI * (radius*radius)))) <= 0.2)
						setLabel(dst, "CIR", contours[i]);
				}
			}
			imshow("src", src);
			imshow("dst", dst);

		}
		else
		{
			break;
		}
	}

	return 0;
}
