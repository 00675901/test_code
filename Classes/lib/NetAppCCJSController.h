//
//  NetAppCCJSController.h
//  Cocos2dx-js controller
//

#include "GNetApplications.h"
#include "GSNotificationPool.h"
#include <deque>

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

class NetAppCCJSController:public GNetApplications{
private:
    std::deque<std::string> msgList;
    std::map<int,std::string> playerList;
    NetAppCCJSController(std::string a):name(a){
        pthread_mutex_init(&playInfoMux, NULL);
        pthread_mutex_init(&serverInfoMux, NULL);
        pthread_mutex_init(&dataListMux, NULL);
        printf("NetAppCCJSController begin\n");
    }
    ~NetAppCCJSController(){
        pthread_mutex_destroy(&playInfoMux);
        pthread_mutex_destroy(&serverInfoMux);
        pthread_mutex_destroy(&dataListMux);
        printf("NetAppCCJSController end\n");
    }
    
public:
    std::string name;
    rapidjson::StringBuffer playInfo;
    rapidjson::Document playInfoDoc;
    pthread_mutex_t playInfoMux;
    
    rapidjson::StringBuffer serverInfo;
    rapidjson::Document serverInfoDoc;
    pthread_mutex_t serverInfoMux;

    rapidjson::StringBuffer dataList;
    rapidjson::Document dataListDoc;
    rapidjson::Value dataListVal;
    pthread_mutex_t dataListMux;
    
    static NetAppCCJSController* shareInstance(std::string);
    
    void inite(){
        playInfoDoc.SetObject();
        serverInfoDoc.SetObject();
        dataListVal.SetObject();
        dataListDoc.SetObject();
    }
    
    void NewConnection(GNPacket gp);
    void Update(GNPacket gp);
    void DisConnection(GNPacket gp);
    
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
    //断开服务器
    void disconnect_server();
    
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
    //测试获取玩家列表（未序列化）
    std::map<int,std::string>* getPalyerList();
    //测试获取信息列表（未序列化）
    std::deque<std::string>* getMsgList();
    //测试发送信息（未序列化,直接发送数据包）
    void sendMsg(int tag);
    //测试获取信息列表（未序列化,直接发送数据包）
    void sendMsg();
};

