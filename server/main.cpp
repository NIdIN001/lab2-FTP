#include <iostream>

#include "Server.h"

/*
 * Серверу передаётся в параметрах номер порта,
 * на котором он будет ждать входящих соединений от клиентов.
 */

const int DestinationPort = 1;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "You must enter <src port>" << std::endl;
        return 0;
    }

    int srcPort = 0;
    try {
        srcPort = std::stoi(argv[DestinationPort]);
    } catch (std::invalid_argument &exception) {
        std::cout << "Port must be an Integer value" << std::endl;
        return 0;
    }

    try {
        Server server(srcPort);
        server.run();
    } catch (std::exception &exception) {
        std::cout << "fatal error :(" << std::endl;
    }

    return 0;
}
