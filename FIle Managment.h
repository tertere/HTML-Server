// FIle Managment.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

struct recvbuffer {
	char* buffdata;
	int buflen;
	recvbuffer* next;
	recvbuffer* prev;
};

recvbuffer* DivideBuffer(string data) {
	int size = data.size();
	int count = 0;
	recvbuffer *currentbuff = (recvbuffer*)malloc(sizeof(recvbuffer));
	currentbuff->prev = NULL;
	currentbuff->next = NULL;

	while (size > 0) {
		if (size > DEFAULT_BUFLEN) {
			char* tempbuffdata = new char[DEFAULT_BUFLEN];
			for (int i = 0; i < DEFAULT_BUFLEN; i++) {
				tempbuffdata[i] = data[count];
				count++;
			}
			currentbuff->buffdata = tempbuffdata;
			currentbuff->buflen = DEFAULT_BUFLEN;
			size -= DEFAULT_BUFLEN;
			recvbuffer* nextbuf = new recvbuffer;
			currentbuff->next = nextbuf;
			nextbuf->prev = currentbuff;
			currentbuff = nextbuf;
		} 
		else {
			char* tempbuffdata = new char[size];
			for (int i = 0; i < size; i++) {
				tempbuffdata[i] = data[count];
				count++;
			}
			currentbuff->buffdata = tempbuffdata;
			currentbuff->buflen = size;
			size = 0;
			currentbuff->next = NULL;
		}
	}
	while (currentbuff->prev != NULL) {
		currentbuff = currentbuff->prev;
	}
	
	return currentbuff;

}

void GetMIMEType(string path, string& MIMEType) {
	int i = 0;
	while ((path[i] != '.') && (i < path.size())) {
		i++;
	}
	i++;
	string temp;
	for (; i < path.size(); i++) {
		temp += path[i];
	}
	if (temp == "html") {
		MIMEType = "text/html; charset=utf-8";
	}
	if (temp == "css") {
		MIMEType = "text/css";
	}
	if (temp == "png") {
		MIMEType = "image/png";
	}
}

string GetFile(string path, int &statecode, string &MIMEType) {

	string RealPath = "badguy/server";
	RealPath += path;
	GetMIMEType(RealPath, MIMEType);
	//Непонятно как работает на релизе
	ifstream file(RealPath,ifstream::binary);
	if (!file) {
		cout << "Error opening file: " << RealPath << "\n";
		file.close();
		RealPath = "badguy/server/404.html";
		file.open(RealPath);
		
		statecode = 404;
	}
	cout << RealPath << "\n";
	char a = 'q';
	string DataFile;
	while (!file.eof()) {
		a = file.get();
		if /* ((a != '\n') && (a != '\t') && */ (!file.eof()) {
			DataFile += a;
		}
	} 
	//cout << DataFile;
	return DataFile;
}

string ResponseCreator(string data, int& statecode, string& MIMEType) {


	//status
	string response = "HTTP/1.1 ";
	if (statecode == 200) {
		response += "200 OK";
	}
	if (statecode == 404) {
		response += "404 Not Found";
	}
	if (statecode == 302) {
		response += "302 Found";
		response += "\r\nLocation: ";
		response += "http://localhost/";
		response += "login.html\r\n\r\n";
		return response;
	}

	//current time
	time_t CurrTime;
	CurrTime = time(NULL);
	tm tm_gmt;
	gmtime_s(& tm_gmt, & CurrTime);
	char buf[26];
	strftime(buf, 26, "%a, %d %b %Y %H:%M:%S", &tm_gmt);
	response += "\r\nDate: ";
	for (int i = 0; i < 26; i++) {
		if (buf[i] != '\0') response += buf[i];
	}
	response += " GMT\r\n";

	response += "Version: HTTP/1.1";
	//response += "\nAccept-Ranges: bytes";

	//server
	response += "\r\nServer: BadGuy/0.2 (Win32)";

	//last-modified
	response += "\r\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT";

	//content length
	response += "\r\nContent-Length: ";
	//response += to_string(data.size() + 180);
	response += to_string(data.size());

	//content type
	response += "\r\nContent-Type: ";
	response += MIMEType;	

	//connection closed
	//response += "\r\nConnection: Closed\r\n\r\n";

	response += "\r\n\r\n";
	response += data;

	//cout << response;
	return response;
}