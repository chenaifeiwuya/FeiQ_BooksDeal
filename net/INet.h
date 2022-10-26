#pragma once
#include<iostream>
#include<winsock.h>
#include<set>

using namespace std;

class INetMediator;   //先告诉编译器有这个类，但是没有编译
class INet
{
public:
	INet() {}
	virtual ~INet(){}

	//初始化网络
    virtual bool InitNet(long ip) = 0;
	//发送数据
	 virtual bool SendData(long ISendIp, char* buf, int nLend=0)=0;

	//关闭网络
	virtual void UninitNet() = 0;


    //把long类型的ip地址转换成string类型的ip地址
    static std::string GetIpString(long ip)
    {
        sockaddr_in sockAddr;
        sockAddr.sin_addr.S_un.S_addr = ip;
        return inet_ntoa(sockAddr.sin_addr);  //inet_ntoa()将网络地址转换成“.”点隔的字符串格式
    }
    //char*, std::string, QString (char*是基础数据类型，std::string和QString是封装好的类，基础数据类型可以给类直接赋值）
    //std::string = char*, QString = char*
    //std::string 调用c_str()转换成char*
    //QString 可以调用toStdString转换成std::string

    //获取本地IP地址列表
    static set<long> GetValidIpList()
    {
        set<long> setIp;
        long ip = 0;
        int i=0;
        //获取主机名字
        char nameBuf[100]="";
        gethostname(nameBuf, 100);
        //2:根据主机名获取ip地址列表
        struct hostent *remoteHost = gethostbyname(nameBuf);
        while(remoteHost->h_addr_list[i] != 0){
            ip = *(u_long *) remoteHost->h_addr_list[i++];
            setIp.insert(ip);
        }
        return setIp;
    }

protected:
	//接收数据
	virtual void RecvData() = 0;
	INetMediator* m_pNediator;   //中介者类指针，父类指针指向子类接口

};
