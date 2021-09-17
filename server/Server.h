#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <iostream>
#include <vector>
#include <thread>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <libc.h>
#include <libproc.h>

typedef int descriptor_t;

const int Backlog = 10; //маскимальная очередь соединений
constexpr int BufferLength = 1024 * 4;

class Server {
private:
    const int FileReceivedSuccessfully = 0;
    const int FileReceivedFail = 1;

    int srcPort;
    descriptor_t tcpSocket;
public:
    explicit Server(int srcPort_);

    ~Server();

    void run();

private:
    unsigned int recvFileNameLength(descriptor_t clientSock);

    std::string recvFileName(descriptor_t clientSock, unsigned int fileNameLength);

    unsigned long recvFileLength(descriptor_t clientSock);

    bool recvFile(descriptor_t clientSock, std::string &fileName, unsigned long fileLength);

    descriptor_t createSocket();

    std::ofstream createNewFile(std::string name);

    void listenSocket();

    void acceptNewClient();

    void processClient(descriptor_t socket);

    void sendResponse(descriptor_t clientSock, bool isSuccess);

    int calcTimeDistance(std::chrono::time_point<std::chrono::system_clock> startTime,
                         std::chrono::time_point<std::chrono::system_clock> endTime);
};


#endif //SERVER_SERVER_H
