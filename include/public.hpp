#ifndef PUBLIC_H
#define PUBLIC_H
/*
server和client的公共文件
*/

enum EnMsgType
{
    LOGIN_MSG=1,   //登录消息1
    LOGIN_MSG_ACK, //登录成功响应消息2
    REG_MSG,   //注册消息3
    REG_MSG_ACK,    //注册成功相应消息4
    ONE_CHAT_MSG,   //聊天消息5
    ADD_FRIEND_MSG, //添加好友消息6
    CREATE_GROUP_MSG,  //创建群组7
    ADD_GROUP_MSG,  //加入群组8
    GROUP_CHAT_MSG,  //群聊天9
    LOGINOUT_MSG,  //退出登录10
};




#endif