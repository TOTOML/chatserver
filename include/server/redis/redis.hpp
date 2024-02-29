#ifndef REDIS_H
#define REDIS_H

#include <hiredis/hiredis.h>
#include <thread>
#include <functional>

/*
redis作为集群服务器通信的基于发布-订阅消息队列时，会遇到两个难搞的bug问题，参考以下博客详细描述：
https://blog.csdn.net/QIANGWEIYUAN/article/details/97895611
*/

class Redis
{
public:
    Redis();
    ~Redis();

    // 连接redis服务器
    bool connect();

    // 向redis指定的通道publish发布消息
    bool publish(int channel, std::string message);

    // 向redis指定的通道subscribe订阅消息
    bool subscribe(int channel);

    // 向redis指定的通道unsubscribe取消订阅消息
    bool unsubscribe(int channel);

    // 在独立线程中 接收 订阅通道中的消息
    void observer_channel_message();

    // 初始化 向业务层上报通道消息的 回调对象
    void init_notify_handler(std::function<void(int, std::string)> fn);

private:
    /*
    下面的同步上下文对象，实际上就是一个redis-cli，既可以publish，也可以subscribe
    那么为什么要使用到两个redis-cli呢？
    那是因为，在redis中使用subscribe命令后，会进行阻塞状态，等待对方publish信息过来。
    如果只使用一个redis-cli，那么当这个redis-cli调用了subscribe后就会阻塞，不能再调用publish了。
    */

    //hiredis同步上下文对象，负责publish消息
    redisContext* _publish_context;

    //hiredis同步上下文对象，负责subscribe消息
    redisContext* _subscribe_context;

    //回调操作，收到订阅的消息，给server上报
    std::function<void(int,std::string)> _notify_message_handler;
};

#endif