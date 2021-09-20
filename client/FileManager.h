#ifndef LAB2_FILEMANAGER_H
#define LAB2_FILEMANAGER_H

#include <string>
#include <fstream>
#include <iostream>

const int BufferSize = 1024 * 4;

class FileManager {
    std::fstream file;
    std::string fileName;
    long fileLength;

    char *buffer;
public:
    explicit FileManager(std::string &fileName);

    ~FileManager();

    long getLength() const;

    std::string getFileName();

    char *getNext();

private:
    long calculateLength();
};


#endif //LAB2_FILEMANAGER_H
