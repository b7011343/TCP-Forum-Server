#pragma once
#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <tuple>

#include "RequestParser.h"

using namespace std;

class Storage
{
public:
	Storage();
	~Storage();
	void addReaderValue(string request, string response);
	void addPosterValue(string request, string response);
	map<string, vector<tuple<string, string>>> getTopicToMessagesWithResponse();
	map<string, vector<tuple<int, string>>> getReaderRequestsWithResponses();

private:
	map<string, vector<tuple<string, string>>> topicToMessagesWithResponse;
	map<string, vector<tuple<int, string>>> readerRequestsWithResponses;
	mutex lock;
};
