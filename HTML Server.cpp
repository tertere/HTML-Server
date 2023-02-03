#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include "Includes.h"
#include "core.h"

//main файл

using namespace std;

int main() {
	WSADATA wsadata;
	int StateCode;

	StateCode = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (StateCode != 0) {
		cout << "WsaStartup error. Code = " << StateCode;
		return 1;
	}

	StartHTTPIOLoop();
}