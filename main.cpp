#include <iostream>
#include <utility>
#include <vector>
#include <thread>
#include "DataTypes.h"
#include "WinSock2.h"
#include "curl/curl.h"
#include "json.hpp"

#pragma comment(lib,"crypt32.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wldap32.lib")

#if _DEBUG
#pragma comment(lib,"../curl/lib/x86/libcurld.lib")
#pragma comment(lib,"../openssl/debug/lib/libssl.lib")
#pragma comment(lib,"../openssl/debug/lib/libcrypto.lib")
#else
#pragma comment(lib,"../curl/lib/x86/libcurl.lib")
#pragma comment(lib,"../openssl/release/lib/libssl.lib")
#pragma comment(lib,"../openssl/release/lib/libcrypto.lib")
#endif

std::string gToken="";//机器人bot的token

void fn_Wcs2Mbs(wchar_t *wcpStr,char *&cpDst,unsigned CP_dst){
    cpDst= nullptr;
    size_t len=WideCharToMultiByte(CP_dst,NULL,wcpStr, -1, nullptr, 0,nullptr,nullptr);
    cpDst=(char *)realloc(cpDst,len * sizeof(char));
    WideCharToMultiByte(CP_dst,NULL,wcpStr,-1,cpDst,(int)(len * sizeof(char)),nullptr,nullptr);
}
void fn_Mbs2Wcs(char *cpStr,wchar_t *&wcpDst,unsigned CP_dst){
    wcpDst= nullptr;
    size_t len=MultiByteToWideChar(CP_dst,NULL,cpStr,-1,nullptr,0);
    wcpDst=(wchar_t *)realloc(wcpDst,len * sizeof(wchar_t));
    MultiByteToWideChar(CP_dst,NULL,cpStr,-1,wcpDst,(int)(len * sizeof(wchar_t)));
}
void fn_EncodingSwitch(std::string &cpSrc,std::string &cpDst, unsigned CP_src, unsigned CP_dst){
    char *cpData;
    wchar_t *wcpData;
    fn_Mbs2Wcs(cpSrc.data(),wcpData,CP_src);
    fn_Wcs2Mbs(wcpData, cpData, CP_dst);
    cpDst.clear();
    cpDst.append(cpData);
    free(cpData);
    free(wcpData);
}

size_t fn_Curl_DataCallBack(char *ptr, size_t size, size_t len, void *userData){
    auto pStr=(std::string *)userData;
    pStr->append(ptr,size *len);
    return size*len;
}
std::string fn_Curl_HttpGet(const char *url){
    std::string szContext;
    CURL *curl_easy=curl_easy_init();
    curl_easy_setopt(curl_easy,CURLOPT_URL,url);
    //curl_easy_setopt(curl_easy,CURLOPT_PROXY,"127.0.0.1:11223");
    curl_easy_setopt(curl_easy,CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl_easy, CURLOPT_WRITEFUNCTION, fn_Curl_DataCallBack);
    curl_easy_setopt(curl_easy,CURLOPT_WRITEDATA,&szContext);
    CURLcode recode=curl_easy_perform(curl_easy);
    if (recode != CURLE_OK){
        printf("error:curl_easy_perform\n");
    }
    curl_easy_cleanup(curl_easy);
    return szContext;
}
bool fn_Socket_CloseWebServer(SOCKET hSocket){
    if (closesocket(hSocket) == SOCKET_ERROR){
        printf("error:closesocket\n");
        return false;
    }
    if (WSACleanup() == SOCKET_ERROR){
        printf("error:WSACleanup\n");
        return false;
    }
    return true;
}
bool fn_Socket_IsBannedIP(){

    return false;
}
int fn_Socket_StartWebServer(int fn_RecvDataCallBack(SOCKET,const char *), SOCKET &hSocket, bool &turnoff){
    /*
     * arg1:<fn>callback
     * arg2:返回socket句柄
     * arg3:传入控制线程变量
     */
    /* 初始化socket */
    WSAData wsaData{};

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0){
        printf("error:WSAStartup\n");
        return 0;
    }
    /* 创建socket */
    hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET) {
        printf("error:socket\n");
        return 0;
    }
    /* 绑定IP地址和端口号 */
    sockaddr_in serAddr{
            AF_INET,//与socket创建时一样
            htons(80),
            INADDR_ANY//绑定本机的任意端口
    };
    if (bind(hSocket, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR) {
        printf("error:bind\n");
        return 0;
    }
    /* 监听 */
    /* 待处理连接队列的最大长度,可以为SOMAXCONN或SOMAXCONN_HINT(N) */
    if (listen(hSocket,SOMAXCONN) == SOCKET_ERROR ){
        printf("error:listen\n");
        return 0;
    }
    /* 接受socket数据 */
    sockaddr_in cliAddr{};
    int lenOfAddr = sizeof(cliAddr);
    while (!turnoff) {
        SOCKET hClient = accept(hSocket, (sockaddr *)&cliAddr, &lenOfAddr);//阻塞
        if (hClient == INVALID_SOCKET) {
            printf("error:accept\n");
            closesocket(hClient);
            return 0;
        }
        printf("[info]connect ip:%s port:%d\n",inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));
        if (fn_Socket_IsBannedIP()){
            printf("[info]Banned ip:%s port:%d try to connect\n",inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));
            closesocket(hClient);
        }
        /* 接收数据 */
        char buffer[4092]{0};
        std::string szBuf;
        while (recv(hClient, buffer, 4092, 0) == SOCKET_ERROR){
            if (WSAGetLastError() == WSAEMSGSIZE){
                szBuf.append(buffer);
                memset(buffer,0,4092);
                continue;
            }
            printf("error:recv\n");
            closesocket(hClient);
            return 0;
        }
        szBuf.append(buffer);
        /* 处理数据 */
        if (!szBuf.empty()){
            fn_RecvDataCallBack(hClient, szBuf.c_str());
        }

        /* 发送完直接关闭，http是无连接的 */
        closesocket(hClient);
    }
    /* turnoff传入true时关闭socket与线程 */
    fn_Socket_CloseWebServer(hSocket);
    return 1;
}
int fn_Socket_RecvData(SOCKET hClient,const char *recvData){
    std::string sendData;
    std::string ContextData= recvData;
    ContextData=ContextData.substr(ContextData.find("\r\n\r\n")+4);
    printf("[data]%s\n",ContextData.c_str());
    if (!nlohmann::json::accept(ContextData)){
        printf("error:json::accept\n");
        return 0;
    }
    nlohmann::json nJson = nlohmann::json::parse(ContextData);
    stTg_Message tgMessage{};
    if (!nJson["message"].is_null()){
        if (!nJson["message"]["from"].is_null()){
            tgMessage.from.id=nJson["message"]["from"]["id"];
            tgMessage.from.is_bot=nJson["message"]["from"]["is_bot"];
            tgMessage.from.first_name=nJson["message"]["from"]["first_name"];
            tgMessage.from.last_name=nJson["message"]["from"]["last_name"];
            tgMessage.from.username=nJson["message"]["from"]["username"];
            tgMessage.from.language_code=nJson["message"]["from"]["language_code"];
        }
        if (!nJson["message"]["chat"].is_null()){
            tgMessage.chat.id=nJson["message"]["chat"]["id"];
            tgMessage.chat.first_name=nJson["message"]["chat"]["first_name"];
            tgMessage.chat.last_name=nJson["message"]["chat"]["last_name"];
            tgMessage.chat.username=nJson["message"]["chat"]["username"];
            tgMessage.chat.type=nJson["message"]["chat"]["type"];
        }
        tgMessage.date=nJson["message"]["date"];
        tgMessage.text=nJson["message"]["text"];
    }

    std::string httpResponse=fn_Curl_HttpGet((
            "https://api.telegram.org/bot"+gToken
            +"/sendMessage?chat_id="
            +std::to_string(tgMessage.from.id)
            +"&text="
            +tgMessage.text
            ).c_str());

    printf("[data]%s\n", httpResponse.c_str());

    sendData = "HTTP/1.1 200 OK\r\n\r\nsucceed receive data!";
    if (send(hClient, sendData.c_str(), (int)sendData.length(), 0) == SOCKET_ERROR){
        printf("error:send\n");
        return 0;
    }
    return 1;
}
int main() {
#if _DEBUG


#else
    SOCKET hSocket;
    bool bTurnOffThread= false;
    curl_global_init(CURL_GLOBAL_ALL);
    //指定fn_RecvData()为接收数据函数
    //std::ref()指定为返回值,共享主线程,子程序内存
    std::thread WebServer(fn_Socket_StartWebServer, fn_Socket_RecvData, std::ref(hSocket), std::ref(bTurnOffThread));
    printf("[info]ServerOpen\n");


    Sleep(-1);


    //注册回调
    //fn_Curl_HttpGet(("https://api.telegram.org/bot"+gToken+"/setWebhook?url=").c_str()); //设置WebHook


    /*
    Sleep(-1);
    //程序清理
    bTurnOffThread= true;
    curl_global_cleanup();
     */
#endif
    return 0;
}
