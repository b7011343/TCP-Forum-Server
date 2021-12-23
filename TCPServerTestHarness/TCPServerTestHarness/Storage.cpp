#include "Storage.h"

Storage::Storage() {}

Storage::~Storage() {}

void Storage::addReaderValue(string request, string response)
{
	ReadRequest read = ReadRequest::parse(request);
	lock.lock();
	this->readerRequestsWithResponses[read.getTopicId()].push_back(make_tuple(read.getPostId(), response));
	lock.unlock();
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

map<string, vector<tuple<int, string>>> Storage::getReaderRequestsWithResponses()
{
	return this->readerRequestsWithResponses;
}
