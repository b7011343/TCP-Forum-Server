#pragma once
#include <string>
#include <vector>

using namespace std;

class RequestGenerator
{
public:
	RequestGenerator(bool isThrottled);
	~RequestGenerator();
	string generateWriteRequest();
	string generateReadRequest();

private:
	vector<string> writeRequests;
	vector<string> readRequests;
};
