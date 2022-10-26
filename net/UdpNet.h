#pragma once
#include "INet.h"
#include<Windows.h>
#include<winsock.h>
#include<iostream>
#include<list>

class UdpNet :public INet
{
public:
	UdpNet(INetMediator* pMediator);
	~UdpNet();
	//初始化网络
    bool InitNet(long ip);
	//发送数据
	bool SendData(long ISendlp, char* buf, int nLen);
	//关闭网络
	void UninitNet();

	//typedef struct RecvInfo {
	//	long ISendIp;
	//	char* buf;
	//	int nLen;
	//};
	//static std::list<RecvInfo*> RecvInfoLst;
	//static SOCKET sock;


protected:
	//接收数据
	void RecvData();
	//线程函数，接收数据
	 static unsigned __stdcall RecvThread(void* IpVoid);
	 /*
	 类的静态函数不属于该类的任何一个对象，而是属于类本身，所以不受对象局部变量的影响，在运行时可以直接调用类的静态函数，从而启动线程！

相反，如果一个类的一个局部对象的生命周期结束了，难道线程函数就不能用了吗？



其实类的静态函数就跟全局函数是一个样子的, 只是调用的时候要加下个类修饰符而以.
至于为什么不能是非静态成员函数呢, 因为非静态成员函数都会在参数列表中加上一个this指针为为参数, 这样的话你写的线程函数就不符合调用规定了.
比如 DWORD WINAPI ThreadFun(LPVOID); 是非静态的,实际编译后,就会变成
DWORD WINAPI ThreadFun(LPVOID, CMyClass *this); 
这个函数就明显不能作为线程的函数了, 因为多了个参数.所以编译就过不了了.
	 */
	SOCKET m_sock;
	HANDLE m_handle;
	bool m_isStop;
    sockaddr_in addr;
    char recvBuf[60*1024 ];
    sockaddr_in addrFrom;
    int addrFromSize;

};

//SOCKET  UdpNet::sock = 0;
//std::list<UdpNet::RecvInfo*> UdpNet::RecvInfoLst;
