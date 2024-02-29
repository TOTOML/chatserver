//该头文件，是专门处理User表的业务类，针对User表的专门的数据操作

#ifndef USERMODEL_H
#define USERMODEL_H
#include"user.hpp"

//该类是对User表的数据操作类
class UserModel
{
    public:
        //  User表的增加方法
        bool insert(User& user);

        //  根据用户id查询用户信息
        User query(int id);

        //  更新用户的状态信息
        bool updatestate(User user);

        //  重置用户的状态信息
        void resetState();
};

#endif