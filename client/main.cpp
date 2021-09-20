#include <iostream>
#include "Client.h"

/*
 * Клиенту передаётся в параметрах относительный или абсолютный путь к файлу, который нужно отправить.
 * Длина имени файла не превышает 4096 байт в кодировке UTF-8. Размер файла не более 1 терабайта.
 *
 * Клиенту также передаётся в параметрах DNS-имя (или IP-адрес) и номер порта сервера.
 * Клиент передаёт серверу имя файла в кодировке UTF-8, размер файла и его содержимое.
 * Для передачи используется TCP. Протокол передачи придумайте сами
 * (т.е. программы разных студентов могут оказаться несовместимы).
 */

const int FileName = 1;
const int DestinationIpAddr = 2;
const int DestinationPort = 3;


int main(int argc, char** argv) {
    if (argc != 4) {
        std::cout << "You must enter <filename> <dest ip addr> <dest port>" << std::endl;
        return 0;
    }

    std::string fileName = argv[FileName];
    std::string destIp = argv[DestinationIpAddr];
    int destPort = 0;
    try {
        destPort = std::stoi(argv[DestinationPort]);
    } catch (std::invalid_argument &exception) {
        std::cout << "Port must be an Integer value" << std::endl;
        return 0;
    }

    try {
        Client client(fileName, destIp, destPort);
        client.sendFile();
    } catch (std::exception &exception) {
        std::cout << "fatal error :(" << std::endl;
    }

    return 0;
}
