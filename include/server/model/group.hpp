#ifndef GROUP_H
#define GROUP_H
#include "groupuser.hpp"
#include <string>
#include<vector>
using namespace std;
class Group{
public:
    Group(int id =-1,string name = "",string desc = ""){
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void setId(int id){this->id =id;}
    void setName(string name) {this->name = name;}
    void setDesc(string desc){this->desc=desc;}
    vector<GroupUser> &getUsers(){return this->users;}
    
    int getId() { return this->id; }
    string getName() { return this->name; }
    string getDesc() { return this->desc; }
private:
    //组id，名字，描述
    int id;
    string name;
    string desc;
    
    //群组里面所有成员
    vector<GroupUser> users;
};
#endif