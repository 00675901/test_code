//
//  GRServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14/11/10.
//
//

#include "GRServer.h"

GRServer::GRServer(void){
    printf("GRServer BEGIN\n");
}
GRServer::~GRServer(void){
    printf("GRServer END\n");
}

int GRServer::sendData(int fd,GCData* pack){
    if (pack->opcode.empty()) {
        pack->opcode="0000";
    }
    int oplen=pack->opcode.length();
    if (oplen==4) {
        int len=pack->data.length()+4;
        char sh[5];
        sprintf(sh,"%04d",len);
        int isl=len+4;
        char sendchar[isl];
        sprintf(sendchar,"%s%s%s",sh,pack->opcode.c_str(),pack->data.c_str());
        int re=send(fd,sendchar,isl,MSG_WAITALL);
        printf("sendCC:%s\nsend:%s\nopc:%s\nsize:%d\n",sh,pack->data.c_str(),pack->opcode.c_str(),re);
        return re;
    }else{
        printf("sendData error:opcode Error\n");
        return -1;
    }
}
int GRServer::recvData(int fd,GCData* pack){
    char bufhead[5];
    int ret=recv(fd, bufhead, 4, MSG_WAITALL);
    int dl=0;
    if (ret==4) {
        bufhead[4]='\0';
        dl=GUtils::ctoi(bufhead);
    }
    if (dl>0) {
        char bufcon[dl];
        ret=recv(fd, bufcon, dl, MSG_WAITALL);
        string tempcon=bufcon;
        pack->opcode=tempcon.substr(0,4);
        pack->data=tempcon.substr(4,dl-4);
    }
    return ret;
}
