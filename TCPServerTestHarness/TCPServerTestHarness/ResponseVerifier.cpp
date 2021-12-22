#include "ResponseVerifier.h"

ResponseVerifier::ResponseVerifier(Storage* db)
{
	this->db = db;
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

int ResponseVerifier::validatePostResponses()
{
	int invalidResponses = 0;

	lock.lock_shared();
	map<string, vector<tuple<string, string>>> topicToMessagesWithResponse = db->getTopicToMessagesWithResponse();
	lock.unlock_shared();

	for (auto topic : topicToMessagesWithResponse)
	{
		string topicId = topic.first;
		vector<tuple<string, string>> messagesWithResponses = topicToMessagesWithResponse[topicId];

		for (int i = 0; i < messagesWithResponses.size(); i++)
		{
			int response = stoi(get<1>(messagesWithResponses[i]));
			if (response != i)
			{
				invalidResponses++;
				cout << "\nInvalid Response!\n";
				cout << "\nExpected Response: " << i << "\n";
				cout << "\nReceived Response: " << response << "\n";
			}
		}
	}

	return invalidResponses;
}
