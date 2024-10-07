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
const char* addr6 = "::";
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

int open_and_bind_v6(int port, int type){
    int socketDesc;
    int opt = 1;
    struct sockaddr_in6 server;
    std::string message;
    socketDesc = socket(AF_INET6, type, 0);
    if(socketDesc == -1){
            //std::cout << "ERROR CREATING SOCKET DESCRIPTOR\n";
            return -1;
    }

    if(setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
            //std::cout << "Setsocket error\n";
            return -1;
    }
    server.sin6_family = AF_INET6;
    inet_pton(AF_INET6, addr6, &server.sin6_addr);
    server.sin6_port = htons(port);

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

using open_bind_fn_t = decltype(open_and_bind);

std::vector<int> bind_all(int maxport, open_bind_fn_t* fn, int type, std::string pname, bool report_fail = false, bool report_success = true, int minport = 0) {
    std::vector<int> result;
    for(int i = minport; i <= maxport; i++) {
        int c = open_and_bind(i, SOCK_STREAM);
        if(c == -1){
            if(report_fail)
                std::cout << "port " << i << " failed to bind " << pname << "\n";
            continue;
        }
            
        result.push_back(i);
        if(report_success)
            std::cout << "port " << i << "bounded " << pname << "\n";
    }
}
int main(int argc, char** argv) {
    int n_ports = 65536;
    if(argc > 1){
        n_ports = atoi(argv[1]);
    }
    

    auto tcpv4 = bind_all(n_ports - 1, open_and_bind, SOCK_STREAM, "tcpv4");
    auto udpv4 = bind_all(n_ports - 1, open_and_bind, SOCK_STREAM, "udpv4");
    auto tcpv6 = bind_all(n_ports - 1, open_and_bind, SOCK_STREAM, "tcpv4");
    auto udpv6 = bind_all(n_ports - 1, open_and_bind, SOCK_STREAM, "udpv4");
    int rc = system("netstat -tulnp");
    while(1) {sleep(10);}
    //return !(fd && !rc);
    return 0;
}