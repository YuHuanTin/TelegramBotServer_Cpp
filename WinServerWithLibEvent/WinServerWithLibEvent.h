#ifndef LinuxServerWithLibEvent_H
#define LinuxServerWithLibEvent_H
#include <string>
#include <future>
#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include "json.hpp"

#pragma comment(lib,"../lib/event.lib")
#pragma comment(lib,"ws2_32.lib")
#if _DEBUG
#pragma comment(lib,"../lib/Winhttp_SimpleAPId.lib")
#else
#pragma comment(lib,"../lib/Winhttp_SimpleAPI.lib")
#endif


class cHttpServer{
public:
    //pls call it after all operation
    void startServer(const char *ip,int port,void (*cb)(evhttp_request *request,void *userArgs));
    void stopServer();
private:
    event_base *pEventBase;
    evhttp *pEvhttp;
};
void cHttpServer::startServer(const char *ip, int port, void (*cb)(evhttp_request *request, void *userArgs)) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2),&wsaData);

    pEventBase = event_base_new();
    if (pEventBase == nullptr){
        //error
    }
    pEvhttp = evhttp_new(pEventBase);
    if (pEvhttp == nullptr){
        //error
        event_base_free(pEventBase);
    }
    if (evhttp_bind_socket(pEvhttp,ip,port) != 0){
        //error
        stopServer();
    }
    evhttp_set_gencb(pEvhttp,cb,nullptr);

    if (event_base_dispatch(pEventBase) != 0){
        //error
        stopServer();
    }
}
void cHttpServer::stopServer() {
    evhttp_free(pEvhttp);
    event_base_free(pEventBase);
}

#endif //LinuxServerWithLibEvent_H