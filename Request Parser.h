// Request Parser.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

typedef struct Node {
    string* NodeName;
    string* NodeValue;
    struct Node* NodeNext;
    struct Node* NodePrev;
} Node_t;


Node_t* new_node(string msg) {
    Node_t* pointer;
    if ((pointer = (Node_t*)malloc(sizeof(Node_t))) == NULL) {
        return NULL;
    }
    string* lpmsg =  new string(msg);
    pointer->NodeName = lpmsg;
    pointer->NodeNext = NULL;
    return pointer;
}

Node_t* ParseRequest(string request) {
    string buf;
    int CRLF = 0;
    bool match = false;
    Node_t* PrevNode;
    if ((PrevNode = (Node_t*)malloc(sizeof(Node_t))) == NULL) {
        return NULL;
    }
    string msg = "firstnode";
    PrevNode->NodeName = new string(msg);
    PrevNode->NodePrev = NULL;
    PrevNode->NodeNext = NULL;
    for (int i = 0; i < request.size(); i++) {
        if ((request[i] == ':') && (!match)) {
            Node_t* NewNode = new_node(buf);
            NewNode->NodePrev = PrevNode;
            NewNode->NodeNext = NULL;
            PrevNode->NodeNext = NewNode;
            buf = "";
            PrevNode = NewNode;
            match = true;
        }
        else if ((request[i] == '\n') || (request[i] == '\r')) {
            CRLF++;
            if (CRLF == 2) {

                PrevNode->NodeValue = new string(buf);
                CRLF = 0;
                buf = "";
                match = false;
            }
        } else buf += request[i];
    }
    return PrevNode;

}