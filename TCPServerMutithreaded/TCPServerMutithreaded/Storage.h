#pragma once
#include <string>
#include <map>
#include <vector>
#include <shared_mutex>

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

private:
	shared_mutex lock;
	map<string, vector<string>> topicToMessages;
	
	vector<string> topicIds();
	static string truncate(string text, int length);
};
