#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include<mutex>
#include "usermodel.hpp"
#include"offlinemessagemodel.hpp"
#include"friendmodel.hpp"
#include"groupmodel.hpp"
#include"redis.hpp"

using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "json.hpp"
#include <string>
using namespace placeholders;
using json = nlohmann::json;

using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>;

// 聊天服务器业务类
class ChatService
{
public:
    //获取单例对象的接口函数
    static ChatService* instance();

    // 处理登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 处理注册业务
    void regis(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //获取消息对应的Handler处理器
    MsgHandler getHandler(int msgid);

    //处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr&conn);

    //处理服务器异常退出
    void reset();

    //一对一聊天业务
    void oneChat(const TcpConnectionPtr& conn,json& js,Timestamp time);

    //添加好友业务
    void addFriend(const TcpConnectionPtr& conn, json& js, Timestamp time);

    //创建群组业务
    void createGroup(const TcpConnectionPtr& conn, json& js, Timestamp time);

    //加入群组业务
    void addGroup(const TcpConnectionPtr& conn, json& js, Timestamp time);

    //群组聊天业务
    void groupChat(const TcpConnectionPtr& conn, json& js, Timestamp time);

    //处理注销业务
    void loginout(const TcpConnectionPtr& conn, json& js, Timestamp time);

    //从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int userid,std::string msg);

private:
    ChatService();

    //存储消息id和其对应的业务处理方法 
    unordered_map<int,MsgHandler> _msgHandlerMap;

    /*存储在线用户的通信连接。因为连接是由多线程管理的，那用户的信息
    就很容易被其他线程修改，所以要注意线程安全问题。
    */
    unordered_map<int,TcpConnectionPtr> _userConnMap;

    //定义互斥锁，保证_userConnMap的线程安全
    mutex _connMutex;

    //数据操作类对象
    UserModel _usermodel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    //redis操作对象
    Redis _redis;
};

#endif