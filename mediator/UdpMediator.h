#pragma once
#include"INetMediator.h"

class UdpMediator :public INetMediator
{
   



    //把接收到的数据发送给kernel
    void SIG_ReadData(long lSendIp , char* buf , int nLen);

public:
	UdpMediator(CKernel* kernel);
	~UdpMediator();
	//打开网络
    bool OpenNet();
	//发送数据
	bool SendData(long ISendIp, char* buf, int nLen);
	//关闭网络
	void CloseNet();
	//接收数据
	void DealData(long ISendIp, char* buf, int nLen);
};
