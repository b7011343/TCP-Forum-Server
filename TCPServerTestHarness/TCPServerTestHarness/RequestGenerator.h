#pragma once
#include <string>
#include <vector>
#include <future>
#include <tuple>
#include <shared_mutex>
#include <sstream>

#include "TCPClient.h"

using namespace std;

class RequestGenerator
{
public:
	RequestGenerator(int threadIndex);
	~RequestGenerator();
	string generateWriteRequest();
	string generateReadRequest();
	string getRandomString(const int len);

private:
	vector<string> writeRequests;
	vector<string> readRequests;
	int writeIndex;
	int readIndex;
	shared_mutex lock;
};
