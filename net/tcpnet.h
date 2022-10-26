#ifndef TCPNET_H
#define TCPNET_H
#include<iostream>
#include"packDef.h"
#include<winsock.h>
#include<QDebug>
#include<windows.h>
#include"INet.h"

class TcpNet:public INet
{
public:
    TcpNet(INetMediator* pMediator);
    ~TcpNet();
    //初始化网络
    bool InitNet(long ip);
    //发送数据
    bool SendData( long ISendIp, char* buf, int nLen);
    //关闭网络
    void UninitNet();

protected:
    //接收数据
    void RecvData();
//    //线程函数，发送数据
     static unsigned __stdcall RecvThread(void* IpVoid);

     SOCKET m_sock;   //套接字
     HANDLE m_handle;   //线程句柄
     bool m_isStop;
     FILE* pFile;   //文件指针
     STRU_FILE_INFO_RQ rq;//当前传输文件的请求信息
      sockaddr_in serverAddr;//sockaddr_in是系统封装的一个结构体，具体包含了成员变量：sin_family、sin_addr、sin_zero
};

#endif // TCPNET_H
