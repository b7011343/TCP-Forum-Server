#pragma once
#include <string>
#include <vector>
#include <tuple>

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
};
