#include "Includes.h"
#include "FIle Managment.h"
#include <ctime>
#include "RequestAnswer.h"
#include "timemanager.h"

using namespace std;

/*class QueueUnit {
public:
	QueueUnit(SOCKET newcon,sockaddr_in newid, int pos) {
		sock = newcon;
		position = pos;
		clientid = newid;
	}
	/*~QueueUnit() {
		delete& position;
		delete& sock;
		delete& clientid;
		delete& IsCompleted;
	}

	int position;
	SOCKET sock;
	sockaddr_in clientid;
	bool IsCompleted = false;

	QueueUnit* PrevQUnit = NULL;
	QueueUnit* NextQUnit = NULL;
};

class Queue {
public:
	Queue() {
		QueueUnit* FirstQUnit = NULL;
		QueueUnit* LastQUnit = NULL;
		position = 0;
	}
	void NewQUnit(SOCKET newcon, sockaddr_in newid) {
		if (FirstQUnit != NULL) {
			position++;
			QueueUnit AddedQUnit(newcon, newid, position);
			AddedQUnit.PrevQUnit = LastQUnit;
			QueueUnit* ptr = &AddedQUnit;
			LastQUnit->NextQUnit = ptr;
			LastQUnit = ptr;
		}
		else {
			QueueUnit AddedQUnit(newcon, newid, position);
			FirstQUnit = &AddedQUnit;
			LastQUnit = &AddedQUnit;
		}		
	}
	void freememoryQUnit() {
		FirstQUnit->PrevQUnit->~QueueUnit();
		delete FirstQUnit->PrevQUnit;
		FirstQUnit->PrevQUnit = NULL;
	 }
	QueueUnit* FirstQUnit;
	QueueUnit* LastQUnit;
	int position;
};*/

void RequestProc(SOCKET& ConnSocket, sockaddr_in& clientid, string &request) {
	
	int sRecieving;
	char recvbuf[DEFAULT_BUFLEN];
	int clientidlen = sizeof(clientid);
	memset(recvbuf, '0', sizeof(recvbuf));
	int TotalBytes = 0;
	
	while (true) {
		sRecieving = recvfrom(ConnSocket, recvbuf, sizeof(recvbuf), NULL, PSOCKADDR(&clientid), &clientidlen);
		//cout << NowTime() << "\t===== Recieve Data from [" << inet_ntoa(clientid.sin_addr) << "]\n";

		if (sRecieving > 0) {
			//cout << NowTime() << "\t===== Bytes recieved " << sRecieving << "\n";
			TotalBytes += sRecieving;
			request += recvbuf;
			ZeroMemory(recvbuf, DEFAULT_BUFLEN);

		}
		if (sRecieving < DEFAULT_BUFLEN) {
			cout << NowTime() << "\t===== Connection closed. Total bytes recieved: " << TotalBytes << "\n";
			break;
		}
	}
}

void ResponceProc(SOCKET& ConnSocket, sockaddr_in& clientid, string& responce) {

	int sRecieving;
	int buflen;
	int TotalBytes = 0;

	recvbuffer* entrybuf = DivideBuffer(responce);

	while (entrybuf != NULL) {
		buflen = entrybuf->buflen;
		sRecieving = sendto(ConnSocket, entrybuf->buffdata, buflen, NULL, PSOCKADDR(&clientid), sizeof(clientid));
		//cout << NowTime() << "\t===== Bytes sended " << sRecieving << " from " << buflen << '\n';
		TotalBytes += sRecieving;
		if (sRecieving == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			break;
		}
		entrybuf = entrybuf->next;
	}

	cout << NowTime() << "\t===== Total bytes sended " << TotalBytes << '\n';
	closesocket(ConnSocket);
}

void ReqRespProc(SOCKET &ConnSocket, sockaddr_in &clientid) {

	//Èíèöèàöèÿ ëîã ôàéëà
	ofstream DataFile("log.txt",ios::app);
	if (!DataFile.is_open()) {
		cout << "ERROR";
	}

	string ClientRequest;
	RequestProc(ConnSocket, clientid, ClientRequest);

	
	DataFile << NowTime() << " Request\r\n";
	DataFile << ClientRequest;

	if (ClientRequest != "") {
		ResolveClientReq(ClientRequest);
		DataFile << NowTime() << " Responce\r\n";
		DataFile << ClientRequest;

		ResponceProc(ConnSocket, clientid, ClientRequest);
	}
	
}

/*void SchedulerÑonnectionQueues(Queue& ListConn) {
	while (true) {
		if (ListConn.FirstQUnit != NULL) {
			if (!ListConn.FirstQUnit->IsCompleted) {
				cout << NowTime() << "\tClient connected: [" << inet_ntoa(ListConn.FirstQUnit->clientid.sin_addr) << "]\n";
				ReqRespProc(ListConn.FirstQUnit->sock, ListConn.FirstQUnit->clientid, ListConn.FirstQUnit->IsCompleted);
				cout << NowTime() << "\tClient disconnected: [" << inet_ntoa(ListConn.FirstQUnit->clientid.sin_addr) << "]\n";
			}
			if (ListConn.FirstQUnit->IsCompleted) {
				ListConn.FirstQUnit = ListConn.FirstQUnit->NextQUnit;
				ListConn.freememoryQUnit();
			}
		}
	}
}*/

int StartHTTPIOLoop() {
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	int state;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	SOCKET ListenSocket = INVALID_SOCKET;

	state = getaddrinfo(DEFAULT_HOST_ADRESS, DEFAULT_PORT, &hints, &result);
	if (state != 0) {
		cout << "getaddrinfo error. code = " << state;
		WSACleanup();
		return 1;
	}

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		cout << "socket failed with error :" << WSAGetLastError();
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	state = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (state == SOCKET_ERROR) {
		cout << "socket binded with error :" << WSAGetLastError();
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	state = listen(ListenSocket, SOMAXCONN);
	if (state == SOCKET_ERROR) {
		cout << "socket listen with error :" << WSAGetLastError();
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET newcon = INVALID_SOCKET;

	sockaddr_in clientid;
	int clientidlen = sizeof(clientid);
	int RcvState = 0;
	//Queue ListConn;
	//thread AcThread(SchedulerÑonnectionQueues, ref(ListConn));
	//AcThread.detach();
	cout << "Server Initialized\n";
	while (true) {
		newcon = accept(ListenSocket, PSOCKADDR(&clientid), &clientidlen);
		ReqRespProc(newcon, clientid);
		newcon = INVALID_SOCKET;
	}
}	
