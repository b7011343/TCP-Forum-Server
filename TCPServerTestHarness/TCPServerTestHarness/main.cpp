#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <future>
#include <queue>
#include <mutex>
#include <tuple>
#include <map>

#include "TCPClient.h"
#include "ThreadPool.h"
#include "RequestGenerator.h"
#include "ResponseVerifier.h"
#include "Storage.h"

#define DEFAULT_PORT 12345

double readRequest(string serverIp, int threadIndex, double timeDurationSecs);
void postRequest(string serverIp, int threadIndex, double timeDurationSecs);

unsigned int readRequests = 0;
unsigned int postRequests = 0;

mutex mLock;
map<int, tuple<double, int>> posterThreadMap;
map<int, vector<future<tuple<bool, int, int>>>> posterThreadErrorResponses;
map<int, tuple<double, int>> readerThreadMap;
Storage* db = new Storage();
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

	cout << "\nStarting throughput test...\n";

	ThreadPool posterPool(posterCount);
	vector<future<void>> posterFutures;
	double posterTotalTime = 0.0;

	ThreadPool readerPool(readerCount);
	vector<future<void>> readerFutures;
	double readerTotalTime = 0.0;

	for (int i = 0; i < posterCount; i++)
		posterFutures.push_back(posterPool.enqueue(postRequest, serverIp, i, timeDurationSecs));

	for (int i = 0; i < posterFutures.size(); i++)
		posterFutures[i].wait();

	for (int i = 0; i < posterThreadMap.size(); i++)
	{
		double posterRequestsPerSecond = get<1>(posterThreadMap[i]);
		double threadRunTime = get<0>(posterThreadMap[i]);
		posterTotalTime += threadRunTime;
		std::cout << "\nPoster thread " << i << " (ran for " << threadRunTime << "s) - Average post requests per second: " << posterRequestsPerSecond << "\n";
	}

	cout << "\nTotal runtime: " << posterTotalTime << "s" << "\n";
	cout << "\nTotal post requests: " << postRequests << "\n";
	cout << "\nAverage post requests per second per thread: " << postRequests / posterTotalTime << "\n\n";

	for (int i = 0; i < readerCount; i++)
	{
	}

	delete db;
	delete requestGenerator;

	system("pause");
	return 0;
}

tuple<bool, int, int> validateAndStore(string request, string response)
{
	return db->addPosterValue(request, response);
}

void postRequest(string serverIp, int threadIndex, double timeDurationSecs)
{
	TCPClient client(serverIp, DEFAULT_PORT);
	client.OpenConnection();

	int threadPostCount = 0;
	double timeSpan;
	chrono::high_resolution_clock::time_point endTime;
	chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();
	
	vector<future<tuple<bool, int, int>>> requestValidations;

	do
	{
		string request = requestGenerator->generateWriteRequest();
		string response = client.send(request);

		requestValidations.push_back(async(launch::async | launch::deferred, validateAndStore, request, response));

		endTime = chrono::high_resolution_clock::now();
		timeSpan = chrono::duration_cast<chrono::duration<double>>(endTime - startTime).count();
		threadPostCount++;
	} while (timeSpan < timeDurationSecs);

	cout << "Thread " << threadIndex << " validation count = " << requestValidations.size() << "\n\n";

	
	double totalRunTime = (endTime - startTime).count();
	double posterRequestsPerSecond = threadPostCount / timeSpan;

	tuple<double, double> returnValues = make_tuple(timeSpan, posterRequestsPerSecond);
	mLock.lock();
	postRequests += threadPostCount;
	posterThreadMap[threadIndex] = returnValues;
	//copy(requestValidations.begin(), requestValidations.end(), back_inserter(posterThreadErrorResponses[threadIndex]));
	mLock.unlock();

	for (int i = 0; i < requestValidations.size(); i++)
	{
		tuple<bool, int, int> result = requestValidations[i].get();
		bool valid = (bool)get<0>(result);
		int correct = get<1>(result);
		int actual = get<2>(result);
		if (!valid)
		{
			cout << correct << "\t" << actual << "\n";
		}
	}

	client.CloseConnection();
}

double readRequest(TCPClient client, int threadIndex, double timeDurationSecs)
{
	return 0.0;
}
