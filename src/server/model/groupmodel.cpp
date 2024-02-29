#include "groupmodel.hpp"
#include "db.h"

// 创建群组
bool GroupModel::createGroup(Group &group)
{
    char sql[1024] = {0};
    std::sprintf(sql, "insert into allgroup(groupname,groupdesc) values('%s','%s')", group.getName().c_str(), group.getDesc().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }

    return false;
}

// 加入群组
void GroupModel::addGroup(int userid, int groupid, std::string role)
{
    char sql[1024] = {0};
    std::sprintf(sql, "insert into groupuser values(%d,%d,'%s')", groupid, userid, role.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 查询用户所在群组信息
std::vector<Group> GroupModel::queryGroups(int userid)
{
    /*
    1. 先根据userid在groupuser表中查询出该用户所属的群组消息
    2. 再根据群组消息，联合group表，查询出具体消息
    */
    char sql[1024] = {0};
    std::sprintf(sql, "select a.id,a.groupname,a.groupdesc from allgroup a inner join groupuser b on a.id=b.groupid where b.userid=%d",
                 userid);

    std::vector<Group> groupvec; //存放用户所加的所有群信息
    MySQL mysql;

    if (mysql.connect())
    {
        MYSQL_RES* res=mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupvec.push_back(group);
            }

            mysql_free_result(res);
        }
    }

    // 查询每个群组的成员用户信息
    for(Group& group : groupvec)
    {
        std::sprintf(sql,"select a.id,a.name,a.state,b.grouprole from user a inner join groupuser b on a.id=b.userid where b.groupid=%d",
        group.getId());

        MYSQL_RES* res=mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
            }

            mysql_free_result(res);
        }
    }

    return groupvec;
}

// 根据给定的groupid查询群组用户id列表，除userid自己外，给组内的其他成员群发消息
// 相当于就是指定用户给指定群组发消息，那么其他成员就可以收到这个消息
std::vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[1024]={0};
    std::sprintf(sql,"select userid from groupuser where groupid=%d and userid !=%d",groupid,userid);

    std::vector<int> idvec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES* res=mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                idvec.push_back(atoi(row[0]));
            }

            mysql_free_result(res);
        }
    }

    return idvec;
}