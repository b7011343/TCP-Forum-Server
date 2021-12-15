#pragma once
#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <shared_mutex>

using namespace std;

class ResponseVerifier
{
public:
	ResponseVerifier();
	~ResponseVerifier();
	tuple<bool, int, int> correctPostResponse(string topicId, string response, map<string, vector<string>> map);
	tuple<bool, string, string> correctReadResponse(string request, string response, map<string, vector<string>> map);

private:
	shared_mutex lock;
};
