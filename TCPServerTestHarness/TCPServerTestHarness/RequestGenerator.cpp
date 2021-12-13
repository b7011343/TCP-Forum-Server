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
	unsigned int i = writeIndex % writeRequests.size();
	lock.lock();
	writeIndex++;
	lock.unlock();
	return writeRequests[i];
}

string RequestGenerator::generateReadRequest()
{
	unsigned int i = readIndex % readRequests.size();
	readIndex++;
	return readRequests[i];
}
