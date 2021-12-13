#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <future>
#include <queue>
#include <mutex>

#include "TCPClient.h"
#include "ThreadPool.h"
#include "RequestGenerator.h"
#include "ResponseVerifier.h"
#include "Storage.h"

#define DEFAULT_PORT 12345

double readRequest(TCPClient client, int threadIndex, double timeDurationSecs);
double postRequest(TCPClient client, int threadIndex, double timeDurationSecs, mutex* lock);
string sendViaClient(TCPClient client, string request);

unsigned int readRequests = 0;
unsigned int postRequests = 0;

Storage* db = new Storage();
ResponseVerifier* responseVerifier = new ResponseVerifier();
RequestGenerator* requestGenerator = new RequestGenerator();

int main(int argc, char **argv)
{
	// Default parameters
	unsigned int posterCount = 5;
	unsigned int readerCount = 0;
	double timeDurationSecs = 10;
	bool throttle = false;
	string serverIp = "127.0.0.1";

	// Validate the parameters
	if (argc != 6) {
		std::cout << "\nUsage (required parameters): server_IP number_of_poster_threads number_of_reader_threads time_duration throttle(0|1)\n\n";
		std::cout << "server_IP - IP of the server\n";
		std::cout << "number_of_poster_threads - number of threads performing POST operations\n";
		std::cout << "number_of_reader_threads - number of threads performing READ operations\n";
		std::cout << "time_duration - duration of test execution in seconds\n";
		std::cout << "throttle(0|1) - 0: do not throttle message speed\n";
		std::cout << "\t\t1: throttle message speed\n\n";

		std::cout << "\nDefault Parameters:\n";
		std::cout << "\tserver_IP - " << serverIp << "\n";
		std::cout << "\tnumber_of_poster_threads - " << posterCount << "\n";
		std::cout << "\tnumber_of_reader_threads - " << readerCount << "\n";
		std::cout << "\ttime_duration - " << timeDurationSecs << "s\n";
		std::cout << "\tthrottle - " << (throttle ? "true" : "false") << "\n\n";

		std::cout << "Enter dev mode using default paramaters?\n";
		system("pause");
		
	}
	else
	{
		serverIp = argv[1];
		posterCount = (int)argv[2];
		readerCount = (int)argv[3];
		timeDurationSecs = (int)argv[4];
		throttle = (int)argv[5];
	}



	//std::queue<future<bool>> posterVerificationQueue;
	//std::queue<bool> readerVerificationQueue;

	ThreadPool posterPool(posterCount);
	//ThreadPool readerPool(readerCount);

	//vector<thread> posterThreads;
	//vector<thread> readerThreads;
	vector<future<double>> posterDurationFutures;
	mutex lock;
	TCPClient client(serverIp, DEFAULT_PORT);
	client.OpenConnection();

	chrono::high_resolution_clock::time_point endTime;
	chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();

	for (int i = 0; i < posterCount; i++)
	{
		//posterThreads.emplace_back(postRequest, client, i, timeDurationSecs);
		//std::future<double> durationFut = std::async(std::launch::async, postRequest, client, i, timeDurationSecs);
		//posterDurationFutures.push_back(async(launch::async, postRequest, client, i, timeDurationSecs));
		posterDurationFutures.push_back(posterPool.enqueue(postRequest, client, i, timeDurationSecs, &lock));
	}

	double posterTotal = 0;
	for (int i = 0; i < posterDurationFutures.size(); i++)
	{
		bool valid = posterDurationFutures[i].valid();
		//posterDurationFutures[i].wait();
		posterTotal += posterDurationFutures[i].get();
	}

	cout << posterTotal;
	
	for (int i = 0; i < readerCount; i++)
	{
		//readerThreads.emplace_back();
	}


	client.CloseConnection();

	delete db;
	delete responseVerifier;
	delete requestGenerator;

	system("pause");
	return 0;
}

inline string sendViaClient(TCPClient client, string request)
{
	return client.send(request);
}

double postRequest(TCPClient client, int threadIndex, double timeDurationSecs, mutex* lock)
{
	int threadPostCount = 0;

	chrono::high_resolution_clock::time_point endTime;
	chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();

	do
	{
		string request = requestGenerator->generateWriteRequest();
		string response = client.send(request);
		bool isValidResponse = db->addPosterValue(request, response);

		endTime = chrono::high_resolution_clock::now();
		threadPostCount++;
		lock->lock();
		postRequests++;
		lock->unlock();
	} while (chrono::duration_cast<chrono::duration<double>>(endTime - startTime).count() < timeDurationSecs);

	double totalRunTime = (endTime - startTime).count();
	double posterRequestsPerSecond = threadPostCount / totalRunTime;

	std::cout << "Thread: " << threadIndex << endl;
	std::cout << "Average post requests per second: " << posterRequestsPerSecond << endl;
	return totalRunTime;
}

double readRequest(TCPClient client, int threadIndex, double timeDurationSecs)
{
	return 0.0;
}
