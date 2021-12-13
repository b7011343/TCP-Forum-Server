#pragma once
#include <map>
#include <future>
#include <string>
#include <vector>
#include <shared_mutex>

#include "ResponseVerifier.h"
#include "RequestParser.h"

using namespace std;

class Storage
{
public:
	Storage();
	~Storage();
	bool addReaderValue(string request, string response);
	tuple<bool, int, int> addPosterValue(string request, string response);
	string getReaderResponse(string request);
	string getPosterResponse(string request);

private:
	map<string, string> readerRequestResponseMap;
	map<string, vector<string>> posterRequestResponseMap;
	ResponseVerifier* verifier;
	shared_mutex lock;
};
