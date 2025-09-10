#include<iostream>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<cstring>
#include<sys/epoll.h>

#define PORT 8080
#define MAX_CLIENTS 30

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

    if(bind(server_fd,(sockaddr*)&address,sizeof(address))<0){
        perror("Fail to bind");
        return 0;
    }

    if(listen(server_fd,MAX_CLIENTS)<0){
        perror("Fail to listen");
        return 0;
    }

    int epfd=epoll_create1(0);
    if(epfd<0){
        perror("Fail to epoll_create1");
        return 0;
    }

    epoll_event event{};
    event.events=EPOLLIN;
    event.data.fd=server_fd;
    if(epoll_ctl(epfd,EPOLL_CTL_ADD,server_fd,&event)<0){
        perror("Fail to epoll_ctl server_fd");
        return 0;
    }

    epoll_event events[MAX_CLIENTS];

    std::cout << "Server listening on port " << PORT << "...\n";

    while(1){
        int n=epoll_wait(epfd,events,MAX_CLIENTS,-1);
        if(n<0){perror("Fail to epoll_wait");continue;}

        for(int i=0;i<n;i++){
            int fd=events[i].data.fd;
            
            if(fd==server_fd){
                sockaddr_in client_add;
                socklen_t client_add_size=sizeof(client_add);

                int client_fd=accept(server_fd,(sockaddr*)&client_add,&client_add_size);
                if(client_fd<0){perror("Fail to accpet");continue;}

                std::cout << "New connection fd=" << client_fd
                          << " ip=" << inet_ntoa(client_add.sin_addr)
                          << " port=" << ntohs(client_add.sin_port) << "\n";

                epoll_event client_event{};
                client_event.events=EPOLLIN;
                client_event.data.fd=client_fd;
                
                if(epoll_ctl(epfd,EPOLL_CTL_ADD,client_fd,&client_event)<0){
                    perror("Fail to epoll_ctl client");
                    continue;
                }
            }
            else{
                char buffer[1024];
                int read_size=read(fd,buffer,sizeof(buffer)-1);
                if(read_size<=0){
                    std::cout << "Client disconnected fd=" << fd << "\n";
                    close(fd);
                    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,nullptr);
                }
                else{
                    buffer[read_size]='\0';
                    std::cout<<"Message from fd="<<fd<<buffer<<std::endl;
                    send(fd,buffer,read_size,0);
                }
            }
        }
    }
    close(server_fd);
    close(epfd);
    return 0;
}
