#ifndef TCPMEDIATOR_H
#define TCPMEDIATOR_H
#include"INetMediator.h"
#include"tcpclient.h"


class TcpMediator:public INetMediator
{

public:
    TcpMediator();
    ~TcpMediator();
    //打开网络
    bool OpenNet();
    //发送数据
    bool SendData(long ISendIp, char* buf, int nLen);
    //关闭网络
    void CloseNet();
    //接收数据
    void DealData(long ISendIp, char* buf, int nLen);



};

#endif // TCPMEDIATOR_H
