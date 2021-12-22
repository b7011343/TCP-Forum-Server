#pragma once
#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <shared_mutex>

#include "Storage.h"

using namespace std;

class ResponseVerifier
{
public:
	ResponseVerifier(Storage* db);
	~ResponseVerifier();
	int validatePostResponses();
	tuple<bool, int, int> correctPostResponse(string topicId, string response, map<string, vector<string>> map);
	tuple<bool, string, string> correctReadResponse(string request, string response, map<string, vector<string>> map);

private:
	shared_mutex lock;
	Storage* db;
};
