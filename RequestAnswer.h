#include <string>
#include "Request Parser.h"

using namespace std;

/*
	GET = 'G';
	
*/

void GetMethod(Node_t* entrynode,char& Method, string& path) {
	while (entrynode->NodePrev != NULL) {
		if (*entrynode->NodeName == "firstnode") {
			break;
		}
		entrynode = entrynode->NodePrev;
	}
	string HeaderString = *entrynode->NodeValue;
	string temp;
	int index = 0;
	while (HeaderString[index] != ' ') {
		temp += HeaderString[index];
		index++;
	}
	if (temp == string("GET")) {
		Method = 'G';
	}
	temp = "";
	index++;
	while (HeaderString[index] != ' ') {
		temp += HeaderString[index];
		index++;
	}
	path = temp;
}

void ResolveClientReq(string& data) {
	Node_t* entrynode = ParseRequest(data);
	char Method;
	string adress;
	GetMethod(entrynode, Method, adress);

	int state = 100;
	string MIMEType = "text/html";
	if (adress == "/") {
		state = 302;
	}
	if (state != 302) {
		data = GetFile(adress, state, MIMEType);
	}
	data = ResponseCreator(data, state, MIMEType);

}
