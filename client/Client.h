#ifndef LAB2_CLIENT_H
#define LAB2_CLIENT_H

#include <string>
#include <libc.h>
#include <iostream>

#include "FileManager.h"

typedef int descriptor_t;

const int SrcPort = 40000;

class Client {
private:
    const int FileReceivedSuccessfully = 0;
    const int FileReceivedFail = 1;


    std::string destIpAddr;
    int destPort;
    int srcPort;
    descriptor_t tcpSocket;

    FileManager file;
public:
    Client(std::string &filePath, std::string &destIpAddr, int destPort);

    ~Client();

    void sendFile();

private:
    descriptor_t createTCPSocket();

    void closeSocket();
    void recvAnswer() const;
};


#endif //LAB2_CLIENT_H
