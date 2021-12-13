#include "Storage.h"

Storage::Storage()
{
	verifier = new ResponseVerifier();
}

Storage::~Storage()
{
	delete verifier;
}

bool Storage::addReaderValue(string request, string response)
{
	return false;
}

tuple<bool, int, int> Storage::addPosterValue(string request, string response)
{
	PostRequest post = PostRequest::parse(request);
	if (!post.valid)
	{
		return make_tuple(false, NULL, stoi(response));
	}
	lock.lock();
	posterRequestResponseMap[post.getTopicId()].push_back(response);
	tuple<bool, int, int> validTuple = verifier->correctPostResponse(post.getTopicId(), response, posterRequestResponseMap);
	lock.unlock();
	return validTuple;
}

string Storage::getReaderResponse(string request)
{
	return "";
}

string Storage::getPosterResponse(string request)
{
	return "";
}
