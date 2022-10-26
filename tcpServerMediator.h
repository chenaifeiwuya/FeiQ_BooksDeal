#pragma once
#include"INetMediator.h"
#include"tcpServer.h"
    class TcpServerMediator :public INetMediator
    {

    public:
        TcpServerMediator(CKernel* kernel);
        ~TcpServerMediator();
        //打开网络
        bool OpenNet();
        //发送数据
        bool SendData(long ISendIp, char* buf, int nLen);
        //关闭网络
        void CloseNet();
        //接收数据
        void DealData(long ISendIp, char* buf, int nLen);


      

    };



