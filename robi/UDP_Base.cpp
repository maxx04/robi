#include "UDP_Base.h"

using namespace std;
using namespace cv;

bool UDP_Base::new_udp_data = false;
bool UDP_Base::transfer_busy = false; 
bool UDP_Base::imagegrab_ready = false; 

udata UDP_Base::dt;

net::socket* UDP_Base::ludp_socket;

std::vector < uchar > UDP_Base::encoded(100);
int UDP_Base::keypoints_number = 0;
std::vector <keypoints_flow> UDP_Base::key_points(500); //TODO 500

net::endpoint UDP_Base::ep;


#define WAIT_ON_CAM 20


UDP_Base::UDP_Base()
{

//	cout << sizeof(exchange_data) << endl;

	assert(sizeof(exchange_data) < SOCKET_BLOCK_SIZE);

	udp_data = &dt.dt_udp;

	udp_thread = new thread(start_Server, 1);

	std::cout << "Server thread started, Id: " << udp_thread->get_id() << endl;

}


UDP_Base::~UDP_Base()
{
	std::cout << "Server thread closed, Id: " << udp_thread->get_id() << endl;
}

void UDP_Base::udp_data_received()
{
	new_udp_data = false;
}

bool UDP_Base::check_incoming_data()
{
	return new_udp_data;
}

void UDP_Base::send_keypoints(int points_number)
{

	/*transfer_busy = true;*/

	int n_blocks =  1 + ((keypoints_number * sizeof(keypoints_flow) - 1) / SOCKET_BLOCK_SIZE);

	cout << sizeof(key_points) << " - " << sizeof(keypoints_flow) << endl;

	int_char tmp;

	// sende anzahl punkten
	tmp.nb = keypoints_number;

	// Groesse senden
	ludp_socket->sendto(tmp.bf, sizeof(int), ep);

	cout << "punktenanzahl gesendet " << keypoints_number << " - " << n_blocks << " bloks" << endl;

	//Data senden

	cout << "start transfer keypoints \t " << sizeof(key_points) << " : "/*<< hex << int((char*)(&key_points[0])) << dec*/ << endl;

	//tm.reset();
	//tm.start();

	int n = 0;

	size_t block_length = SOCKET_BLOCK_SIZE;

	while (n < n_blocks)
	{

		//cout << ludp_socket-> sendto( (reinterpret_cast<char *>(key_points)) + n * SOCKET_BLOCK_SIZE, SOCKET_BLOCK_SIZE, ep) << endl;
		ludp_socket->sendto((char *)(&key_points[0]) + n * block_length, block_length, ep);
		//cout << n << endl;


		//OPTI gibt es moeglichkeit zum erneutes senden
		// Baestaetigung block war erfolgreich uebertragen
		int i = ludp_socket->recvfrom(tmp.bf, sizeof(int), &ep);

		//cout << "receive " << n << endl;

		if (i == -1)
		{
			cerr << "falsche antwort vom client \n";
			break;
		}

		n++;
	}

	/*transfer_busy = false;*/
}

//void UDP_Base::set_frame_pointer(Mat* frame)
//{
//	ptrFrame = frame;
//}

//Mat * UDP_Base::get_frame_pointer()
//{
//	return ptrFrame;
//}



void UDP_Base::receive_keypoints()
{
}

void UDP_Base::start_Server(int args)
{

	TickMeter tm;

	//init only required for windows, no-op on *nix
	net::init();

	//create an ipv4 udp socket, we can optionaly specify the port to bind to as the 3rd arg
	ludp_socket = new net::socket(net::af::inet, net::sock::dgram, 4010);


	if (!ludp_socket->good()) {
		std::cerr << "failed to create & bind ipv4 socket" << std::endl;
		return;
	}

	std::cout << "ipv4 socket created..." << std::endl;

	//we can access a sockets local endpoint by getlocaladdr() && getremoteaddr() for tcp peers
	std::cout << "listening at: " << ludp_socket->getlocaladdr().to_string() << std::endl
		<< "send a udp packet to :: " << ludp_socket->getlocaladdr().get_port() << " to continue" << std::endl;


	//recv a packet up to 512 bytes and store the sender in endpoint ep
	//ludp_socket.recvfrom(dt.union_buff, SOCKET_BLOCK_SIZE, &ep); //erste client aufgenommen 
	//std::cout << "erstes pack, buffer: " << dt.union_buff << std::endl;
	//std::cout << ep.to_string() << std::endl;

	while (true)
	{
		//wartet auf inkommende hauptdaten
		cout << "wartet auf client \n";
		int i = ludp_socket->recvfrom(dt.union_buff, SOCKET_BLOCK_SIZE, &ep);
		if (i == -1)
		{
			cerr << "Fehler beim warten \n";	break;
		}
		cout << ep.to_string() << endl;


		//imagegrab_ready = false;
		new_udp_data = true;

		//warten auf bild
		tm.reset();
		tm.start();

		cout.precision(3);

		while (!imagegrab_ready)
		{
			//cout << "waiting on image " << tm.getTimeSec() << " s \n";
			if (tm.getTimeSec() > WAIT_ON_CAM)
			{
				cerr << " bild nicht aufgenommen \n"; break;
			}
			usleep(200000);
			tm.stop();
			tm.start();
		}

		transfer_busy = true;

		int n_blocks = 1 + ((encoded.size() - 1) / SOCKET_BLOCK_SIZE);

		int_char tmp;

		tmp.nb = n_blocks;

		ludp_socket->sendto(tmp.bf, sizeof(int), ep);

		cout << "antwort gesendet " << n_blocks << " blocks \n";

		//Bild senden
		int n = 0;

		cout << "start transfer picture \t" << /*hex << int((char*)(&encoded[0])) << dec <<*/ endl;

		tm.reset();
		tm.start();

		while (n < n_blocks)
		{
			//cout << n << "\r";

			ludp_socket->sendto((char*)&encoded[ n * SOCKET_BLOCK_SIZE], SOCKET_BLOCK_SIZE, ep);
			n++;

			//OPTI gibt es moeglichkeit zum erneutes senden
			// Baestaetigung block war erfolgreich uebertragen
			int i = ludp_socket->recvfrom(tmp.bf, sizeof(int), &ep);

			if (i == -1)
			{
				cerr << "falsche antwort vom client \n";
				break;
			}
		}

		// points Uebertragen
		/*
		tmp.nb = n_blocks;

		ludp_socket.sendto(tmp.bf, sizeof(int), ep);

		cout << "antwort gesendet Daten" << n_blocks << " blocks \n";
		*/
		// 

		tm.stop();

		cout << "end transfer picture" << n << " blocks in " << tm << endl;

		send_keypoints(0);

		transfer_busy = false;
		imagegrab_ready = false;

		//OPTI wenn gibt es neues antwort dann senden

	}

	ludp_socket->close();
	cout << "socket closed" << endl;
}



