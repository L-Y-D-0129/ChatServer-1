#include "friendmodel.hpp"
#include "db.hpp"
#include <iostream>
using namespace std;
// 添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into Friend values('%d','%d')", userid, friendid);
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 返回用户好友列表
vector<User> FriendModel::query(int userid){
    char sql[1024] = {0};


    //复杂sql语句设计
    sprintf(sql, "select u.id, u.name, u.state from User u join Friend f on u.id = f.friend_id where f.user_id = %d",userid);
    vector<User> vec;
    cout<<"sql::"<<sql<<endl;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            // 把userid用户的所有离线消息放入vec中返回
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            //return vec;
        }
    }
    return vec;
}