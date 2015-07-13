//
//  NetAppCCJSController.h
//  Cocos2dx-js controller
//

#include "GNetApplications.h"
#include "GSNotificationPool.h"

class NetAppCCJSController:public GNetApplications{
private:
    std::vector<std::string> msgList;
    std::map<int,std::string> playerList;
public:
    std::string name;
    NetAppCCJSController(std::string a):name(a){
        printf("NetAppCCJSController begin\n");
    }
    ~NetAppCCJSController(){
        printf("NetAppCCJSController end\n");
    }
    void NewConnection(GNPacket gp){
        printf("被通知\n");
        
        playerList.insert(std::make_pair(gp.origin, gp.data));
        
        std::map<int,std::string>::iterator iter;
        for (iter=playerList.begin(); iter!=playerList.end(); ++iter) {
            std::cout<<"origin:"<<iter->first<<" name:"<<iter->second<<std::endl;
        }
        
        msgList.push_back("a player join the room!!");
        GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
        GSNotificationPool::shareInstance()->postNotification("updateMsg", NULL);
    }
    
    void Update(GNPacket gp){
        
    }
    
    void DisConnection(GNPacket gp){
        playerList.erase(gp.origin);
        msgList.push_back("a player leave the room!!");
        GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
        GSNotificationPool::shareInstance()->postNotification("updateMsg", NULL);
    }
    //启动服务器
    void start_server(int);
    //暂停服务器
    void pause_server_service();
    //恢复服务器
    void resume_server_service();
    //停止服务器
    void stop_server_service();
    //启动客户端
    void start_client();
    //暂停客户端
    void pause_client_service();
    //恢复客户端
    void resume_client_service();
    //停止客户端
    void stop_client_service();
    
    //连接服务器
    void connect_server(int ip);
    //获取服务器列表
    std::string get_server_list();
    //获取已经连接到服务器的玩家列表
    std::string get_player_list();
    //发送信息
    bool send_message(std::string jsonString);
    //获取信息
    std::string get_message();
    
    //测试获取服务器（未序列化）
    std::map<unsigned int, std::string>* getServerList();
    //测试获玩家列表（未序列化）
    std::map<int,std::string>* getPalyerList();
};

