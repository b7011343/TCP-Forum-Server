#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <future>

#include "TCPClient.h"
#include "ThreadPool.h"
#include "RequestGenerator.h"
#include "ResponseVerifier.h"
#include "Storage.h"

#define DEFAULT_PORT 12345

void readRequest();
void postRequest();


unsigned int readRequests = 0;
unsigned int postRequests = 0;

int main(int argc, char **argv)
{
	// Validate the parameters
	if (argc != 6) {
		//printf("\nUsage: %s server_name\nserver_IP number_of_poster_threads number_of_reader_threads time_duration throttle(0|1)\n\n", argv[0]);
		std::cout << "\nUsage (required parameters): server_IP number_of_poster_threads number_of_reader_threads time_duration throttle(0|1)\n\n";
		std::cout << "server_IP - IP of the server\n";
		std::cout << "number_of_poster_threads - number of threads performing POST operations\n";
		std::cout << "number_of_reader_threads - number of threads performing READ operations\n";
		std::cout << "time_duration - duration of test execution in seconds\n";
		std::cout << "throttle(0|1) - 0: do not throttle message speed\n";
		std::cout << "\t\t1: throttle message speed\n\n";
		return 1;
	}

	Storage* db = new Storage();
	ResponseVerifier* responseVerifier = new ResponseVerifier();
	RequestGenerator* requestGenerator = new RequestGenerator();

	unsigned int posterCount = (int)argv[2];
	unsigned int readerCount = (int)argv[3];
	double timeDurationSecs = (int)argv[4];
	bool throttle = (int)argv[5];

	ThreadPool posterPool(posterCount);
	ThreadPool readerPool(readerCount);

	TCPClient client(argv[1], DEFAULT_PORT);
	client.OpenConnection();
	//std::string request;

	chrono::high_resolution_clock::time_point endTime;
	chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();

	do {
		// Write requests
		if (!throttle || (throttle && postRequests < 1000))
		{
			std::string postReq = requestGenerator->generateWriteRequest();
			posterPool.enqueue(postRequest);
		}

		// Read requests
		if (!throttle || (throttle && readRequests < 1000))
		{
			readerPool.enqueue(readRequest);
		}

		endTime = chrono::high_resolution_clock::now();
	} while (chrono::duration_cast<chrono::duration<double>>(endTime - startTime).count() < timeDurationSecs);

	readerPool.~ThreadPool();
	posterPool.~ThreadPool();

	/*
	do {
		request = "";
		std::cout << "Enter string to send to server or \"exit\" (without quotes to terminate): ";
		std::getline(std::cin, request);

		std::cout << "String sent: " << request << std::endl;
		std::cout << "Bytes sent: " << request.size() << std::endl;

		std::string reply = client.send(request);
		std::future<string> ret = std::async(client.send, request);

		std::cout << "String returned: " << reply << std::endl;
		std::cout << "Bytes received: " << reply.size() << std::endl;
	} while (request != "exit");
	*/

	client.CloseConnection();

	delete db;
	delete responseVerifier;
	delete requestGenerator;

	return 0;
}

void postRequest()
{
	//std::string request = requestGenerator->generateWriteRequest();
}

void readRequest()
{

}
