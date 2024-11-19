#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>

int main(){
    system("rm seed");
    system("rm victim1");
    system("rm victim2");
    system("rm victim3");
    system("g++ -o virus virus.cpp -w");
    system("cp virus seed");
    system("printf '/xde/xad/xbe/xef' >> seed");
    system("cat host >> seed");
    system("ls -l virus");
    system("ls -l host");
    system("ls -l seed");
    system("od -j 0 -t x1 seed -N 4");
    system("cp /bin/ls ./victim1");
    system("cp /usr/bin/wc ./victim2");
    system("cp /usr/bin/file ./victim3");
    system("chmod ugo-x ./victim1");
    system("chmod ugo-x ./victim2");
    system("chmod ugo-x ./victim3");
}