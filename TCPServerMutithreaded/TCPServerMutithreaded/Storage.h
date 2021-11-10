#pragma once
#include <string>
#include <map>
#include <vector>

using namespace std;

class Storage
{
public:
	Storage();
	~Storage();
	int postRequest(string topicId, string message);
	string readRequest(string topicId, int messageId);
	int countRequest(string topicId);
	string listRequest();
	vector<string> topicIds();

private:
	map<string, vector<string>> topicToMessages;
	static string truncate(string text, int length = 140);
};
