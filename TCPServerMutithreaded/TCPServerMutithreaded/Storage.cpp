#include "Storage.h"
#include <string>

using namespace std;

Storage::Storage() {}

Storage::~Storage() {}

int Storage::countRequest(string topicId)
{
	return this->topicToMessages[topicId].size();
}

string Storage::listRequest() // check what return value should be if there are 0 topics
{
	string list = "";
	for (auto topicId : topicIds()) {
		list += "@" + topicId + "#";
	}
	list.pop_back();
	return list;
}

string Storage::readRequest(string topicId, int messageId)
{
	string topicIdTrunc = truncate(topicId);
	bool isValid = this->topicToMessages.find(topicIdTrunc) == this->topicToMessages.end() && this->topicToMessages[topicIdTrunc].size() >= messageId;
	if (!isValid)
	{
		return "";
	}

	return this->topicToMessages[topicIdTrunc][messageId];
}

int Storage::postRequest(string topicId, string message)
{
	string messageTrunc = truncate(message);
	string topicIdTrunc = truncate(topicId);
	int messageId = this->topicToMessages[topicIdTrunc].size();
	this->topicToMessages[topicIdTrunc].push_back(messageTrunc);
	return messageId;
}

vector<string> Storage::topicIds() {
	std::vector<std::string> keys;
	for (auto topic : this->topicToMessages) {
		keys.push_back(topic.first);
	}
	return keys;
}

string Storage::truncate(string text, int length)
{
	int adjustedLength = text.length() > length ? length : text.length();
	return text.substr(0, adjustedLength);
}
