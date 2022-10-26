#pragma once

class CKernel; //告诉编译器有这个类，但是还未编译
class INet;//告诉编译器有这个类，但是还未编译
class INetMediator
{
   
public:
    INetMediator();
    virtual ~INetMediator();
	//打开网络
    virtual bool OpenNet()=0;
	//发送数据
	virtual bool SendData(long ISendIp, char* buf, int nLen) = 0;
	//关闭网络、
	virtual void CloseNet() = 0;
	//接收数据
	virtual void DealData(long ISendIp, char* buf, int nLen) = 0;

protected:
	INet* m_pNet;    //网络接口类指针
	CKernel* p_Kernel;
};
