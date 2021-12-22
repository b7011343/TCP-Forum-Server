#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <future>
#include <queue>
#include <shared_mutex>
#include <condition_variable>
#include <tuple>
#include <map>

#include "TCPClient.h"
#include "ThreadPool.h"
#include "RequestGenerator.h"
#include "ResponseVerifier.h"
#include "Storage.h"

#define DEFAULT_PORT 12345

struct compare
{
	bool operator()(tuple<long long int, string, string> a, tuple<long long int, string, string> b)
	{
		return get<0>(a) > get<0>(b);
	}
};

void readRequest(string serverIp, int threadIndex, double timeDurationSecs);
void postRequest(string serverIp, int threadIndex, double timeDurationSecs);
void verifyPosts();
void verifyReads();

int readRequests = 0;
int postRequests = 0;

map<int, tuple<double, int>> posterThreadMap;
priority_queue<tuple<long long int, string, string>, vector<tuple<long long int, string, string>>, compare> posterVerificationQueue;

map<int, tuple<double, int>> readerThreadMap;
queue<tuple<long int, string, string>> readerVerificationQueue;

shared_mutex mLock;
condition_variable cv;
Storage* db = new Storage();
RequestGenerator* requestGenerator = new RequestGenerator();
ResponseVerifier* verifier = new ResponseVerifier(db);

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
	double posterTotalTime = 0.0; // The total time in seconds that all poster threads took to run

	ThreadPool readerPool(readerCount);
	vector<future<void>> readerFutures;
	double readerTotalTime = 0.0; // The total time in seconds that all reader threads took to run

	for (int i = 0; i < posterCount; i++)
		posterFutures.push_back(posterPool.enqueue(postRequest, serverIp, i, timeDurationSecs));

	for (int i = 0; i < readerCount; i++)
		readerFutures.push_back(readerPool.enqueue(readRequest, serverIp, i, timeDurationSecs));

	for (int i = 0; i < posterFutures.size(); i++)
		posterFutures[i].wait();

	for (int i = 0; i < readerFutures.size(); i++)
		readerFutures[i].wait();

	for (int i = 0; i < posterThreadMap.size(); i++)
	{
		double threadRunTime = get<0>(posterThreadMap[i]);
		double posterRequestsPerSecond = get<1>(posterThreadMap[i]);
		posterTotalTime += threadRunTime;
		std::cout << "\nPoster thread " << i << " (ran for " << threadRunTime << "s) - Average post requests per second: " << posterRequestsPerSecond << "\n";
	}

	cout << "\nTotal poster runtime: " << posterTotalTime << "s" << "\n";
	cout << "Total post requests: " << postRequests << "\n";
	cout << "Average post requests per second per thread: " << postRequests / posterTotalTime << "\n";
	
	cout << "\nDo you want to verify all the responses? (Warning: This can take a while)\n";
	system("pause");
	int invalidResponses = verifier->validatePostResponses();
	//verifyPosts();
	//verifyReads();

	// TODO: Implement the block above for reader threads

	delete db;
	delete requestGenerator;

	system("pause");
	return 0;
}

void postRequest(string serverIp, int threadIndex, double timeDurationSecs)
{
	TCPClient client(serverIp, DEFAULT_PORT);
	client.OpenConnection();

	int threadPostCount = 0;
	double timeSpan;
	chrono::high_resolution_clock::time_point endTime;
	chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();
	
	do 
	{
		// TODO: Add throttling
		string request = requestGenerator->generateWriteRequest();
		string response = client.send(request);
		db->addPosterValue2(0, request, response);
		threadPostCount++;

		endTime = chrono::high_resolution_clock::now();
		timeSpan = chrono::duration_cast<chrono::duration<double>>(endTime - startTime).count();
	} while (timeSpan < timeDurationSecs);

	double totalRunTime = (endTime - startTime).count();
	double posterRequestsPerSecond = threadPostCount / timeSpan;
	tuple<double, double> returnValues = make_tuple(timeSpan, posterRequestsPerSecond);

	mLock.lock();
	postRequests += threadPostCount;
	posterThreadMap[threadIndex] = returnValues;
	mLock.unlock();

	client.CloseConnection();
}

void readRequest(string serverIp, int threadIndex, double timeDurationSecs)
{
	TCPClient client(serverIp, DEFAULT_PORT);
	client.OpenConnection();

	int threadReadCount = 0;
	double timeSpan;
	chrono::high_resolution_clock::time_point endTime;
	chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();

	do
	{
		string request = requestGenerator->generateReadRequest();
		mLock.lock();
		string response = client.send(request);
		readerVerificationQueue.push(make_tuple(0, request, response)); // TODO: create a local copy of this queue  and then push into main one afterwards
		mLock.unlock();
		threadReadCount++;

		endTime = chrono::high_resolution_clock::now();
		timeSpan = chrono::duration_cast<chrono::duration<double>>(endTime - startTime).count();
	} while (timeSpan < timeDurationSecs);

	double totalRunTime = (endTime - startTime).count();
	double posterRequestsPerSecond = threadReadCount / timeSpan;
	tuple<double, double> returnValues = make_tuple(timeSpan, posterRequestsPerSecond);

	mLock.lock();
	readRequests += threadReadCount;
	readerThreadMap[threadIndex] = returnValues;
	mLock.unlock();

	client.CloseConnection();
}

void verifyPosts()
{
	vector<tuple<string, int, int>> incorrectPostResponses;


	while (!posterVerificationQueue.empty())
	{
		tuple<int, string, string> post = posterVerificationQueue.top();
		posterVerificationQueue.pop();

		int postIndex = get<0>(post);
		string request = get<1>(post);
		string response = get<2>(post);

		tuple<bool, int, int> postVerification = db->addPosterValue(postIndex, request, response);
		bool isValid = get<0>(postVerification);

		if (!isValid)
		{
			int correctResponse = get<1>(postVerification);
			int actualResponse = get<2>(postVerification);
			cout << "Invalid response found! Should be " << correctResponse << " but actual response was " << actualResponse << "\n";
			incorrectPostResponses.push_back(make_tuple(request, correctResponse, actualResponse));
		}
	}

	cout << "\nIncorrect post responses: " << incorrectPostResponses.size() << "\n";

	for (int i = 0; i < incorrectPostResponses.size(); i++)
	{
		tuple<string, int, int> incorrectResponse = incorrectPostResponses[i];
		string request = get<0>(incorrectResponse);
		int correctResponse = get<1>(incorrectResponse);
		int actualResponse = get<2>(incorrectResponse);
		cout << "Incorrect response #" << i + 1 << "\n";
		cout << "Request: " << request << "\n";
		cout << "Expected response: " << correctResponse << "\n";
		cout << "Actual response: " << actualResponse << "\n\n";
	}
}

void verifyReads()
{

}
