#include "RequestGenerator.h"

RequestGenerator::RequestGenerator(int threadIndex, int posterCount)
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
		stringStream << "READ@Topic" << threadIndex % posterCount << "#" << i;
		readRequests.push_back(stringStream.str());
	}

	writeIndex = 0;
	readIndex = 0;
}

RequestGenerator::~RequestGenerator() {}

string RequestGenerator::generateWriteRequest()
{
	unsigned int i = writeIndex % writeRequests.size();
	string request = writeRequests[i];
	writeIndex++;
	return request;
}

string RequestGenerator::generateReadRequest()
{
	unsigned int i = readIndex % readRequests.size();
	string request = readRequests[i];
	readIndex++;
	return request;
}

string RequestGenerator::getRandomString(const int len) {
	static const char alphanum[] = "0123456789" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz";

	string tmp_s;
	tmp_s.reserve(len);

	for (int i = 0; i < len; ++i) {
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return tmp_s;
}
