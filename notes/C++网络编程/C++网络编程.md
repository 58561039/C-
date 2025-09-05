
# 一、实战一个网络编程程序

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

int socket(int domain,int type,int protocal)

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



#### 3.listen监听

只适用于`TCP`协议

一个套接字调用`listen`后，会变成被动套接字，只能被动的接受`connect`连接

调用`listen`后，操作系统会为该套接字生成两个队列

半连接队列：正处于三次握手过程中的连接

全连接队列：已经完成三次握手，等待底层调用`accept`的连接

从半连接队列出队→进入全连接队列



#### 4.accept

从全连接队列中取出一个连接，

返回一个套接字用来通信，结构体保存客户端的端口，IP地址

