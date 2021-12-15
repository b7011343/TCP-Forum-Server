#include "ResponseVerifier.h"

ResponseVerifier::ResponseVerifier()
{

}

ResponseVerifier::~ResponseVerifier()
{

}

tuple<bool, int, int> ResponseVerifier::correctPostResponse(string topicId, string response, map<string, vector<string>> map)
{
	lock.lock_shared();
	int correctResponse = map[topicId].size() - 1;
	lock.unlock_shared();
	return make_tuple(correctResponse == stoi(response), correctResponse, stoi(response));
}

tuple<bool, string, string> ResponseVerifier::correctReadResponse(string request, string response, map<string, vector<string>> map)
{
	return make_tuple(false, "", "");
}
