#pragma once
#include <string>
#include <map>
#include <vector>
#include <shared_mutex>

using namespace std;

class ResponseVerifier
{
public:
	ResponseVerifier();
	~ResponseVerifier();
	bool verifyPost(string topicId, string response, map<string, vector<string>> map);
	bool verifyRead(string request, string response, map<string, vector<string>> map);

private:
};
