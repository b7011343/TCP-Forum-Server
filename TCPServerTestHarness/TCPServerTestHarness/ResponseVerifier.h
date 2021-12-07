#pragma once

class ResponseVerifier
{
public:
	ResponseVerifier();
	~ResponseVerifier();

private:
	bool verifyPost();
	bool verifyRead();

};
