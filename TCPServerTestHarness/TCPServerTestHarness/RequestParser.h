#include <string>

#ifndef __REQUESTPARSER_H
#define __REQUESTPARSER_H

class PostRequest
{
public:
	PostRequest();
	~PostRequest();
	static PostRequest parse(std::string request);
	std::string getTopicId();
	std::string getMessage();
	std::string toString();

	std::string topicId;
	std::string message;
	bool valid;
};

class ReadRequest
{
public:
	ReadRequest();
	~ReadRequest();
	static ReadRequest parse(std::string request);
	std::string getTopicId();
	int getPostId();
	std::string toString();

	std::string topicId;
	int postId;
	bool valid;
};

#endif //__REQUESTPARSER_H
