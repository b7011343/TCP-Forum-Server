#pragma once
#include <map>
#include <future>
#include <string>
#include <iostream>
#include <vector>
#include <shared_mutex>
#include <condition_variable>
#include <queue>

#include "ResponseVerifier.h"
#include "RequestParser.h"

using namespace std;

class Storage
{
public:
	Storage();
	~Storage();
	tuple<bool, string, string> addReaderValue(string request, string response);
	tuple<bool, int, int> addPosterValue(int postIndex, string request, string response);

private:
	map<string, vector<string>> topicToMessages;
	//map<string, string> readerRequestResponseMap;
	//map<string, vector<string>> posterRequestResponseMap;
	ResponseVerifier* verifier;
	mutex lock;
	//int currentPostIndex;
};
