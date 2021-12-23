#pragma once
#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <shared_mutex>
#include <iostream>

#include "Storage.h"

using namespace std;

class ResponseVerifier
{
public:
	ResponseVerifier(Storage* db);
	~ResponseVerifier();
	int validatePostResponses();
	int validateReadResponses();

private:
	shared_mutex lock;
	Storage* db;
};
