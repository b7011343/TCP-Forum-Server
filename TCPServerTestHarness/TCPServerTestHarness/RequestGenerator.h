#pragma once
#include <string>
#include <vector>
#include <future>
#include <tuple>
#include <shared_mutex>

#include "TCPClient.h"

using namespace std;

class RequestGenerator
{
public:
	RequestGenerator();
	~RequestGenerator();
	string generateWriteRequest();
	string generateReadRequest();

private:
	vector<string> writeRequests;
	vector<string> readRequests;
	int writeIndex;
	int readIndex;
	shared_mutex lock;
};
