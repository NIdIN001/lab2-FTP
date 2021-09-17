#include "FileManager.h"

FileManager::FileManager(std::string &fileName_) {
    file.open(fileName_, std::ios::in);
    buffer = new char[BufferSize];

    if (!file.is_open()) {
        std::cout << "can't open file " << fileName_ << std::endl;
        throw std::exception();
    }

    fileName = fileName_;
    fileLength = calculateLength();
}

FileManager::~FileManager() {
    delete[] buffer;
    file.close();
}

long FileManager::getLength() const {
    return fileLength;
}

long FileManager::calculateLength() {
    file.seekg(0, std::ios::end);
    fileLength = file.tellg();
    file.seekg(0, std::ios::beg);

    return fileLength;
}

char* FileManager::getNext() {
    memset(buffer, 0, BufferSize);
    file.read(buffer, BufferSize);

    return buffer;
}

std::string FileManager::getFileName() {
    return fileName;
}
