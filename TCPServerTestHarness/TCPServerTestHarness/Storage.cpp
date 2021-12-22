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

void Storage::addPosterValue(string request, string response)
{
	PostRequest post = PostRequest::parse(request);
	lock.lock();
	this->topicToMessagesWithResponse[post.getTopicId()].push_back(make_tuple(post.getMessage() , response));
	lock.unlock();
}

map<string, vector<tuple<string, string>>> Storage::getTopicToMessagesWithResponse()
{
	return this->topicToMessagesWithResponse;
}
