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
        NetAppCCJSControllerInstance->bind();
    }
    return NetAppCCJSControllerInstance;
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
    gns->getTempUdpMap();
    return "";
}
//获取已经连接到服务器的玩家列表
std::string NetAppCCJSController::get_player_list(){
    std::map<unsigned int,std::string> *tempm=gns->getTempUdpMap();
    std::map<unsigned int,std::string>::iterator iter;
    for (iter=tempm->begin(); iter!=tempm->end(); ++iter) {
        std::cout<<"ip:"<<iter->first<<"----name:"<<iter->second<<std::endl;
    }
    return "";
};
//发送信息
bool NetAppCCJSController::send_message(std::string jsonString){
    GNPacket msg;
    msg.UUID=this->UDID;
    msg.data=jsonString;
    if(gns->sendNetPack(&msg)>0){
        return true;
    }
    return false;
}
//获取信息
std::string NetAppCCJSController::get_message(){
    return "";
}

std::map<unsigned int, std::string>* NetAppCCJSController::getServerList(){
    return gns->getTempUdpMap();
}

std::map<int,std::string>* NetAppCCJSController::getPalyerList(){
    return &playerList;
}

std::vector<std::string>* NetAppCCJSController::getMsgList(){
    return &msgList;
}
