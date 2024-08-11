# nginx集群聊天服务器
## 业务流程
项目大概是模仿微信去实现一个通讯工具，主要业务分为注册、登录、加好友、查看离线消息、一对一群聊、创建群、加入群、群聊等，详细业务流程关系如下图： 
![68747470733a2f2f696d672d626c6f672e6373646e696d672e636e2f323032313031323731333533353632382e706e673f782d6f73732d70726f636573733d696d6167652f77617465726d61726b2c747970655f5a6d46755a33706f5a57356e6147567064476b2c736861646f775f31302c74](https://github.com/user-attachments/assets/a3a4c7f9-f917-4d00-ab53-f71b557e1240)

## 数据模板
### 表的设计
首先就是要解决数据的问题，作为一个聊天系统，我们的服务器端肯定要有用户的信息，比如说账号，用户名，密码等。 在登录的时候，我们可以查询这个表里面的信息对用户身份进行验证，在注册的时候，我们则可以往表里面去写入数据。
#### 表User:
![68747470733a2f2f696d672d626c6f672e6373646e696d672e636e2f32303231303132373134343432363734352e706e67](https://github.com/user-attachments/assets/c3f5b101-cddb-4cdc-bcfd-7d94d2683a9f)

用户登录之后，首先就是进行聊天业务，我们必须要知道该用户的好友都有谁。 在加好友时，我们就可以往这张表里面去写入信息并在一对一聊天时查询这里面的信息去看好友是否在线。
#### 表Friend：
![68747470733a2f2f696d672d626c6f672e6373646e696d672e636e2f32303231303132373134343735383133372e706e67](https://github.com/user-attachments/assets/8a0becab-9e36-42d5-afc5-b8f46569ab59)

但是我们的设计目的又要存储离线消息，这就涉及到离线消息发给谁，谁发的，发的什么三个问题，所以我们又需要一个新表来存储离线消息。这样我们一旦有离线消息便可以往这个表里面去写入数据。
#### 表OffilineMessage：
![68747470733a2f2f696d672d626c6f672e6373646e696d672e636e2f32303231303132373134353231353536322e706e67](https://github.com/user-attachments/assets/60198540-30f7-48c6-9df8-69049065119f)

然后便是群组业务了，群组中我们需要有一个记录群组信息的表，方便我们创建群时往其中去写入数据；
#### 表AllGroup：
![68747470733a2f2f696d672d626c6f672e6373646e696d672e636e2f32303231303132373134353630393236332e706e67](https://github.com/user-attachments/assets/1845a88f-fd7e-4ec5-850b-9a42db298e26)

同时群里面肯定是有群员的，我们就需要一个记录群成员的表，我们在加入群的时候，把用户id写入这个表。并且在发送群消息的时候查询这个表由服务器向这些成员转发消息。

![68747470733a2f2f696d672d626c6f672e6373646e696d672e636e2f32303231303132373135303033303733342e706e67](https://github.com/user-attachments/assets/58cf28bb-bfc0-479b-b0e8-13f0079d887c)
### 数据库模板设计
在/include/db/MySQL.hpp文件中，封装着对数据库的连接、查询、更新、释放连接几个操作。
其是数据库模块中设计的最底层，为上层各个表以及其操作模块提供基础的服务。其关系图如下所示：

![68747470733a2f2f696d672d626c6f672e6373646e696d672e636e2f32303231303132373135313135353133392e706e673f782d6f73732d70726f636573733d696d6167652f77617465726d61726b2c747970655f5a6d46755a33706f5a57356e6147567064476b2c736861](https://github.com/user-attachments/assets/a9433212-dd31-4a84-bd1f-cdd0cb7519b7)


这里要解释几点：

*  名字带有model的类都是对数据库的操作类并不负责存储数据，而像User这个类则是负责暂存从数据库中查询到的数据
*  FriendModel和OfflineMessageModel则是没有暂存数据的上层类，这是因为对于Friend来说，其数据本身就是一个User，只需要查询到好友的id然后在User表中内联查询一下便可得到信息；
*  对于OfflineMessage这是没有必要这些类都在/include/model里面
## 通信格式
服务器和客户端的通信采用了JSON来完成数据在网络中的标准传输。

对于不同的数据则是采用了不同的格式，具体如下：
'''
1.登录
json["msgid"] = LOGIN_MSG;
json["id"]			//用户id
json["password"]	//密码

2.登录反馈
json["msgid"] = LOGIN_MSG_ACK;
json["id"]			//登录用户id
json["name"]		//登录用户密码
json["offlinemsg"]	//离线消息
json["friends"]		//好友信息,里面有id、name、state三个字段
json["groups"]		//群组信息,里面有id，groupname，groupdesc，users三个字段
					//users里面则有id，name，state，role四个字段
json["errno"]		//错误字段，错误时被设置成1，用户不在线设置成2
json["errmsg"]		//错误信息

3.注册
json["msgid"] = REG_MSG;
json["name"]		//用户姓名
json["password"]	//用户姓名

4.注册反馈
json["msgid"] = REG_MSG_ACK;
json["id"]			//给用户返回他的id号
json["errno"]		//错误信息，失败会被设置为1

5.加好友
json["msgid"] = ADD_FRIEND_MSG;
json["id"]			//当前用户id
json["friendid"]	//要加的好友的id

6.一对一聊天
json["msgid"] = ONE_CHAT_MSG;
json["id"]			//发送者id
json["name"]		//发送者姓名
json["to"]			//接受者id
json["msg"]			//消息内容
json["time"]		//发送时间

7.创建群
json["msgid"] = CREATE_GROUP_MSG;
json["id"]			//群创建者id
json["groupname"]	//群名
json["groupdesc"]	//群描述

8.加入群
json["msgid"] = ADD_GROUP_MSG;
json["id"]			//用户id
json["groupid"]		//群id

9.群聊
json["msgid"] = GROUP_CHAT_MSG;
json["id"]			//发送者id
json["name"]		//发送者姓名
json["groupid"]		//发送者姓名
json["msg"]			//消息内容
json["time"]		//发送时间

10.注销
json["msgid"] = LOGINOUT_MSG;
json["id"]			//要注销的id
'''





