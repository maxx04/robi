#pragma once

#include <iostream>
#include <thread> 
#include <assert.h>

#include "xsocket.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>

#define SOCKET_BLOCK_SIZE 4096
#define ENCODE_QUALITY 90
#define ANZAHL_AUFNAHMEN 5

using namespace std;

struct exchange_data 
{
	float angle_horizontal;
	float angle_vertikal;
	float move_stright;
	float stright_velocity;
	float direction;
};

struct keypoints_flow
{
	cv::Point2f p;
	cv::Point2f flow[ANZAHL_AUFNAHMEN];
};


union udata
{
	exchange_data dt_udp; 
	char union_buff[SOCKET_BLOCK_SIZE];	
}; 

union int_char
{
	int nb;
	char bf[sizeof(int)];
};


class UDP_Base
{
	std::thread* udp_thread;
	static bool new_udp_data;
	static udata dt;
	static net::endpoint ep;
	
public:

	static net::socket* ludp_socket;
	static bool transfer_busy;
	static bool imagegrab_ready;
	static bool keypoints_sendet;
	static std::vector <uchar> encoded;
	static int keypoints_number;
	static std::vector <keypoints_flow> key_points;
	exchange_data* udp_data;

	UDP_Base();
	~UDP_Base();
	void udp_data_received();
	static bool check_incoming_data();
	static void send_keypoints(int points_number);
	static void receive_keypoints();
	static void start_Server(int args);


};

