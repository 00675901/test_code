//
//  GNetServer.h
//  Cocos2dx
//  用于完成基于TCP协议网状结构网络
//

#ifndef __GNetServer__
#define __GNetServer__

#include <sys/select.h>
#include <vector>
#include <map>
#include "TcpServer.h"
#include "UdpServer.h"
#include "pthread.h"
#include "ByteBuffer.h"
#include "GNetObserver.h"

#define UDPLIVECOUNT 5
#define TCPLIVECOUNT 5

class GNetServer{
private:
    enum{
        SERVER_STOP,
        SERVER_S_RUN,
        SERVER_S_PAUSE,
        SERVER_C_RUN,
        SERVER_C_PAUSE,
        SERVER_C_TCP_RUN
    }serverStatus;
    
    enum{
        NEWCONNECTION,
        DISCONNECTION,
        PLAYER_NAME,
        REPLAYER_NAME,
        SEND_IP
    }sysEvent;
    
    std::map<std::string,GNetObserver*> obmap;
    
    //public system member
    UdpServer *udps;
    TcpServer *tcps;
//    int serverStatus;
    pthread_mutex_t udpMapMutex;
    pthread_mutex_t remoteFDIPMutex;
    pthread_mutex_t netMutex;
    
    int maxLinsten;
    int localTcpFD;
    unsigned int localIP;
    const char* localName;
    //除自己之外,fd-ip表
    std::map<int,unsigned int> remoteFDIP;
    std::map<int,int> fdStatusMap;
    fd_set rfdset;
    
    //UDP Send localIP Service function(Server)
    pthread_t tidRoomService;
    pthread_t tidListenSConnectService;
    pthread_t tidListenSRemotaService;
    
    //UDP search response 用于搜寻网络(Client)
    std::map<unsigned int, std::string> udpMap; //临时ip-描述信息表
    std::map<int,int> udpMapStatus;
    pthread_t tidSearchServer;
    pthread_t tidRecvServer;
    //TCP connect
    pthread_t tidListenCConnectService;
    pthread_t tidListenCRemotaService;
    
    GNetServer(void){
        serverStatus=SERVER_STOP;
        pthread_mutex_init(&udpMapMutex, NULL);
        pthread_mutex_init(&remoteFDIPMutex, NULL);
        pthread_mutex_init(&netMutex, NULL);
        printf("GNetServer BEGIN\n");
    }
    ~GNetServer(void){
        obmap.clear();
        pthread_mutex_destroy(&udpMapMutex);
        pthread_mutex_destroy(&remoteFDIPMutex);
        pthread_mutex_destroy(&netMutex);
        printf("GNetServer END\n");
    }
public:
    static GNetServer* shareInstance();
    //netService secretary
    bool addObs(std::string name ,GNetObserver* gob){
        if (obmap.find(name)==obmap.end()) {
            obmap.insert(std::make_pair(name, gob));
            return true;
        }
        return false;
    }
    void removeObs(std::string name){obmap.erase(name);}
    void removeAllObs(){obmap.clear();}
    void distributeData(std::string name,GNPacket tp){
        obmap[name]->Update(tp);
    }
    //数据包群发通知
    void notificationData(GNPacket tp){
         std::map<std::string,GNetObserver*>::iterator iter=obmap.begin();
         while (iter!=obmap.end()) {
            ((*iter).second)->Update(tp);
            iter++;
         }
    }
    //系统全局通知
    void notificationSystemData(GNPacket tp){
        printf("通知全局\n");
        std::map<std::string,GNetObserver*>::iterator iter=obmap.begin();
        if (NEWCONNECTION==tp.sysCode) {       //操作码为1 通知所有ob有新连接
            while (iter!=obmap.end()) {
                ((*iter).second)->NewConnection(tp);
                iter++;
            }
        }else if(DISCONNECTION==tp.sysCode){  //操作码为-1 通知所有ob有断开连接
            while (iter!=obmap.end()) {
                ((*iter).second)->DisConnection(tp);
                iter++;
            }
        }
    }
    
    //use member function
    unsigned int* getLocalIP();
    const char* getLocalName();
    std::map<int,unsigned int>* getRemoteFDIP(); //获取除自己外fd-ip表
    std::map<unsigned int, std::string>* getTempUdpMap();
    
    //UDP Send localIP Service function
    void startResponseService(int maxl,const char* uname);
    void pauseResponseService();
    void resumeResponseService();
    void stopResponseService();
    static void* responseService(void* obj);
    //监听连接\接收封包线程
    static void* listenSConnectServer(void* obj);
    static void* listenSRemotaServer(void* obj);
    
    static void* listenSNetService(void* obj);
    
    
    //UDP search response
    void startSearchService(const char* uname);
    void pauseSearchService();
    void resumeSearchService();
    void stopSearchService();
    static void* searchServer(void* obj);
    static void* recvServerList(void* obj);
    //TCP connect
    void connectService(int addr);
    void disconnectService();
    //监听tcp通信,接收封包
    static void* listenCConnectServer(void* obj);
    static void* listenCRemotaServer(void* obj);
    
    static void* listenCNetService(void* obj);
    
    //通过tcp fd 发送封包
    long sendNetPack(int,GNPacket*);
    long sendNetPack(GNPacket*);
    
    //心跳线程
    static void* nettyNetService(void* obj);
};

#endif /* defined(__GNetServer__) */
