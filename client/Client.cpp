#include "Client.h"

Client::Client(std::string &filePath_, std::string &destIpAddr_, int destPort_) : file(filePath_) {
    destPort = destPort_;
    destIpAddr = destIpAddr_;

    srcPort = SrcPort;
    tcpSocket = createTCPSocket();
}

Client::~Client() {
    closeSocket();
}

void Client::closeSocket() {
    close(tcpSocket);
}

descriptor_t Client::createTCPSocket() {
    descriptor_t s;
    sockaddr_in ssin;

    memset(&ssin, 0, sizeof(ssin));
    ssin.sin_family = AF_INET;
    ssin.sin_port = htons(destPort);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, destIpAddr.c_str(), &ssin.sin_addr)<=0) {
        std::cout << "bad destination IP address" << std::endl;
        throw std::exception();
    }

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cout << "socket error" << std::endl;
        throw std::exception();
    }

    /* make a connection */
    if (connect(s, (const struct sockaddr *) &ssin, sizeof(ssin)) == -1) {
        std::cout << "connect error" << std::endl;
        throw std::exception();
    }

    return s;
}

void Client::sendFile() {
    /* отправил длину названия файла */
    unsigned int fileNameLength = file.getFileName().length();
    send(tcpSocket, (char *) &fileNameLength, sizeof(unsigned int), 0);

    /* отправил имя файла */
    send(tcpSocket, file.getFileName().c_str(), file.getFileName().length(), 0);

    /* отправил длину файла */
    unsigned long fileLength = file.getLength();
    send(tcpSocket, (char *) &fileLength, sizeof(unsigned long), 0);

    /* отправил файл */
    long bytesSent = 0;
    long toSendLength = 0;
    int result = 0;

    while (bytesSent < file.getLength()) {
        char *toSend = file.getNext();
        toSendLength = (file.getLength() - bytesSent > BufferSize) ? BufferSize : file.getLength() - bytesSent;

        result = send(tcpSocket, toSend, toSendLength, 0);
        switch (result) {
            case (-1): {
                std::cout << "Send error" << std::endl;
                throw std::exception();
            }
            case (0): {
                std::cout << "Server close connection" << std::endl;
                return;
            }
            default: {
                bytesSent += result;
            }

        }
    }

    recvAnswer();
}

void Client::recvAnswer() const {
    int answer;
    recv(tcpSocket, &answer, sizeof(int), 0);

    if (answer == FileReceivedSuccessfully) {
        std::cout << "Success!" << std::endl;
    } else {
        std::cout << "Fail!" << std::endl;
    }
}
