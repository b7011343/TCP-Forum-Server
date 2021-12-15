#include "RequestGenerator.h"

RequestGenerator::RequestGenerator()
{
	writeIndex = 0;
	readIndex = 0;
	writeRequests = { "POST@Topic1#This is a test", "POST@Topic1#This is another test", "POST@Topic2#This is a test for an additional topic" };
	readRequests = { "READ@Topic1#0", "READ@Topic1#1", "READ@Topic2#0" };
}

RequestGenerator::~RequestGenerator() {}

string RequestGenerator::generateWriteRequest()
{
	lock.lock();
	int i = writeIndex % writeRequests.size();
	writeIndex++;
	string request = writeRequests[i];
	lock.unlock();
	return request;
}

string RequestGenerator::generateReadRequest()
{
	lock.lock();
	int i = readIndex % readRequests.size();
	readIndex++;
	string request = readRequests[i];
	lock.unlock();
	return request;
}
