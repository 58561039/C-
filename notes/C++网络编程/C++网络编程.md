# 一、实战一个使用TCP的网络编程程序

```C++
#include <iostream>
#include <cstring>
#include <sys/socket.h> // 核心Socket API
#include <netinet/in.h> // sockaddr_in 结构体
#include <unistd.h>     // read, write, close

int main() {
    // 1. 创建Socket
    // AF_INET: IPv4, SOCK_STREAM: TCP, 0: 默认协议
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create socket.\n";
        return 1;
    }

    // 2. 绑定地址和端口
    sockaddr_in address;
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // 绑定到所有本地地址
    address.sin_port = htons(8080); // 将主机字节序的端口号转换为网络字节序

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed.\n";
        close(server_fd);
        return 1;
    }

    // 3. 开始监听
    // SOMAXCONN: 系统允许的最大挂起连接数
    if (listen(server_fd, SOMAXCONN) < 0) {
        std::cerr << "Listen failed.\n";
        close(server_fd);
        return 1;
    }

    std::cout << "Server is listening on port 8080..." << std::endl;

    // 4. 接受一个传入的连接
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_socket = accept(server_fd, (sockaddr*)&client_addr, &client_addr_len);
    if (client_socket < 0) {
        std::cerr << "Accept failed.\n";
        close(server_fd);
        return 1;
    }

    std::cout << "Client connected!" << std::endl;

    // 5. 与客户端通信：读取数据并回显
    char buffer[1024] = {0};
    int bytes_read = read(client_socket, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        std::cerr << "Read error.\n";
    } else if (bytes_read == 0) {
        std::cout << "Client disconnected.\n";
    } else {
        std::cout << "Received: " << buffer << std::endl;
        // 将收到的数据发回去
        send(client_socket, buffer, bytes_read, 0);
        std::cout << "Echoed back.\n";
    }

    // 6. 关闭连接
    close(client_socket);
    close(server_fd);

    return 0;
}
```



```C++
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>

int main() {
    // 1. 创建Socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket.\n";
        return 1;
    }

    // 2. 指定要连接的服务器地址
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080); // 服务器端口

    // 将字符串形式的IP地址("127.0.0.1")转换为网络格式
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported.\n";
        close(sock);
        return 1;
    }

    // 3. 连接到服务器
    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed.\n";
        close(sock);
        return 1;
    }

    std::cout << "Connected to server!" << std::endl;

    // 4. 发送消息
    const char* message = "Hello, Server!";
    send(sock, message, strlen(message), 0);
    std::cout << "Message sent: " << message << std::endl;

    // 5. 读取服务器的回复
    char buffer[1024] = {0};
    int bytes_read = read(sock, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        std::cerr << "Read error.\n";
    } else {
        std::cout << "Server echoed: " << buffer << std::endl;
    }

    // 6. 关闭连接
    close(sock);

    return 0;
}
```





#### 1.socket讲解

#include<sys/socket.h>

int socket(`int domain`,`int type`,`int protocal`)

失败返回-1，并修改关键字

**domain：协议族，指定地址的格式**

AF_INET：ipv4，对应常规的ip地址

AF_INET6：ipv6，对应ipv6的地址

AF_LOCAL：本地进程间通信，对应的是文件系统路径名作为地址

**type：套接字的类型**

SOCK_STREAM：TCP

SOCK_DGRAM：UDP

protocol：协议



#### 2.sockaddr_in结构体

```C
struct sockaddr_in {
    sa_family_t    sin_family;   // 地址族 (Address family)
    in_port_t      sin_port;     // 端口号 (Port number)
    struct in_addr sin_addr;     // IPv4地址 (IP address)
    char           sin_zero[8];  // 填充字节，未使用
};

// in_addr 结构体通常定义如下：
struct in_addr {
    uint32_t s_addr; // 32位的IPv4地址（网络字节序）
};
```

定义之后，指定地址族，端口号，ip地址

端口号和IP地址都是网络字节序，也就是大端序

TCP编程中，必须将端口号和IP地址设置为网络字节序，因为不同的操作系统中数据存储的格式不一样，有的是大端序，有的是小端序，为了防止在不同操作系统中运行出现问题，必须这么设置

TCP协议中，还有其他字段都是网络字节序，但是对于程序员是不可见，一般都有操作系统处理，除非自己编写协议，直接对底层数据操作



#### 3.bind

int bind(`int sockfd`, `const struct sockaddr *addr`, `socklen_t addrlen`);

成功返回0，失败返回-1

将套接字绑定到本地IP和端口





#### 4.listen监听

int listen(`int sockfd`, `int backlog`);

参数：套接字和半连接队列的最大长度

成功返回0，失败返回-1

只适用于TCP协议

一个套接字调用`listen`后，会变成被动套接字，只能被动的接受`connect`连接

调用`listen`后，操作系统会为该套接字生成两个队列

半连接队列：正处于三次握手过程中的连接

全连接队列：已经完成三次握手，等待底层调用`accept`的连接

从半连接队列出队→进入全连接队列



#### 5.accept

int accept(`int sockfd`, `struct sockaddr *addr`, `socklen_t *addrlen`);

成功返回套接字，失败返回-1

从全连接队列中取出一个连接，返回一个套接字用来通信，结构体保存客户端的端口，IP地址

默认是阻塞模式，但是可以设置为非阻塞

方法1：将创建套接字时，将套接字设置为非阻塞

方法2：如果已经创建过套接字，用`fcntl`函数将已有的套接字设置为非阻塞

一般高并发服务器都会将套接字设置为非阻塞，配合`select/poll/epoll`使用

必须配合`listen`使用，只会从全连接队列中取连接



#### 6.read

ssize_t read(`int fd`, `void *buf`, `size_t count`);

成功返回读到的字节数（>0），返回0代表关闭连接，-1代表失败

从缓冲区中读数据



#### 7.send

ssize_t send(`int sockfd`, `const void *buf`, `size_t len`, `int flags`);

参数flag：

默认是0，结果和write一样

MSG_DONTWAIT：非阻塞发送

MSG_OOB：发送 带外数据（TCP 特性）

MSG_NOSIGNAL：防止发送 SIGPIPE 信号

成功返回写入的字节数，返回0代表关闭连接，-1代表失败

`send`本质是对`write`的封装，多一个`flag`的参数，专用于套接字

而`write`可用于套接字，管道，文件





#### 8.connect

int connect(`int sockfd`, `const struct sockaddr *addr`, `socklen_t addrlen`);

成功返回0，失败返回-1

默认是阻塞，可以设置套接字为非阻塞

对于TCP，建立连接；对于UDP，可以设置默认地址，后面send就不需要再标注地址



#### 9.为什么IP地址的转换是inet_pton，而端口号的转换是htons？

因为IP地址一般指定是点分十进制，例如"127.0.0.1"，需要转化为32位二进制整数，再转换为网络字节序

端口号只需要转换为网络字节序



# 二、I/O模型

### 1.阻塞模型

最简单，效率最低

一般一个线程处理一个连接，要想处理多个只能使用多进程或者多线程，非常消耗资源

一个线程监听一个socket文件描述符，处于阻塞状态等待事件的发生

### 2.select

#### 知识点

int select(`int nfds`,`fd_set *readfds`,`fd_set *writefds`,`fd_set *exceptfds`,`struct timeval *timeout`);

nfds：要检测的文件描述符的最大值+1，表示要检测的范围是从0到ndfs

readfds：fd_set集合，用来监测哪些文件描述符可读

write_fds：用于监测哪些文件描述符可写

exceptfds：用于监测哪些文件描述符发生了异常事件

timeval：设置等待的时间，如果是NULL，就一直等待，直到有事件发生，如果是0，就立即返回（非阻塞），如果是指定值，就等待指定值的事件，如果超时就返回0

返回值：>0：有事件发生，返回就绪的文件描述符个数

=0：超时，没有事件发生

<0：出错，返回-1

内部实现是以数组实现，得到请求以后需要程序轮询，检查是哪个文件描述符出现新的事件，复杂度O(n)

由于内部实现是数组，所以有默认的数量限制，默认是1024，但012三个套接字分别是标准输入、标准输出和标准错误输出，所以默认最大是1021，不过默认值可以修改

```C
#define FD_SETSIZE 2048
#include<sys/select.h>//必须在include之前define
```

监听新连接的套接字需要放到可读集合中

在select中需要三个集合，读集合，写集合，异常集合

设置好三个集合，调用select，当返回值>0，表明有几个事件发生，这时需要轮询，通过函数`FD_ISSET`函数判断，使用方式为`FD_ISSET(server_fd, &readfds)`，看是否是该集合中的该文件描述符出现了请求。

通过使用select，可以在一个线程中监听多个文件描述符，判断它们是否需要执行IO操作，读写，或者是异常，最重要的是一个线程可以监听多个文件描述符，防止出现一个线程阻塞等待一个文件描述符

#### 使用说明及代码

用于监听新连接的套接字需要放到read集合中

当监测到事件发生后，需要先判断是否是监听到新连接，监听到新连接需要将返回的套接字放到定义好的套接字数组中

然后再判断是否是已有的连接中发送的新消息，使用轮询的方式，读取对应套接字的消息，如果读取的字节是0字节，说明该套接字断开连接

```C++
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
```

### 3.poll

#### 知识点

int poll(`struct pollfd fds[]`,` nfds_t nfds`, `int timeout`);

fds：一个pollfd格式的数组指针，如果使用vector存储所有的文件描述符，那么可以调用vector的data函数返回指针，因为poll函数是C风格的函数，不认识vector

nfds：文件描述符的最大值+1，但可以直接调用vector的size函数

timeout：时间，单位是毫秒，可以是-1，表示在没有监测到事件时一直阻塞；可以是0，表示如果没有监测到事件立刻返回；可以是具体的数字，表示在规定时间内监测，到时间就返回。

返回值是监测到多少个事件，仍需要轮询检查是哪个文件描述符有事件

内部是链表实现，所以没有数量的限制。

与select相比，select是把所有的文件描述符放到读或写或异常数组中监测，理论上来说需要三个数组

而poll是将文件描述符封装为一个结构体pollfd，该结构体的第一个元素是fd，即文件描述符；

第二个元素是events，即事件，可以是POLLIN（读），可以是POLLOUT（写），可以是POLLERR（异常），POLLHUP（对端关闭/挂起），POLLNVAL（文件描述符无效）。注意，可以选择多个，这里的关键字是位，类似于01010，即每个事件都有对应的位。需要程序员设置好

第三个元素是revents，不需要程序员设置，在创建的时候，可以默认选择0。这样当出现事件的时候，通过revents检查，`fds[i].revents & POLLIN`

服务器用来监听的文件描述符需要设置events为POLLIN

#### 使用说明及代码

与select不同的是，文件描述符封装为pollfd结构体，用vector存储。select也可以用vector存储，但是poll程序中不能用C风格数组存储，虽然硬要用也能用，但是用这种数组需要考虑超过数组大小，需要扩张的问题，而vector帮我们这么做了，不需要自己再考虑

select是用FD_ISSET轮询哪一个文件描述符有事件，poll是通过revents&POLLIN，这样相与检查是否有事件

select在与客户端断开连接时，需要将记录文件描述符的数组元素置0

poll在与客户端断开连接时，需要将vector数组中记录改文件描述符的元素删除，索引-1。

```C++
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
```

### 4.epoll

#### 知识点

int epoll_create1(`int flags`);

创建一个epoll实例，返回一个文件描述符，该文件描述符用来监听所有需要被监听的文件描述符

失败返回-1，创建实例成功返回一个正数，表示一个文件描述符

flags：默认是0



int epoll_ctl(`int epfd`, `int op`, `int fd`, `struct epoll_event *event`);

控制 epoll 对哪些 fd 进行监听

epfd：epoll_create1创建的epoll实例

op：操作类型

fd：要操作的socket或者文件描述符

event：监听的事件类型，EPOLLIN（读），EPOLLOUT（写），EPOOLLET（边缘触发）

成功返回0，失败返回-1



int epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout);

等待事件发生，将发生事件的fd列表放在events中，maxenects表示该数组的大小

返回值：

>0：返回就绪的文件描述符数量

=0：表示超时，没有事件发生

<0：出错





#### 使用说明及代码

```C++
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
```
