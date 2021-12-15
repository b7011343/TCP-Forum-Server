#include "Storage.h"

Storage::Storage()
{
	verifier = new ResponseVerifier();
	//currentPostIndex = 0;
}

Storage::~Storage()
{
	delete verifier;
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
		correct = topicToMessages.at(read.topicId).at(stoi(response));
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

tuple<bool, int, int> Storage::addPosterValue(int postIndex, string request, string response)
{
	PostRequest post = PostRequest::parse(request);
	if (!post.valid)
	{
		return make_tuple(false, NULL, stoi(response));
	}

	lock.lock();
	this->topicToMessages[post.getTopicId()].push_back(post.getMessage());
	//posterRequestResponseMap[post.getTopicId()].push_back(response);
	tuple<bool, int, int> validTuple = verifier->correctPostResponse(post.getTopicId(), response, topicToMessages);
	//currentPostIndex++;
	lock.unlock();

	return validTuple;
}

