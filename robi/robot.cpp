#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>


#include "UDP_Base.h"
#include "follower.h"
#include "robot.h"


using namespace cv;
using namespace std;


static void help()
{
	// print a welcome message, and the OpenCV version
	cout << "\nRobot program start\n"
		"Using OpenCV version " << CV_VERSION << endl;

	cout << "\nHot keys: \n"
		"\tESC - quit the program\n"
		"\tq- quit the program\n"
		"\tk - calibrate camera\n"
		<< endl;
}

static UDP_Base udp_base;

int main(int argc, char** argv)
{
	help();

	TickMeter tkm;


	static Mat  frame[ANZAHL_AUFNAHMEN], gray;

	vector <int> compression_params;
	int jpegqual = ENCODE_QUALITY; // Compression Parameter
	compression_params.push_back(cv::ImwriteFlags::IMWRITE_JPEG_QUALITY);
	compression_params.push_back(jpegqual);	 //TODO

	tm tim;

	unsigned long t = mktime(&tim);

	char outpipe[512];

	//sprintf(outpipe, "appsrc !videorate !autovideoconvert ! video/x-raw, format=I420 !omxh264enc \
	//!mpegtsmux ! filesink location = video_%lu.avi", t);

	sprintf(outpipe, "appsrc !autovideoconvert ! video/x-raw, format=I420 !omxh264enc bitrate = 10000000\
	! qtmux ! filesink location = video_%lu.mp4", t);
	//omxh264enc bitrate = 10000000 !qtmux !filesink location = video_ % u.mp4

	//sprintf(outpipe, "appsrc ! autovideoconvert !omxh265enc ! matroskamux ! filesink location = video_%lu.mkv", t);

	//sprintf(outpipe, "appsrc ! videoconvert ! omxh264enc ! mpegtsmux ! filesink location = video_%lu.ts", t);
	//sprintf(outpipe, "appsrc ! videoconvert ! nvjpegenc ! filesink location=video_%lu.mjpg ", t);

	char gst[] = "nvarguscamerasrc  ! video/x-raw(memory:NVMM), format=(string)NV12, \
		width=(int)1280, height=(int)720, framerate=(fraction)60/1 ! \
		nvvidconv ! video/x-raw, format=(string)BGRx ! 	\
		videoconvert ! video/x-raw, format=(string)BGR  ! appsink";


	cv::VideoCapture cap(gst);

	if (!cap.isOpened())
	{
		cout << "Could not initialize capturing...\n";
		return 0;
	}

	cout << "capturing initialised: " << endl;

	cout << cap.get(cv::CAP_PROP_FRAME_WIDTH) << ":" <<
		cap.get(cv::CAP_PROP_FRAME_HEIGHT) << ":" <<
		cap.get(cv::CAP_PROP_FPS) << endl;

	//cap >> gray;

	follower robot(cap);

	//--- INITIALIZE VIDEOWRITER
	VideoWriter writer;
	sleep(5);
	
	writer.open(outpipe, 0, 60, cv::Size(1280, 720));

	if (!writer.isOpened()) {
		std::cout << "Failed to open gst-writer." << std::endl;
		return (-4);
	}
	
	//char f[30];
	robot.start_move(0.10f);


	for (int i = 0; i < 100; i++)
	{
		if (cap.read(gray)) 
		{
			// encode the frame into the videofile stream
			//sprintf(f, "./frames/n_%06d.png", i);

			//cv::imwrite(f, gray);

			writer.write(gray);
		}
		else
		cout << i << endl;

	}

	robot.stop_move();

	cout << "Video done" << endl;

	cout << "waiting transfer" << endl;

	writer.release();

	for (;;)
	{
		delay(200); //fuer andere Prozesse und Auslaustung notwendig


		while (udp_base.transfer_busy)
		{
			delay(200);
		}

		if (udp_base.check_incoming_data())
		{
			cout << "new udp data \n";
			// start der bewegung
			tkm.reset();
			tkm.start();

			robot.new_data_proceed(&udp_base);
			robot.needToInit = true;

			// serie von bildern aufnehmen
			for (int n = 0; n < ANZAHL_AUFNAHMEN; n++)
			{
				robot.stop_move(&udp_base);
				//  buffer entlehren
				delay(800);
				for (int i = 0; i < 5; i++) cap >> frame[n];

				cap >> frame[n];
				tkm.stop();

				cout << "aufnahmen abstand " << tkm << " s" << endl;
				tkm.reset();
				tkm.start();
				delay(10);

				robot.start_move(&udp_base);
			}

			robot.stop_move(&udp_base);


			//bearbeiten
			for (int n = 0; n < ANZAHL_AUFNAHMEN; n++)
			{
				// wenn alles in ordnung weiter
				if (robot.proceed_frame(&frame[n], n))
				{
					return 11;
				}
				cout << "Frame " << n << endl; ;
			}




			cv::imencode(".jpg", frame[ANZAHL_AUFNAHMEN - 1], udp_base.encoded, compression_params);

			robot.copy_keypoints();

			udp_base.imagegrab_ready = true; // fuer thread mit Server

		}


	}

	cap.release();
	return 0;
}
