#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <future>
#include <mutex>
#include <tuple>
#include <map>
#include <fstream>

#include "TCPClient.h"
#include "ThreadPool.h"
#include "RequestGenerator.h"
#include "ResponseVerifier.h"
#include "Storage.h"
#include "Limiter.h"

#define DEFAULT_PORT 12345
#define THROTTLE_RATE 1000
#define THROTTLE_BURST 100
#define THROTTLE_N 1
#define LOG_FILE_NAME "test-harness-results.csv"

void readRequest(string serverIp, unsigned int threadIndex, double timeDurationSecs, bool throttle, unsigned int posterCount, bool validate);
void postRequest(string serverIp, unsigned int threadIndex, double timeDurationSecs, bool throttle, unsigned int posterCount, bool validate);

unsigned int readRequests = 0;
unsigned int postRequests = 0;

map<int, tuple<double, double, unsigned int>> posterThreadMap;
map<int, tuple<double, double, unsigned int>> readerThreadMap;

mutex mLock;
Storage* db = new Storage();
ResponseVerifier* verifier = new ResponseVerifier(db);

int main(int argc, char **argv)
{
	// Default parameters
	string serverIp = "127.0.0.1";
	unsigned int posterCount = 5;
	unsigned int readerCount = 5;
	double timeDurationSecs = 10;
	bool throttle = false;
	bool validate = false;

	// Validate the parameters
	if (argc != 6 && argc != 7) {
		cout << "\nUsage (required parameters): server_IP number_of_poster_threads number_of_reader_threads time_duration throttle(0|1) validate(0|1)\n\n";
		cout << "server_IP - IP of the server\n";
		cout << "number_of_poster_threads - number of threads performing POST operations\n";
		cout << "number_of_reader_threads - number of threads performing READ operations\n";
		cout << "time_duration - duration of test execution in seconds\n";
		cout << "throttle(0|1) - 0: do not throttle message speed\n";
		cout << "\t\t1: throttle message speed\n";
		cout << "validate(0|1) - 0: do not validate responses\n";
		cout << "\t\t1: do validate responses\n";

		cout << "\nDefault Parameters:\n";
		cout << "\tserver_IP - " << serverIp << "\n";
		cout << "\tnumber_of_poster_threads - " << posterCount << "\n";
		cout << "\tnumber_of_reader_threads - " << readerCount << "\n";
		cout << "\ttime_duration - " << timeDurationSecs << "s\n";
		cout << "\tthrottle - " << (throttle ? "true" : "false") << "\n";
		cout << "\tvalidate - " << (validate ? "true" : "false") << "\n\n";

		cout << "Enter dev mode using default paramaters?\n";
		system("pause");
	}
	else
	{
		serverIp = argv[1];
		posterCount = stoi(argv[2]);
		readerCount = stoi(argv[3]);
		timeDurationSecs = stoi(argv[4]);
		throttle = stoi(argv[5]);
		if (argc == 7)
			validate = stoi(argv[6]);
		cout << serverIp << " " << posterCount << " " << readerCount << " " << timeDurationSecs << " " << (bool)throttle << " " << (bool)validate;
	}

	cout << "\nStarting throughput test...\n";

	ThreadPool posterPool(posterCount);
	vector<future<void>> posterFutures;
	double posterTotalTime = 0.0;

	ThreadPool readerPool(readerCount);
	vector<future<void>> readerFutures;
	double readerTotalTime = 0.0;

	for (unsigned int i = 0; i < posterCount; i++)
		posterFutures.push_back(posterPool.enqueue(postRequest, serverIp, i, timeDurationSecs, throttle, posterCount, validate));

	this_thread::sleep_for(chrono::milliseconds(15)); // Wait for posters to begin posting

	for (unsigned int i = 0; i < readerCount; i++)
		readerFutures.push_back(readerPool.enqueue(readRequest, serverIp, i, timeDurationSecs, throttle, posterCount, validate));

	for (unsigned int i = 0; i < posterFutures.size(); i++)
		posterFutures[i].wait();

	for (unsigned int i = 0; i < readerFutures.size(); i++)
		readerFutures[i].wait();

	cout << "\nTest complete!\n\nResults:\n";

	for (unsigned int i = 0; i < posterThreadMap.size(); i++)
	{
		double threadRunTime = get<0>(posterThreadMap[i]);
		double posterRequestsPerSecond = get<1>(posterThreadMap[i]);
		unsigned int threadPostCount = get<2>(posterThreadMap[i]);
		posterTotalTime += threadRunTime;
		cout << "Poster thread " << i << " (ran for " << threadRunTime << "s) - Average post requests per second: " << posterRequestsPerSecond << " (Total = " << threadPostCount << ")\n";
	}

	cout << "\n\tTotal poster runtime: " << posterTotalTime << "s" << "\n";
	cout << "\tTotal post requests: " << postRequests << "\n";
	cout << "\tAverage post requests per second per thread: " << postRequests / posterTotalTime << "\n\n";

	for (unsigned int i = 0; i < readerThreadMap.size(); i++)
	{
		double threadRunTime = get<0>(readerThreadMap[i]);
		double readerRequestsPerSecond = get<1>(readerThreadMap[i]);
		unsigned int threadReadCount = get<2>(readerThreadMap[i]);
		readerTotalTime += threadRunTime;
		cout << "Reader thread " << i << " (ran for  " << threadRunTime << "s) - Average read requests per second: " << readerRequestsPerSecond << " (Total = " << threadReadCount << ")\n";
	}

	cout << "\n\tTotal reader runtime: " << readerTotalTime << "s" << "\n";
	cout << "\tTotal read requests: " << readRequests << "\n";
	cout << "\tAverage read requests per second per thread: " << readRequests / readerTotalTime << "\n";

	// Check if log file exists
	ifstream fileCheck(LOG_FILE_NAME);
	bool fileExists = fileCheck.good();
	ofstream resultsFile;
	resultsFile.open(LOG_FILE_NAME, ios_base::app);
	if (!fileExists)
		resultsFile << "Date/Time, Thread Type, Thread Count, Total Request Count, Total Duration (s), Average Requests Per Second" << endl;

	// Write results to log file
	chrono::system_clock::time_point p = chrono::system_clock::now();
	time_t t = chrono::system_clock::to_time_t(p);
	char strCurrentDateTime[26];
	ctime_s(strCurrentDateTime, sizeof strCurrentDateTime, &t);
	resultsFile << strCurrentDateTime << ",Poster," << posterCount << "," << postRequests << "," << posterTotalTime << "," << (postRequests / posterTotalTime) << endl;
	resultsFile << ",Reader," << readerCount << "," << readRequests << "," << readerTotalTime << "," << (readRequests / readerTotalTime) << endl;
	resultsFile.close();

	// Terminate the server
	TCPClient client(serverIp, DEFAULT_PORT);
	client.OpenConnection();
	string response = client.send("EXIT");
	client.CloseConnection();
	
	if (validate)
	{
		cout << "\nComplete validations?\n";
		system("pause");

		unsigned int invalidPostResponses = verifier->validatePostResponses();
		unsigned int invalidReadResponses = verifier->validateReadResponses();
	
		cout << "\n" << invalidPostResponses << " / " << postRequests << " invalid posts\n";
		cout << invalidReadResponses << " / " << readRequests << " invalid reads\n";

		if (invalidReadResponses == 0 && invalidPostResponses == 0)
			cout << "\nSUCCESS: no invalid responses found\n\n";
		else
			cout << "\nFAILED: some invalid responses were found\n\n";
	}
	
	delete db;
	delete verifier;

	system("pause");
	return 0;
}

void postRequest(string serverIp, unsigned int threadIndex, double timeDurationSecs, bool throttle, unsigned int posterCount, bool validate)
{
	token_bucket::Limiter* rateLimiter = new token_bucket::Limiter(THROTTLE_RATE, THROTTLE_BURST);
	RequestGenerator* requestGenerator = new RequestGenerator(threadIndex, posterCount);
	TCPClient client(serverIp, DEFAULT_PORT);
	client.OpenConnection();

	unsigned int threadPostCount = 0;
	double timeSpan;

	chrono::high_resolution_clock::time_point endTime;
	chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();
	
	do 
	{
		if (!throttle || rateLimiter->wait(THROTTLE_N, chrono::nanoseconds(0)))
		{
			string request = requestGenerator->generateWriteRequest();
			string response = client.send(request);
			if (validate)
				db->addPosterValue(request, response);
			threadPostCount++;
		}
		endTime = chrono::high_resolution_clock::now();
		timeSpan = chrono::duration_cast<chrono::duration<double>>(endTime - startTime).count();
	} while (timeSpan < timeDurationSecs);

	double totalRunTime = (endTime - startTime).count();
	double posterRequestsPerSecond = threadPostCount / timeSpan;
	tuple<double, double, unsigned int> returnValues = make_tuple(timeSpan, posterRequestsPerSecond, threadPostCount);

	mLock.lock();
	postRequests += threadPostCount;
	posterThreadMap[threadIndex] = returnValues;
	mLock.unlock();

	client.CloseConnection();
	delete requestGenerator;
	delete rateLimiter;
}

void readRequest(string serverIp, unsigned int threadIndex, double timeDurationSecs, bool throttle, unsigned int posterCount, bool validate)
{
	token_bucket::Limiter* rateLimiter = new token_bucket::Limiter(THROTTLE_RATE, THROTTLE_BURST);
	RequestGenerator* requestGenerator = new RequestGenerator(threadIndex, posterCount);
	TCPClient client(serverIp, DEFAULT_PORT);
	client.OpenConnection();

	unsigned int threadReadCount = 0;
	double timeSpan;
	chrono::high_resolution_clock::time_point endTime;
	chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();

	do
	{
		if (!throttle || rateLimiter->wait(THROTTLE_N, chrono::nanoseconds(0)))
		{
			string request = requestGenerator->generateReadRequest();
			string response = client.send(request);
			if (validate)
				db->addReaderValue(request, response);
			threadReadCount++;
		}
		endTime = chrono::high_resolution_clock::now();
		timeSpan = chrono::duration_cast<chrono::duration<double>>(endTime - startTime).count();
	} while (timeSpan < timeDurationSecs);

	double totalRunTime = (endTime - startTime).count();
	double posterRequestsPerSecond = threadReadCount / timeSpan;
	tuple<double, double, unsigned int> returnValues = make_tuple(timeSpan, posterRequestsPerSecond, threadReadCount);

	mLock.lock();
	readRequests += threadReadCount;
	readerThreadMap[threadIndex] = returnValues;
	mLock.unlock();

	client.CloseConnection();
	delete requestGenerator;
	delete rateLimiter;
}
