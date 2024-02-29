#include "chatserver.hpp"
#include "chatservice.hpp"
#include <functional>
#include"json.hpp"
using namespace std;
using namespace placeholders;
using json = nlohmann::json;

// 初始化聊天服务器对象
ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg) : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册连接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 注册读写事件回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置线程数量
    _server.setThreadNum(4);
}

// 启动服务
void ChatServer::start()
{
    _server.start();
}

// 用户连接相关的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    //客户端断开连接
    if(!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

// 读写事件相关的回调函数  ①
//该函数会被多个线程回调，要注意线程安全问题
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    string buf=buffer->retrieveAllAsString();
    //数据的反序列化
    json js=json::parse(buf);

    //达到的目的：完全解耦合网络模块的代码和业务模块的代码
    //通过msgidl来获取其对应的Handler处理函数
    auto msgHandler =ChatService::instance()->getHandler(js["msgid"].get<int>());  //通过get函数转换成int类型值
    //所获得的消息对应的Handler处理函数
    msgHandler(conn,js,time);
}