#include "Storage.h"
#include <string>

using namespace std;

Storage::Storage() {}

Storage::~Storage() {}

int Storage::countRequest(string topicId)
{
	lock.lock_shared();
	int count = this->topicToMessages[topicId].size();
	lock.unlock_shared();
	return count;
}

string Storage::listRequest()
{
	string list = "";
	vector<string> topicIdVec = topicIds();
	if (topicIdVec.size() < 1)
	{
		return list;
	}
	for (auto topicId : topicIdVec) {
		list += topicId + "#";
	}
	list.pop_back();
	return list;
}

string Storage::readRequest(string topicId, int messageId)
{
	string topicIdTrunc = truncate(topicId, 140);
	lock.lock_shared();
	bool isValid = this->topicToMessages.count(topicIdTrunc) && this->topicToMessages[topicIdTrunc].size() > messageId;
	if (!isValid)
	{
		lock.unlock_shared();
		return "";
	}
	string message = this->topicToMessages[topicIdTrunc][messageId];
	lock.unlock_shared();
	return message;
}

int Storage::postRequest(string topicId, string message)
{
	string messageTrunc = truncate(message, 140);
	string topicIdTrunc = truncate(topicId, 140);
	unsigned int messageId = this->topicToMessages[topicIdTrunc].size();
	lock.lock();
	this->topicToMessages[topicIdTrunc].push_back(messageTrunc);
	lock.unlock();
	return messageId;
}

vector<string> Storage::topicIds() {
	std::vector<std::string> keys;
	lock.lock_shared();
	for (auto topic : this->topicToMessages) {
		keys.push_back(topic.first);
	}
	lock.unlock_shared();
	return keys;
}

inline string Storage::truncate(string text, int length)
{
	unsigned int adjustedLength = text.length() > length ? length : text.length();
	return text.substr(0, adjustedLength);
}
