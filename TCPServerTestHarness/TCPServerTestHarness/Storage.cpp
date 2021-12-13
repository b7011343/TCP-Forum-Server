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

bool Storage::addPosterValue(string request, string response)
{
	PostRequest post = PostRequest::parse(request);
	if (!post.valid)
	{
		return false;
	}
	lock.lock();
	posterRequestResponseMap[post.getTopicId()].push_back(response);
	bool valid = verifier->verifyPost(post.getTopicId(), response, posterRequestResponseMap);
	lock.unlock();
	return valid;
}

string Storage::getReaderResponse(string request)
{
	return "";
}

string Storage::getPosterResponse(string request)
{
	return "";
}
