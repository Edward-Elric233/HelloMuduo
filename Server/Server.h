// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2021/11/23
// Description: 
#ifndef HELLOMUDUO_SERVER_H
#define HELLOMUDUO_SERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <functional>
#include <string>
#include "unistd.h"
#include <iostream>

/*!
 * 1. 组合TcpServer对象
 * 2. 委托一个EventLoop时间循环对象
 * 3. 明确TcpServer构造函数需要的参数
 * 4. 绑定TcpServer所需要处理连接和读写事件的回调函数
 * 5. 设置合适的服务端线程数量，muduo库会自己分配IO线程和工作线程
 */
class Server {
    using EventLoop = muduo::net::EventLoop;
    using TcpServer = muduo::net::TcpServer;
    using InetAddress = muduo::net::InetAddress;
    using Buffer = muduo::net::Buffer;
    using string = std::string;
    using TcpConnectionPtr = muduo::net::TcpConnectionPtr;
    TcpServer server;  // #1
    EventLoop *loop;    // #2
public: /*! * ChatServer构造函数 * @param loop_ 事件循环
     * @param listenAddr IP+port
     * @param nameArg 服务器的名称
     */
    Server(EventLoop *loop_ = new EventLoop,
           const InetAddress &listenAddr = InetAddress("0.0.0.0", 8888),
           const string &nameArg = "Server")
            : server(loop_, listenAddr, nameArg)
            , loop(loop_) {
        //给服务器注册用户连接的创建和断开回调
        server.setConnectionCallback(std::bind(&Server::onConnection, this, std::placeholders::_1));
        //给服务器注册用户读写事件回调
        server.setMessageCallback(std::bind(&Server::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        //设置服务器端的线程数量，我的电脑上是1个8核的CPU，1个IO线程，7个工作线程
        server.setThreadNum(sysconf(_SC_NPROCESSORS_ONLN));
    }

    /*!
     * 开启事件循环
     */
    void start() {
        server.start();  // epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, events);
        loop->loop();    // epoll_wait以阻塞方式等待新用户连接
    }

protected:
    /*!
     * 专门用于处理用户连接的回调函数，在发现listenfd上有新的连接请求后，调用accept函数然后再调用该函数
     * @param conn 连接信息
     */
    virtual void onConnection(const TcpConnectionPtr &conn) {
        if (conn->connected()) {
            //连接成功
            std::cout << conn->peerAddress().toIpPort() << " online\n";
        } else {
            //连接断开
            std::cout << conn->peerAddress().toIpPort() << " offline\n";
            conn->shutdown();
            // 结束epoll_wait，退出服务器
            // loop->quit();
        }
    }

    /*!
     * 专门用于处理用户发送数据的回调函数，在read之后调用该函数
     * @param conn 连接信息
     * @param buf 缓冲区
     * @param time 发送时间信息
     */
    virtual void onMessage(const TcpConnectionPtr &conn, Buffer *buf, muduo::Timestamp time) {
        string buf_ = buf->retrieveAllAsString();
        std::cout << "[" << time.toString() << "] " << conn->peerAddress().toIpPort() << " : " << buf_;
        conn->send(buf_);
    }
};

#endif //HELLOMUDUO_SERVER_H
