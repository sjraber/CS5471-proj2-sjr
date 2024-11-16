#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>

int main(){
    system("g++ -o host host.cpp");
    system("g++ -o virus virus.cpp");

    system("cp host seed");
    system("printf '\xde\xad\xbe\xef' >> seed");
    system("cat virus >> seed");

    system("ls -l virus");
    system("ls -l host");
    system("ls -l seed");

    system("cp /bin/ls ./victim1");
    system("cp /usr/bin/wc ./victim2");
    system("cp /usr/bin/file ./victim3");

    system("chmod 777 victim1");
    system("chmod 777 victim2");
    system("chmod 777 victim3");
    return 0;

}