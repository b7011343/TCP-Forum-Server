#include <iostream>

#include "TCPClient.h"
#include "ThreadPool.h"

#define DEFAULT_PORT 12345

int main(int argc, char **argv)
{
	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s server-name|IP-address\n", argv[0]);
		return 1;
	}

	TCPClient client(argv[1], DEFAULT_PORT);
	std::string request;

	ThreadPool readerPool(2);
	ThreadPool writerPool(1);

	client.OpenConnection();

	do {
		request = "";
		std::cout << "Enter string to send to server or \"exit\" (without quotes to terminate): ";
		std::getline(std::cin, request);

		std::cout << "String sent: " << request << std::endl;
		std::cout << "Bytes sent: " << request.size() << std::endl;

		std::string reply = client.send(request);

		std::cout << "String returned: " << reply << std::endl;
		std::cout << "Bytes received: " << reply.size() << std::endl;
	} while (request != "exit");

	client.CloseConnection();

	return 0;
}
