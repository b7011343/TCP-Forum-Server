#pragma once
#include <string>
#include <vector>
#include <future>
#include <tuple>
#include <mutex>

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
	mutex lock;
};
