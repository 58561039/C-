#include<iostream>
#include<cstring>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 30

int main(){
    int server_fd;
    int client_fd[MAX_CLIENTS];
    int new_socket;
    sockaddr_in address;
    int add_len=sizeof(address);

    for(int i=0;i<MAX_CLIENTS;i++){
        client_fd[i]=0;
    }

    if((server_fd=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("Fail to create server_fd");
        return 0;
    }
    
    address.sin_family=AF_INET;
    address.sin_port=htons(PORT);
    address.sin_addr.s_addr=INADDR_ANY;
    
    if(bind(server_fd,(sockaddr*)&address,sizeof(address))<0){
        perror("Fail to bind");
        return 0;
    }

    if(listen(server_fd,5)<0){
        perror("Fail to listen");
        return 0;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    fd_set readfds;

    while(1){
        FD_ZERO(&readfds);
        FD_SET(server_fd,&readfds);

        int max_fd=server_fd;
        for(int i=0;i<MAX_CLIENTS;i++){
            int sd=client_fd[i];
            if(sd>0){
                FD_SET(sd,&readfds);
            }
            if(sd>max_fd){
                max_fd=sd;
            }
        }

        int activity=select(max_fd+1,&readfds,NULL,NULL,NULL);
        if(activity<0){
            perror("Fail to select");
            return 0;
        }

        if(FD_ISSET(server_fd,&readfds)){
            if((new_socket=accept(server_fd,(sockaddr*)&address,(socklen_t*)&add_len))<0){
                perror("Fail to accept");
                return 0;
            }
            std::cout << "New connection, socket fd: " << new_socket
                      << " , ip: " << inet_ntoa(address.sin_addr)
                      << " , port: " << ntohs(address.sin_port) << "\n";
            
            for(int i=0;i<MAX_CLIENTS;i++){
                if(client_fd[i]==0){
                    client_fd[i]=new_socket;
                    break;
                }
            }
        }

        char buffer[1024];
        for(int i=0;i<MAX_CLIENTS;i++){
            int sd=client_fd[i];
            if(FD_ISSET(sd,&readfds)){
                int sizeread=read(sd,buffer,sizeof(buffer)-1);
                if(sizeread==0){
                    getpeername(sd,(sockaddr*)&address,(socklen_t*)&add_len);
                    std::cout << "Host disconnected , ip: "
                              << inet_ntoa(address.sin_addr)
                              << " , port: " << ntohs(address.sin_port)
                              << "\n";
                    close(sd);
                    client_fd[i]=0;
                }
                else{
                    buffer[sizeread]='\0';
                    std::cout << "Message from client(" << sd << "): " << buffer;
                    send(sd,buffer,sizeread,0);
                }
            }
        }
    }
}
