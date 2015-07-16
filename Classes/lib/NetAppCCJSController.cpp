//
//  GApp.h
//  NetWorkFrame
//
//  Created by othink on 15/7/7.
//
//
#include "NetAppCCJSController.h"

static NetAppCCJSController* NetAppCCJSControllerInstance;
NetAppCCJSController* NetAppCCJSController::shareInstance(std::string name){
    if (!NetAppCCJSControllerInstance) {
        NetAppCCJSControllerInstance=new NetAppCCJSController(name);
        NetAppCCJSControllerInstance->bind(CLASS_NAME(NetAppCCJSControllerInstance));
        NetAppCCJSControllerInstance->inite();
    }
    return NetAppCCJSControllerInstance;
}

void NetAppCCJSController::NewConnection(GNPacket gp){
    printf("被通知\n");
    
    pthread_mutex_lock(&playInfoMux);
    Document::AllocatorType& allocator=playInfoDoc.GetAllocator();
    Value templ(kObjectType);
    Value key(gp.data.c_str());
    Value val(gp.origin);
    templ.AddMember(key, val, allocator);
    playInfoDoc.PushBack(templ, allocator);
    pthread_mutex_unlock(&playInfoMux);
    
    playerList.insert(std::make_pair(gp.origin, gp.data));
    
    std::map<int,std::string>::iterator iter;
    for (iter=playerList.begin(); iter!=playerList.end(); ++iter) {
        std::cout<<"origin:"<<iter->first<<" name:"<<iter->second<<std::endl;
    }
    
    msgList.push_back("a player join the room!!");
    GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
    GSNotificationPool::shareInstance()->postNotification("updateMsg", NULL);
}

void NetAppCCJSController::Update(GNPacket gp){
    msgList.push_back(gp.data);
    
    pthread_mutex_lock(&dataListMux);
    Value val(gp.data.c_str());
    dataListDoc.PushBack(val, dataListDoc.GetAllocator());
    pthread_mutex_unlock(&dataListMux);
    
    GSNotificationPool::shareInstance()->postNotification("updateMsg", NULL);
}

void NetAppCCJSController::DisConnection(GNPacket gp){
    printf("被通知\n");
    playerList.erase(gp.origin);
    msgList.push_back("a player leave the room!!");
    GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
    GSNotificationPool::shareInstance()->postNotification("updateMsg", NULL);
}

//启动服务器
void NetAppCCJSController::start_server(int playerCount){
    gns->startResponseService(playerCount, name.c_str());
}
//暂停服务器
void NetAppCCJSController::pause_server_service(){
    gns->pauseResponseService();
}
//恢复服务器
void NetAppCCJSController::resume_server_service(){
    gns->resumeResponseService();
}
//停止服务器
void NetAppCCJSController::stop_server_service(){
    gns->stopResponseService();
}
//启动客户端
void NetAppCCJSController::start_client(){
    gns->startSearchService(name.c_str());
}
//暂停客户端
void NetAppCCJSController::pause_client_service(){
    gns->pauseSearchService();
}
//恢复客户端
void NetAppCCJSController::resume_client_service(){
    gns->resumeSearchService();
}
//停止客户端
void NetAppCCJSController::stop_client_service(){
    gns->stopSearchService();
}
//连接服务器
void NetAppCCJSController::connect_server(int ip){
    gns->connectService(ip);
}
void NetAppCCJSController::disconnect_server(){
    gns->disconnectService();
}

std::string NetAppCCJSController::get_server_list(){
    std::map<unsigned int, std::string>* tempa=gns->getTempUdpMap();
    std::map<unsigned int, std::string>::iterator iter=tempa->begin();
    std::map<unsigned int, std::string>::iterator enditer=tempa->end();
    pthread_mutex_lock(&serverInfoMux);
    for (;iter!=enditer; ++iter) {
        Value tempa(kObjectType);
        Value key(iter->second.c_str(),serverInfoDoc.GetAllocator());
        Value val(iter->first);
        tempa.AddMember(key, val, serverInfoDoc.GetAllocator());
        serverInfoDoc.PushBack(tempa, serverInfoDoc.GetAllocator());
    }
    rapidjson::Writer<rapidjson::StringBuffer> writer(serverInfo);
    serverInfoDoc.Accept(writer);
    std::string result(serverInfo.GetString());
    serverInfo.Clear();
    serverInfoDoc.Clear();
    pthread_mutex_unlock(&serverInfoMux);
    return result;
}
//获取已经连接到服务器的玩家列表
std::string NetAppCCJSController::get_player_list(){
    pthread_mutex_lock(&playInfoMux);
    rapidjson::Writer<rapidjson::StringBuffer> writer(playInfo);
    playInfoDoc.Accept(writer);
    std::string result(playInfo.GetString());
    playInfo.Clear();
    playInfoDoc.Clear();
    pthread_mutex_unlock(&playInfoMux);
    return result;
};
//群发信息
bool NetAppCCJSController::send_message(std::string jsonString){
    GNPacket msg;
    msg.UUID=this->UDID;
    msg.data=jsonString;
    if(gns->sendNetPack(msg)>0){
        return true;
    }
    return false;
}
//发送信息
bool NetAppCCJSController::send_message(int tag, std::string jsonString){
    GNPacket msg;
    msg.UUID=this->UDID;
    msg.data=jsonString;
    if(gns->sendNetPack(tag,msg)>0){
        return true;
    }
    return false;
}

//获取信息
std::string NetAppCCJSController::get_message(){
    pthread_mutex_lock(&dataListMux);
    rapidjson::Writer<rapidjson::StringBuffer> writer(dataList);
    dataListDoc.Accept(writer);
    std::string result(dataList.GetString());
    dataList.Clear();
    dataListDoc.Clear();
    pthread_mutex_unlock(&dataListMux);
    return result;
}

std::map<unsigned int, std::string>* NetAppCCJSController::getServerList(){
    return gns->getTempUdpMap();
}

std::map<int,std::string>* NetAppCCJSController::getPalyerList(){
    return &playerList;
}

std::deque<std::string>* NetAppCCJSController::getMsgList(){
    return &msgList;
}

void NetAppCCJSController::sendMsg(int tag){
    std::map<int,std::string>::iterator iter=playerList.find(tag);
    if (iter!=playerList.end()) {
        std::string a(name);
        a.append(":你好,");
        a.append(iter->second);
        GNPacket msg;
        msg.UUID=this->UDID;
        msg.data=a;
        gns->sendNetPack(tag, msg);
    }
}

void NetAppCCJSController::sendMsg(){
    std::map<int,std::string>::iterator iter=playerList.begin();
    std::map<int,std::string>::iterator enditer=playerList.end();
    for (; iter!=enditer; ++iter) {
        this->sendMsg(iter->first);
    }
}
