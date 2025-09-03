#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  // 添加这个头文件以使用inet_ntoa
#include <unistd.h>
#include <string>
#include <csignal>
#include <pthread.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;
int server_fd;  // 服务器套接字文件描述符，用于信号处理

// 信号处理函数，用于优雅关闭服务器
void handle_shutdown(int signal) {
    std::cout << "\n收到关闭信号，正在停止服务器..." << std::endl;
    if (server_fd > 0) {
        close(server_fd);
    }
    exit(EXIT_SUCCESS);
}

// 客户端处理函数，由线程执行
void* handle_client(void* client_socket_ptr) {
    int client_socket = *((int*)client_socket_ptr);
    delete (int*)client_socket_ptr;  // 释放动态分配的内存
    
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // 获取客户端信息
    getpeername(client_socket, (struct sockaddr*)&client_addr, &client_len);
    // 使用inet_ntoa将网络地址转换为字符串
    std::string client_ip = inet_ntoa(client_addr.sin_addr);
    int client_port = ntohs(client_addr.sin_port);
    
    std::cout << "客户端 " << client_ip << ":" << client_port << " 已连接" << std::endl;
    
    // 持续处理客户端消息
    while (true) {
        // 清空缓冲区
        memset(buffer, 0, BUFFER_SIZE);
        
        // 读取客户端消息
        ssize_t recv_len = read(client_socket, buffer, BUFFER_SIZE - 1);
        if (recv_len < 0) {
            std::cerr << "读取客户端 " << client_ip << ":" << client_port << " 数据错误" << std::endl;
            break;
        } else if (recv_len == 0) {
            std::cout << "客户端 " << client_ip << ":" << client_port << " 主动断开连接" << std::endl;
            break;
        }
        
        // 输出收到的消息
        std::cout << "收到来自 " << client_ip << ":" << client_port << " 的消息: " << buffer << std::endl;
        
        // 检查退出命令
        std::string message(buffer);
        if (message == "/exit") {
            std::string response = "已收到退出命令，即将断开连接";
            send(client_socket, response.c_str(), response.length(), 0);
            std::cout << "客户端 " << client_ip << ":" << client_port << " 请求退出" << std::endl;
            break;
        }
        
        // 构建响应消息
        std::string response = "服务器已收到: " + message;
        if (response.length() >= BUFFER_SIZE) {
            response = response.substr(0, BUFFER_SIZE - 1);
        }
        
        // 发送响应
        ssize_t send_len = send(client_socket, response.c_str(), response.length(), 0);
        if (send_len < 0) {
            std::cerr << "向客户端 " << client_ip << ":" << client_port << " 发送响应失败" << std::endl;
            break;
        }
    }
    
    // 关闭客户端连接
    close(client_socket);
    std::cout << "客户端 " << client_ip << ":" << client_port << " 连接已关闭" << std::endl;
    pthread_exit(nullptr);
    return nullptr;
}

int main() {
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // 注册信号处理函数，捕获Ctrl+C
    signal(SIGINT, handle_shutdown);
    
    // 创建服务器套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("创建套接字失败");
        exit(EXIT_FAILURE);
    }
    
    // 设置套接字选项，允许重用端口和地址
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("设置套接字选项失败");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // 监听所有可用网络接口
    address.sin_port = htons(PORT);
    
    // 绑定套接字到指定端口
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("绑定端口失败");
        exit(EXIT_FAILURE);
    }
    
    // 开始监听连接请求，最大等待队列长度为5
    if (listen(server_fd, 5) < 0) {
        perror("监听失败");
        exit(EXIT_FAILURE);
    }
    
    std::cout << "服务器已启动，监听端口 " << PORT << "，按Ctrl+C停止服务器" << std::endl;
    
    // 循环接受客户端连接
    while (true) {
        int* client_socket = new int;  // 动态分配，用于传递给线程
        
        // 接受客户端连接
        if ((*client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("接受连接失败");
            delete client_socket;
            continue;
        }
        
        // 创建线程处理客户端
        pthread_t thread_id;
        if (pthread_create(&thread_id, nullptr, handle_client, client_socket) != 0) {
            perror("创建线程失败");
            close(*client_socket);
            delete client_socket;
            continue;
        }
        
        // 分离线程，自动回收资源
        pthread_detach(thread_id);
    }
    
    return 0;
}
    
