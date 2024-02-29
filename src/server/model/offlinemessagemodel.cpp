#include "offlinemessagemodel.hpp"
#include "db.h"
#include<string>

// 存储用户的离线信息
void OfflineMsgModel::insert(int userid, std::string msg)
{
    char sql[1024]={0};
    std::sprintf(sql,"insert into offlinemessage values(%d,'%s')",userid,msg.c_str());

    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            return ;
        }
    }
}

// 删除用户的离线信息
void OfflineMsgModel::remove(int userid)
{
    char sql[1024]={0};
    std::sprintf(sql,"delete from offlinemessage where id=%d",userid);

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}

// 查询用户的离线信息
std::vector<std::string> OfflineMsgModel::query(int userid)
{
    char sql[1024]={0};
    std::sprintf(sql,"select message from offlinemessage where id = %d",userid);

    std::vector<std::string> vec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES* res=mysql.query(sql);
        if(res!=nullptr)
        {
            //把userid用户的所有离线消息放入vec中返回
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res)) != nullptr)
            {
                /*这里为什么是row[0]呢？因为row存储的是一行信息，因为我们指定只挑选message属性的值，
                所以这一行信息中，只包含message的值，所以row[0]就是message的值
                */
                vec.push_back(row[0]);
            }

            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}