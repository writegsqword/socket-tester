#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <cstdlib>




const char* addr = "0.0.0.0";

//credits: https://stackoverflow.com/questions/58487981/why-isnt-port-listening-in-c
int open_and_bind(int port, int type){
    int socketDesc;
    int opt = 1;
    struct sockaddr_in server;
    std::string message;
    socketDesc = socket(AF_INET, type, 0);
    if(socketDesc == -1){
            //std::cout << "ERROR CREATING SOCKET DESCRIPTOR\n";
            return -1;
    }

    if(setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
            //std::cout << "Setsocket error\n";
            return -1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(addr);
    server.sin_port = htons(port);

    if(bind(socketDesc, (struct sockaddr *)&server, sizeof(server)) < 0){
            //std::cout << "BIND FAILED\n";
            return -1;
    }
    //std::cout << "Bind finished\n";

    if(listen(socketDesc,10) < 0){
        close(socketDesc);
    }

    return socketDesc;
}


int main() {
    std::vector<int> sock_fds;
    for(int i = 0; i < 65536; i++) {
        int c = open_and_bind(i, SOCK_STREAM);
        if(c == -1){
            std::cout << "port " << i << " failed to bind(tcp)\n";
        }
        sock_fds.push_back(c);
    }
    for(int i = 0; i < 65536; i++) {
        int c = open_and_bind(i, SOCK_DGRAM);
        if(c == -1){
            std::cout << "port " << i << " failed to bind(udp)\n";
        }
        sock_fds.push_back(c);
    }
    int rc = system("netstat -tlnp");
    while(1) {sleep(10);}
    //return !(fd && !rc);
    return 0;
}