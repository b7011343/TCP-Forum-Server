#include "RequestGenerator.h"

RequestGenerator::RequestGenerator(int threadIndex)
{
	for (int i = 0; i < 10; i++)
	{
		stringstream stringStream;
		stringStream << "POST@Topic" << threadIndex << "#" << getRandomString(5);
		writeRequests.push_back(stringStream.str());
	}

	for (int i = 0; i < 10; i++)
	{
		stringstream stringStream;
		stringStream << "READ@Topic" << threadIndex << "#" << i;
		readRequests.push_back(stringStream.str());
	}

	writeIndex = 0;
	readIndex = 0;
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

string RequestGenerator::getRandomString(const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	string tmp_s;
	tmp_s.reserve(len);

	for (int i = 0; i < len; ++i) {
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return tmp_s;
}
