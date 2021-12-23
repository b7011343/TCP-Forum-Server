#include "ResponseVerifier.h"

ResponseVerifier::ResponseVerifier(Storage* db)
{
	this->db = db;
}

ResponseVerifier::~ResponseVerifier() {}

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
				cout << "\nInvalid Post Response!\n";
				cout << "Expected Post Response: " << i << "\n";
				cout << "Received Post Response: " << response << "\n";
			}
		}
	}

	return invalidResponses;
}

int ResponseVerifier::validateReadResponses()
{
	int invalidResponses = 0;

	lock.lock_shared();
	map<string, vector<tuple<string, string>>> topicToMessagesWithResponse = db->getTopicToMessagesWithResponse();
	map<string, vector<tuple<int, string>>> readerRequestsWithResponses = db->getReaderRequestsWithResponses();
	lock.unlock_shared();

	for (auto topic : readerRequestsWithResponses)
	{
		string topicId = topic.first;
		vector<tuple<int, string>> messageIdsWithResponses = readerRequestsWithResponses[topicId];
		for (int i = 0; i < messageIdsWithResponses.size(); i++)
		{
			int messageId = get<0>(messageIdsWithResponses[i]);
			string response = get<1>(messageIdsWithResponses[i]);
			string expectedResponse = get<0>(topicToMessagesWithResponse[topicId][messageId]);
			if (response != expectedResponse)
			{
				invalidResponses++;
				cout << "\nInvalid Read Response!\n";
				cout << "Expected Read Response: " << expectedResponse << "\n";
				cout << "Received Read Response: " << response << "\n";
			}
		}
	}

	return invalidResponses;
}
