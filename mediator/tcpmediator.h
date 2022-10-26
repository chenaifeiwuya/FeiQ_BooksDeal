#ifndef TCPMEDIATOR_H
#define TCPMEDIATOR_H
#include"INetMediator.h"
#include"tcpnet.h"


class TcpMediator:public INetMediator
{
    Q_OBJECT
public:
    TcpMediator();
    ~TcpMediator();
    //打开网络
    bool OpenNet(long ip);
    //发送数据
    bool SendData(long ISendIp, char* buf, int nLen);
    //关闭网络
    void CloseNet();
    //接收数据
    void DealData(long ISendIp, char* buf, int nLen);

signals:
    //把接收到的数据发送给kernel
    void SIG_ReadTcpData(char* buf, int nLen );
};

#endif // TCPMEDIATOR_H
