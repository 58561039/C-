#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<iostream>
#include<cstring>
#include<arpa/inet.h>

int main(){
    int send_fd=socket(AF_INET,SOCK_STREAM,0);
    if(send_fd<0){
        std::cerr<<"Fail to create socket\n";
        return 0;
    }

    sockaddr_in address;
    address.sin_family=AF_INET;
    address.sin_port=htons(8080);
    
    if(inet_pton(AF_INET,"127.0.0.1",&address.sin_addr)<=0){
        std::cerr << "Invalid address / Address not supported.\n";
        close(send_fd);
        return 0;
    }

    if(connect(send_fd,(sockaddr*)&address,sizeof(address))<0){
        std::cerr<<"Fail to connect\n";
        close(send_fd);
        return 0;
    }
    std::cout << "Connected to server!" << std::endl;
    const char *Message="hello server,i am a sender";
    send(send_fd,Message,strlen(Message),0);
    std::cout << "Message sent: " << Message << std::endl;

    char *buffer={0};
    int bytes_read=read(send_fd,buffer,sizeof(buffer));
    if (bytes_read < 0) {
        std::cerr << "Read error.\n";
    } else {
        std::cout << "Server echoed: " << buffer << std::endl;
    }

    close(send_fd);
}
