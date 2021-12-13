#include "ResponseVerifier.h"

ResponseVerifier::ResponseVerifier()
{

}

ResponseVerifier::~ResponseVerifier()
{

}

bool ResponseVerifier::verifyPost(string topicId, string response, map<string, vector<string>> map)
{
	int correctResponse = map[topicId].size() - 1;
	return correctResponse == stoi(response);
}

bool ResponseVerifier::verifyRead(string request, string response, map<string, vector<string>> map)
{
	return false;
}
