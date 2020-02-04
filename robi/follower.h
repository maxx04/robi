#pragma once
//#pragma warning(disable : 4996)

#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdio>
#include <queue>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/tracking.hpp>

#include "UDP_Base.h"
#include "camera_calibration.h"
#include "neck.h"
#include "driver.h"


using namespace cv;
using namespace std;

class follower
{
	int wait_time = 100;

	TickMeter tm;

	TermCriteria termcrit;
	Size subPixWinSize, winSize;

  	neck fneck;
	driver fdriver;

	Mat image;
	Mat prev_image;

	vector<uchar> status; // status vom calcOpticalFlowPyrLK
	vector<float> err; // error vom calcOpticalFlowPyrLK
	vector<Point2f> kpt;
	vector<Point2f> prev_kpt;
	vector<Point2f> kpt_diff;




public:

	bool needToInit = false;

	Mat cameraMatrix;
	Mat distCoeffs;

	follower(VideoCapture cap);
	follower(cv::Size res);
	~follower();

	void take_picture(Mat* frame);
	bool key(int wait);

	// Bearbeitet jedes frame
	bool proceed_frame(Mat* frame, int frame_index);
	void find_keypoints();
	void find_diff_keypoints();
	void calcOptFlow(int frame_index);
	void copy_keypoints();
	void new_data_proceed(UDP_Base* udp_base);
	void start_move(UDP_Base * udp_base);
	void start_move(float power);  // Bewegung gerade, power - Geschwindigkeit [0.0...1.0]
	void stop_move(UDP_Base * udp_base);
	void stop_move();
	void clean_bad_keypoints(int frame_index);
};


