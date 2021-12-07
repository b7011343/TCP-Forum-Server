#pragma once
#include <map>
#include <future>
#include <string>
#include <vector>

using namespace std;

class Storage
{
public:
	Storage();
	~Storage();
	void addReaderValue(string request, future<string> responseFuture);
	void addWriterValue(string request, future<string> responseFuture);
	string getReaderResponse(string request);
	string getWriterResponse(string request);

private:
	map<string, future<string>> readerRequestResponseMap;
	map<string, future<string>> writerRequestResponseMap;
};
