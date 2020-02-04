#include "follower.h"

//Point2f AimPoint;
//bool setAimPt = false;

follower::follower(cv::Size frame_size)
{
	termcrit = TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 10, 0.03);
	subPixWinSize = Size(6, 6);
	winSize = Size(11, 11);

}

follower::follower(VideoCapture cap) : follower(cv::Size(1240,720))	//HACK
{
//	fneck.test();


	FileStorage ks("out_camera_data.xml", FileStorage::READ); // Read the settings

	if (!ks.isOpened()) 
	{
		cout << "calibrate camera!" << endl;

		cam_calibrate(&cap, &cameraMatrix, &distCoeffs);

		//cout << " Camera matrix fixed" << endl;
		//float data[10] = { 700, 0, 320, 0, 700, 240, 0, 0, 1 };
		//cameraMatrix = Mat(3, 3, CV_32FC1, data); // rows, cols

		//float data1[5] = { -4.1802327018241026e-001, 5.0715243805833121e-001, 0., 0.,
		//			-5.7843596847939704e-001 };

		//distCoeffs = Mat(5, 1, CV_32FC1, data1); // rows, cols
	}
	else
	{
		cout << "Camera matrix loaded" << endl;
		ks["Camera_Matrix"] >> cameraMatrix;
		ks["Distortion_Coefficients"] >> distCoeffs; //TODO
	}

	

	tm.reset();
}



follower::~follower()
{
}

// Bearbeitet Frame in schritten
bool follower::proceed_frame(Mat* frame, int frame_index)
{

	take_picture(frame);

	if (needToInit)
	{
		// nur erstes frame wird ausgeführt
		cout << "find keypoints: " << endl;

		kpt.clear();
		kpt_diff.clear();

		// Uebertragungspoints bereinigen
		UDP_Base::key_points.clear();

		find_keypoints();

		needToInit = false;

		// nicht weiter bearbeiten
		return false;
	}

	if (prev_image.size().width != 0)
	{
		cout << "optical Flow compute... ";

		tm.reset();
		tm.start();

		calcOptFlow(frame_index);

		tm.stop();

		cout << tm << " Keypoints: " << kpt.size() << endl;
	}


	if (key(wait_time)) return true;

	return false;

}


void follower::take_picture(Mat* frame)
{
	if (frame->empty())
	{
		cerr << " Frame empty \n";
		return; //TODO Fehlerabarbeitung
	}

	image.copyTo(prev_image);

	cvtColor(*frame, image, COLOR_BGR2GRAY);

}

void follower::find_keypoints()
{
	
	//finde features
	tm.reset();
	tm.start();

	kpt.clear();

	//goodFeaturesToTrack(image, kpt, 500, 0.03, 10, Mat(), 9, 5);


	vector<KeyPoint> keypoints_1;
	//vector<Point2f> key_points;

	int fast_threshold = 10;
	bool nonmaxSuppression = true;

	AGAST(image, keypoints_1, fast_threshold, nonmaxSuppression, AgastFeatureDetector::OAST_9_16);
	//FAST(gray, keypoints_1, fast_threshold, nonmaxSuppression);

	KeyPoint::convert(keypoints_1, kpt, vector<int>());

	//refine position
	//try {
	//	cornerSubPix(image, kpt, subPixWinSize, Size(-1, -1), termcrit);
	//}
	//catch (Exception e) {
	//	cout << "calc subpix fault " << tm << " kp " << kpt.size() << endl;
	//	exit(3);
	//}
	tm.stop();

	cout << "Features compute " << tm << " kp " << kpt.size() << endl;
	
}


bool follower::key(int wait)
{
	char c = (char)waitKey(wait);

	if (c == 27) return true;

	switch (c)
	{
	case 'r':
		break;

	case 'c':
		break;

		case 'k':
		//cam_calibrate(&cap, &cameraMatrix, &distCoeffs);
			break;
	}

	return false;
}



void follower::find_diff_keypoints()
{



}

void follower::calcOptFlow(int frame_index)
{
//	Point2f p;

	if (kpt.size() != 0)
	{
		// kpt muss nicht leer sein
		kpt.swap(prev_kpt);

		calcOpticalFlowPyrLK(prev_image, image, /*prev*/ prev_kpt, /*next*/ kpt,
			status, err, winSize, 5, termcrit, 0, 0.001);

		if (kpt_diff.size() == 0) kpt_diff.resize(kpt.size()); // OPTI immer pruefen?

		if (frame_index != 0) //wenn nicht erste frame
		{
			for (size_t i = 0; i < kpt.size(); i++)
			{
				Point2f tmp = kpt[i] - prev_kpt[i];
				//bei mehrerer aufnahmen summieren flow
				kpt_diff[i] += tmp;
				// kopiere auch getrennt
				UDP_Base::key_points[i].flow[frame_index] = tmp;
			}
		}
		else
			cerr << "OptFlow mit erstem Bild" << endl;

		clean_bad_keypoints(frame_index);
	}

}

void follower::clean_bad_keypoints(int frame_index)
{
	cout << "size kpt: " << kpt.size() << endl;
	for (size_t i = 0; i < kpt.size(); i++)
	{
		if (status[i] != 1 && err[i] < 10.0)
		{
			kpt.erase(kpt.begin() + i);
			kpt_diff.erase(kpt_diff.begin() + i); //OPTI erase zu langsam
			// bereinige auch zwischenbilder (vollständiges punkt)
			UDP_Base::key_points.erase(UDP_Base::key_points.begin() + i);
		}
	}

	cout << "new size kpt: " << kpt.size() << endl;
}

//Am Ende die übertragungspunkte kopieren
void follower::copy_keypoints()
{
	

	for (size_t i = 0; i < kpt.size(); i++) //TODO und weniger als 300
	{

		UDP_Base::key_points[i].p = kpt[i];
		UDP_Base::key_points[i].flow[0] = kpt_diff[i];
	}

	UDP_Base::keypoints_number = kpt.size();

}

void follower::new_data_proceed(UDP_Base* udp_base)
{

	fneck.move_to(udp_base->udp_data->angle_horizontal,
		udp_base->udp_data->angle_vertikal);

	// rukmeldung alles in ordnung
	udp_base->udp_data_received();

	//send antwort an client 
}

void follower::start_move(UDP_Base* udp_base) 
{
	fdriver.start_move( udp_base->udp_data->stright_velocity);
}

void follower::start_move(float power)
{
	fdriver.start_move(power);
}

void follower::stop_move(UDP_Base* udp_base)
{
	fdriver.stop_move(udp_base->udp_data->stright_velocity);
}

void follower::stop_move()
{
	fdriver.stop_move(0.0);
}




