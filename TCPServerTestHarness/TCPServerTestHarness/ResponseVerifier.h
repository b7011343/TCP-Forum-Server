#pragma once
#include <string>

using namespace std;

class ResponseVerifier
{
public:
	ResponseVerifier();
	~ResponseVerifier();
	bool verifyPost(string request, string response);
	bool verifyRead(string request, string response);

private:

};
