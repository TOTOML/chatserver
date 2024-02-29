#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include"group.hpp"
#include<vector>
#include<string>

class GroupModel
{
    public:
        //创建群组
        bool createGroup(Group& group);

        //加入群组
        void addGroup(int userid,int groupid,std::string role);

        //查询用户所在群组信息
        std::vector<Group> queryGroups(int userid);

        //根据给定的groupid查询群组用户id列表，除userid自己外，给组内的其他成员群发消息
        //相当于就是指定用户给指定群组发消息，那么其他成员就可以收到这个消息
        std::vector<int> queryGroupUsers(int userid,int groupid);
};


#endif