#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <csignal>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;
const char* SERVER_IP = "127.0.0.1";
int client_socket;  // 全局变量，用于信号处理函数关闭套接字

// 信号处理函数，用于优雅退出
void handle_signal(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n收到中断信号，正在退出..." << std::endl;
        if (client_socket > 0) {
            close(client_socket);
        }
        exit(EXIT_SUCCESS);
    }
}

// 显示帮助信息
void show_help() {
    std::cout << "\n使用说明:" << std::endl;
    std::cout << "  输入消息并按回车发送给服务器" << std::endl;
    std::cout << "  输入 \"/exit\" 退出程序" << std::endl;
    std::cout << "  输入 \"/help\" 显示此帮助信息" << std::endl;
}

int main() {
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    
    // 注册信号处理函数，捕获Ctrl+C
    signal(SIGINT, handle_signal);
    
    // 创建套接字
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "创建套接字失败" << std::endl;
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // 转换IP地址
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "无效的IP地址" << std::endl;
        close(client_socket);
        return -1;
    }
    
    // 连接到服务器
    if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "连接服务器失败" << std::endl;
        close(client_socket);
        return -1;
    }
    
    std::cout << "成功连接到服务器 " << SERVER_IP << ":" << PORT << std::endl;
    show_help();
    
    // 主循环：持续发送和接收消息
    while (true) {
        std::string message;
        
        // 提示用户输入
        std::cout << "\n请输入消息: ";
        std::getline(std::cin, message);
        
        // 检查退出命令
        if (message == "/exit") {
            std::cout << "正在断开连接..." << std::endl;
            break;
        }
        
        // 显示帮助信息
        if (message == "/help") {
            show_help();
            continue;
        }
        
        // 发送消息到服务器
        ssize_t send_len = send(client_socket, message.c_str(), message.length(), 0);
        if (send_len < 0) {
            std::cerr << "发送消息失败" << std::endl;
            break;
        }
        
        // 接收服务器响应
        ssize_t recv_len = read(client_socket, buffer, BUFFER_SIZE - 1);
        if (recv_len < 0) {
            std::cerr << "接收响应失败" << std::endl;
            break;
        } else if (recv_len == 0) {
            std::cerr << "服务器已断开连接" << std::endl;
            break;
        }
        
        // 处理接收到的消息
        buffer[recv_len] = '\0';  // 添加字符串结束符
        std::cout << "服务器响应: " << buffer << std::endl;
        
        // 清空缓冲区
        memset(buffer, 0, BUFFER_SIZE);
    }
    
    // 关闭套接字
    close(client_socket);
    std::cout << "已成功断开连接" << std::endl;
    
    return 0;
}

