#include "RequestGenerator.h"

RequestGenerator::RequestGenerator()
{
	writeIndex = 0;
	readIndex = 0;
	writeRequests = { "POST@Topic1#This is a test", "POST@Topic1#This is another test", "POST@Topic2#This is a test for an additional topic" };
	readRequests = { "READ@Topic1#0", "READ@Topic1#1", "READ@Topic2#0", "READ@Topic1#2", "READ@Topic1#3", "READ@Topic2#1", "READ@Topic1#4", "READ@Topic1#5", "READ@Topic2#4" };
}

RequestGenerator::~RequestGenerator() {}

string RequestGenerator::generateWriteRequest()
{
	lock.lock_shared();
	int i = writeIndex % writeRequests.size();
	string request = writeRequests[i];
	lock.unlock_shared();
	lock.lock();
	writeIndex++;
	lock.unlock();
	return request;
}

string RequestGenerator::generateReadRequest()
{
	lock.lock_shared();
	int i = readIndex % readRequests.size();
	string request = readRequests[i];
	lock.unlock_shared();
	lock.lock();
	readIndex++;
	lock.unlock();
	return request;
}
