#include "ResponseVerifier.h"

ResponseVerifier::ResponseVerifier()
{

}

ResponseVerifier::~ResponseVerifier()
{

}

tuple<bool, int, int> ResponseVerifier::correctPostResponse(string topicId, string response, map<string, vector<string>> map)
{
	int correctResponse = map[topicId].size() - 1;
	return make_tuple(correctResponse == stoi(response), correctResponse, stoi(response));
}

bool ResponseVerifier::verifyRead(string request, string response, map<string, vector<string>> map)
{
	return false;
}
