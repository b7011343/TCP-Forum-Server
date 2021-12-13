#pragma once
#include <string>
#include <map>
#include <vector>
#include <tuple>

using namespace std;

class ResponseVerifier
{
public:
	ResponseVerifier();
	~ResponseVerifier();
	tuple<bool, int, int> correctPostResponse(string topicId, string response, map<string, vector<string>> map);
	bool verifyRead(string request, string response, map<string, vector<string>> map);

private:
};
