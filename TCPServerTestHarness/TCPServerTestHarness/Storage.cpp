#include "Storage.h"

Storage::Storage()
{
	//verifier = new ResponseVerifier();
	//currentPostIndex = 0;
}

Storage::~Storage()
{
	//delete verifier;
}

tuple<bool, string, string> Storage::addReaderValue(string request, string response)
{
	ReadRequest read = ReadRequest::parse(request);
	if (!read.valid)
	{
		return make_tuple(false, "", response);
	}

	string correct;

	lock.lock();
	try
	{
		lock.unlock();
	}
	catch(exception e)
	{
		cout << "Error: " << e.what();
		lock.unlock();
		return make_tuple(false, "", response);
	}

	return make_tuple(false, "", "");
}

void Storage::addPosterValue(string topicId, string message, string response)
{
	lock.lock();
	this->topicToMessagesWithResponse[topicId].push_back(make_tuple(message, response));
	lock.unlock();
}

map<string, vector<tuple<string, string>>> Storage::getTopicToMessagesWithResponse()
{
	return this->topicToMessagesWithResponse;
}
