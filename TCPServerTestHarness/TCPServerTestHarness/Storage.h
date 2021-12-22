#pragma once
#include <map>
#include <future>
#include <string>
#include <iostream>
#include <vector>
#include <shared_mutex>
#include <condition_variable>
#include <queue>
#include <tuple>

//#include "ResponseVerifier.h"
#include "RequestParser.h"

using namespace std;

class Storage
{
public:
	Storage();
	~Storage();
	tuple<bool, string, string> addReaderValue(string request, string response);
	void addPosterValue(string request, string response);
	map<string, vector<tuple<string, string>>> getTopicToMessagesWithResponse();

private:
	map<string, vector<tuple<string, string>>> topicToMessagesWithResponse;
	mutex lock;
};
