#include "redis.hpp"
#include <iostream>

Redis::Redis() : _publish_context(nullptr), _subscribe_context(nullptr)
{
}

Redis::~Redis()
{
    if (_publish_context != nullptr)
    {
        redisFree(_publish_context);
    }

    if (_subscribe_context != nullptr)
    {
        redisFree(_subscribe_context);
    }
}

bool Redis::connect()
{
    // 负责publish发布消息的上下文连接
    // 6379是redis服务器所使用的端口号
    _publish_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _publish_context)
    {
        std::cerr << "connect redis failed!" << std::endl;
        return false;
    }

    // 负责subscribe订阅消息的上下文连接
    _subscribe_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _subscribe_context)
    {
        std::cerr << "connect redis failed!" << std::endl;
        return false;
    }

    // 在单独的线程中，监听通道上的事件，有消息给业务层进行上报
    std::thread t([&]()
                  { observer_channel_message(); });
    t.detach();

    std::cout << "connect redis-server success !" << std::endl;

    return true;
}

// 向redis指定的通道channel publish消息
bool Redis::publish(int channel, std::string message)
{
    // redisCommand函数的作用，就相当于以第一个参数作为redis-cli，发送后面参数为主的信息
    redisReply *reply = (redisReply *)redisCommand(_publish_context, "PUBLISH %d %s", channel, message.c_str());

    if (nullptr == reply)
    {
        std::cerr << "publish command failed!" << std::endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

// 向redis指定的通道 subscribe订阅消息
bool Redis::subscribe(int channel)
{
    /*
    SUBSCRIBE命令本身会造成线程阻塞，等待通道里面发生消息。所以这里只做订阅通道，不接收通道消息。
    通道消息的接收，专门 由observer_channel_message函数中的 独立线程负责。
    只负责发送命令，但不阻塞接收redis server的响应信息，否则会和notifyMsg线程抢占响应资源。
    */
    if (REDIS_ERR == redisAppendCommand(this->_subscribe_context, "SUBSCRIBE %d", channel))
    {
        std::cerr << "subscribe command failed !" << std::endl;
        return false;
    }
    // redisBufferWrite可以循环发送缓冲区，直到缓冲区数据发送完毕（done被值1）
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subscribe_context, &done))
        {
            std::cerr << "subscribe command failed!!" << std::endl;
            return false;
        }
    }

    /*
    这里说一下redisCommand和redisAppendComand的关系：
    redisCommand的底层，其实是redisAppendCommand、redisBufferWrite、redisGetReply三个函数的组合。
    也就是先通过redisAppendCommand把命令写进缓冲区，然后用redisBufferWrite把缓冲区中的东西发到redis-server上执行，
    最后用redisGetReply等待redis-server返回响应结果。

    为什么_publish_context使用redisCommand，而_subscribe_context不用？
    因为在redis-server中，publish命令不会阻塞，会立即返回。而subscribe在redis-server中会阻塞，不会有返回。
    */

    return true;
}

// 向redis指定的通道 unsubscribe取消订阅消息
bool Redis::unsubscribe(int channel)
{
    if (REDIS_ERR == redisAppendCommand(this->_subscribe_context, "UNSUBSCRIBE %d", channel))
    {
        std::cerr << "unsubscribe command failed!" << std::endl;
        return false;
    }

    // redisBufferWrite可以循环发送缓冲区内容，直到缓冲区数据发送完毕(done被置为1)
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subscribe_context, &done))
        {
            std::cerr << "unsubscribe command failed!" << std::endl;
            return false;
        }
    }

    return true;
}

// 在独立线程中，接收订阅通道中的消息
void Redis::observer_channel_message()
{
    redisReply *reply = nullptr;

    // 以循环阻塞的方式，等待_subscribe_context连接对象中，是否有消息发生
    while (REDIS_OK == redisGetReply(this->_subscribe_context, (void **)&reply))
    {
        // 订阅收到的消息，是一个带三个元素的数组，在Linux的redis-server上，给某一个订阅的通道publish一个消息可以看出
        if (reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr)
        {
            // 把通道发生的消息，推送到相应的ChatServer服务器上
            // element[1]是通道号，element[2]就是通道消息
            _notify_message_handler(atoi(reply->element[1]->str), reply->element[2]->str);
        }

        freeReplyObject(reply);
    }

    std::cerr << ">>>>>>>>>>>>>>>>>>>>>>observer_channel_message quit" << std::endl;
}

// 初始化 向业务层上报通道消息的 回调对象
void Redis::init_notify_handler(std::function<void(int, std::string)> fn)
{
    this->_notify_message_handler=fn;
}