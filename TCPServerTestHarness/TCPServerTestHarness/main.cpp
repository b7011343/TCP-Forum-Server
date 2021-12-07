#include <iostream>
#include <ctime>
#include <chrono>

#include "TCPClient.h"
#include "ThreadPool.h"
#include "RequestGenerator.h"
#include "ResponseVerifier.h"
#include "Storage.h"

#define DEFAULT_PORT 12345

int main(int argc, char **argv)
{
	// Validate the parameters
	if (argc != 6) {
		//printf("\nUsage: %s server_name\nserver_IP number_of_poster_threads number_of_reader_threads time_duration throttle(0|1)\n\n", argv[0]);
		std::cout << "\nUsage: server_IP number_of_poster_threads number_of_reader_threads time_duration throttle(0|1)\n\n";
		std::cout << "server_IP - IP of the server\n";
		std::cout << "number_of_poster_threads - number of threads performing POST operations\n";
		std::cout << "number_of_reader_threads - number of threads performing READ operations\n";
		std::cout << "time_duration - duration of test execution in seconds\n";
		std::cout << "throttle(0|1) - 0: do not throttle message speed\n";
		std::cout << "\t\t1: throttle message speed\n\n";
		return 1;
	}

	unsigned int posterCount = (int)argv[2];
	unsigned int readerCount = (int)argv[3];
	unsigned int timeDurationSecs = (int)argv[4];
	bool throttle = (int)argv[5];

	ThreadPool posterPool(posterCount);
	ThreadPool readerPool(readerCount);

	Storage db();
	RequestGenerator requestGenerator();
	ResponseVerifier responseVerifier();

	TCPClient client(argv[1], DEFAULT_PORT);
	client.OpenConnection();
	std::string request;

	do {

	} while ();

	/*
	do {
		request = "";
		std::cout << "Enter string to send to server or \"exit\" (without quotes to terminate): ";
		std::getline(std::cin, request);

		std::cout << "String sent: " << request << std::endl;
		std::cout << "Bytes sent: " << request.size() << std::endl;

		std::string reply = client.send(request);

		std::cout << "String returned: " << reply << std::endl;
		std::cout << "Bytes received: " << reply.size() << std::endl;
	} while (request != "exit");
	*/

	client.CloseConnection();

	return 0;
}
