#ifndef CHATSERVER_H
#define CHATSERVER_H

#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<iostream>

using namespace muduo;
using namespace muduo::net;

class ChatServer
{
    public:
        //初始化聊天服务器对象
        ChatServer(EventLoop* loop,const InetAddress& listenfd,const string& nameArg);

        //启动服务
        void start();
    private:
        // 用户连接相关的回调函数
        void onConnection(const TcpConnectionPtr&);

        // 读写事件相关的回调函数
        void onMessage(const TcpConnectionPtr&,Buffer*,Timestamp);

        TcpServer _server;   //服务器对象
        EventLoop *_loop;    //事件循环对象
};


#endif