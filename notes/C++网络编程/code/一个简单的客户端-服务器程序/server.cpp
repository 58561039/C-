#include<iostream>
#include<cstring>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>

//server

int main(){
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0){
        std::cerr<<"Fail to create socket\n";
        return 0;
    }

    sockaddr_in address;
    address.sin_family=AF_INET;
    address.sin_port=htons(8080);
    address.sin_addr.s_addr=INADDR_ANY;

    if(bind(server_fd,(sockaddr*)&address,sizeof(address))){
        std::cerr<<"Fail to bind\n";
        close(server_fd);
        return 0;
    }

    if(listen(server_fd,SOMAXCONN)<0){
        std::cerr<<"Fail to listen\n";
        close(server_fd);
        return 0;
    }

    sockaddr_in client_read;
    socklen_t client_read_len=sizeof(client_read);
    int client_read_socket=accept(server_fd,(sockaddr*)&client_read,&client_read_len);
    if(client_read_socket<0){
        std::cerr<<"Fail to accpet";
        close(server_fd);
        return 0;
    }

    char buffer[1024]={0};
    int bytes_read=read(client_read_socket,buffer,sizeof(buffer));
    if (bytes_read < 0) {
        std::cerr << "Read error.\n";
    } else if (bytes_read == 0) {
        std::cout << "Client disconnected.\n";
    } else {
        std::cout << "Received: " << buffer << std::endl;
        send(client_read_socket, buffer, bytes_read, 0);
        std::cout << "Echoed back.\n";
    }

    close(client_read_socket);
    close(server_fd);

    
}
