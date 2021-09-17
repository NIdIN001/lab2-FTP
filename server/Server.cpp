#include "Server.h"

typedef std::chrono::system_clock timer;

Server::Server(int srcPort_) {
    srcPort = srcPort_;
    tcpSocket = createSocket();

    if (!std::filesystem::exists("uploads"))
        std::filesystem::create_directory("uploads");
}

Server::~Server() {
    close(tcpSocket);
}

descriptor_t Server::createSocket() {
    descriptor_t s;
    sockaddr_in ssin;

    memset(&ssin, 0, sizeof(ssin));
    ssin.sin_family = AF_INET;
    ssin.sin_port = htons(srcPort);
    ssin.sin_addr.s_addr = INADDR_ANY;

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cout << "socket error" << std::endl;
        throw std::exception();
    }

    if ((bind(s, (sockaddr *) &ssin, sizeof(ssin))) == -1) {
        std::cout << "bind error" << std::endl;
        close(s);
        throw std::exception();
    }

    return s;
}

void Server::listenSocket() {
    if (listen(tcpSocket, Backlog) == -1) {
        std::cout << "listen error" << std::endl;
        throw std::exception();
    }
}

void Server::run() {
    std::cout << "Server listening" << std::endl;
    while (true) {
        listenSocket();
        acceptNewClient();
    }
}

void Server::acceptNewClient() {
    sockaddr_un clientAddr;
    socklen_t len = sizeof(clientAddr);
    memset(&clientAddr, 0, sizeof(struct sockaddr_un));

    descriptor_t clientSock = -1;
    if ((clientSock = accept(tcpSocket, (struct sockaddr *) &clientAddr, &len)) == -1) {
        std::cout << "accept error" << std::endl;
        close(clientSock);
        return;
    }

    /* сделал новый тред и отделил его чтобы он сам закончился когда закончит */
    std::thread clientThread(&Server::processClient, this, clientSock);
    clientThread.detach();
}

void Server::processClient(descriptor_t clientSock) {
    try {
        unsigned int fileNameLength = recvFileNameLength(clientSock);
        std::string fileName = recvFileName(clientSock, fileNameLength);
        unsigned long fileLength = recvFileLength(clientSock);

        bool status = recvFile(clientSock, fileName, fileLength);

        sendResponse(clientSock, status);
    } catch (std::exception &exception) {
        std::cout << "error while process client" << std::endl;
    }

    close(clientSock);
}

unsigned int Server::recvFileNameLength(descriptor_t clientSock) {
    /* получил длину названия */
    unsigned int fileNameLength = 0;
    recv(clientSock, &fileNameLength, sizeof(unsigned int), 0);
    return fileNameLength;
}

std::string Server::recvFileName(descriptor_t clientSock, unsigned int fileNameLength) {
    /* получил название */
    char fileName[fileNameLength + 1];
    memset(fileName, 0, fileNameLength + 1);
    recv(clientSock, fileName, fileNameLength, 0);
    return std::string(fileName);
}

unsigned long Server::recvFileLength(descriptor_t clientSock) {
    /* получил длину файла */
    unsigned long fileLength = 0;
    recv(clientSock, &fileLength, sizeof(unsigned long), 0);
    return fileLength;
}

int Server::calcTimeDistance(std::chrono::time_point<std::chrono::system_clock> startTime, std::chrono::time_point<std::chrono::system_clock> endTime) {
    return std::chrono::duration_cast<std::chrono::seconds>(endTime.time_since_epoch()).count()
           - std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch()).count();
}

bool Server::recvFile(descriptor_t clientSock, std::string& fileName, unsigned long fileLength) {
    std::ofstream newFile = createNewFile(fileName);

    bool isPrintedSpeed = false;
    long bytesRecv = 0;
    int recvInLast3Sec = 0;
    char *buffer = new char[BufferLength];

    auto startTime = timer::now();
    auto curTime = startTime;

    while (bytesRecv < fileLength) {
        memset(buffer, 0, BufferLength);

        curTime = timer::now();
        if (calcTimeDistance(startTime, curTime) >= 3) {
            isPrintedSpeed = true;
            std::cout << "file \"" << fileName << "\" current speed: " << recvInLast3Sec / 3 << " bytes/sec"
                      << std::endl;
            recvInLast3Sec = 0;
            startTime = curTime;
        }

        int received = recv(clientSock, buffer, BufferLength, 0);
        switch (received) {
            case (-1): {
                newFile.close();
                delete[] buffer;
                return false;
            }

            case (0): {
                std::cout << "file \"" << fileName << "\" close connection" << std::endl;
                newFile.close();
                delete[] buffer;
                return false;
            }

            default: {
                bytesRecv += received;
                recvInLast3Sec += received;
                newFile.write(buffer, received);
            }
        }
    }

    std::cout << "Recv file \"" << fileName << "\" length: " << bytesRecv << std::endl;
    if (!isPrintedSpeed) {
        std::cout << "Speed: " << bytesRecv << " bytes/sec" << std::endl;
    }

    newFile.close();
    delete[] buffer;
    return true;
}

std::ofstream Server::createNewFile(std::string name) {
    /* убрал подставу в выходом из директории */
    while (true) {
        int pos = name.find('/');
        if (pos != std::string::npos) {
            name = name.substr(pos + 1, name.length());
        } else
            break;
    }

    name.insert(0, "uploads/");

    /* законтрил повторяющиеся имена файлов */
    if (std::filesystem::exists(name))
        name.insert(8, "(" + std::to_string(1) + ")");

    int i = 1;
    while (std::filesystem::exists(name)) {
        name.replace(9, 1, std::to_string(++i));
    }

    return std::ofstream(name);
}

void Server::sendResponse(descriptor_t clientSock, bool isSuccess) {
    int response = isSuccess ? FileReceivedSuccessfully : FileReceivedFail;
    send(clientSock, (char *) &response, sizeof(int), 0);
}
