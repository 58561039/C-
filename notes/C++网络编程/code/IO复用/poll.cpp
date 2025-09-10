#include<iostream>
#include<netinet/in.h>
#include<unistd.h>
#include<poll.h>
#include<sys/socket.h>
#include<cstring>
#include<vector>
#include<arpa/inet.h>

#define PORT 8080
#define MAX_CLIENT 30

int main(){
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0){
        perror("Fail to create server_fd");
        return 0;
    }

    sockaddr_in address;
    address.sin_family=AF_INET;
    address.sin_port=htons(PORT);
    address.sin_addr.s_addr=INADDR_ANY;
    socklen_t add_len=sizeof(address);

    if(bind(server_fd,(sockaddr*)&address,add_len)<0){
        perror("Fail to bind");
        return 0;
    }

    if(listen(server_fd,MAX_CLIENT)<0){
        perror("Fail to listen");
        return 0;
    }

    std::cout << "Server listening on port " << PORT << "\n";

    std::vector<pollfd> fds;

    fds.emplace_back([](int fd,short ev){return pollfd{fd,ev,0};}(server_fd,POLLIN));

    while(1){
        int activity=poll(fds.data(),fds.size(),-1);
        if(activity<0){
            perror("Fail to poll");
            return 0;
        }

        for(size_t i=0;i<fds.size();i++){
            if(fds[i].revents&POLLIN){
                if(fds[i].fd==server_fd){
                    sockaddr_in client_add;
                    socklen_t client_add_len=sizeof(client_add);
                    int newsock=accept(fds[i].fd,(sockaddr*)&client_add,&client_add_len);
                    if(newsock<0){
                        perror("Fail to accept");
                        return 0;
                    }
                    std::cout << "New connection fd=" << newsock
                              << " ip=" << inet_ntoa(client_add.sin_addr)
                              << " port=" << ntohs(client_add.sin_port) << "\n";
                    fds.emplace_back([](int fd,short ev){return pollfd{fd,ev,0};}(newsock,POLLIN));
                }
                else{
                    char buffer[1024];
                    int read_bytes=read(fds[i].fd,buffer,sizeof(buffer)-1);
                    if(read_bytes<=0){
                        std::cout << "Client disconnected fd=" << fds[i].fd << "\n";
                        close(fds[i].fd);
                        fds.erase(fds.begin()+i);
                        i--;
                    }
                    else{
                        buffer[read_bytes] = '\0';
                        std::cout << "Message from fd=" << fds[i].fd << ": " << buffer;
                        send(fds[i].fd, buffer, read_bytes, 0);
                    }
                }
            }
        }
    }
    return 0;
}
