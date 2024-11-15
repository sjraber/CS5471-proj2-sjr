#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>

#define MARKER 0xdeadbeef

//function to read file
std::vector<char> fileRead(const std::string &filename{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()){
        exit(1);
    }
    size_t fSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fSize);
    file.read(buffer.data(), fSize);
    return buffer;
}

//function to write to file

void writeToFile(const std::string &filename, const std::vector<char> &data){
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()){
        std::cout<<"file not open";
        exit(1);
    }
    file.write(data.data(), data.size());
}

int main(int argc, char *argv[]){

}