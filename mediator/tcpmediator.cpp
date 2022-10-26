#include "tcpmediator.h"

TcpMediator::TcpMediator()
{
    m_pNet = new TcpNet(this);   //结合UdpNet的构造函数来看
}

TcpMediator::~TcpMediator()
{

}

//打开网络：建立连接
bool TcpMediator::OpenNet(long ip)
{
   if(m_pNet->InitNet(ip))
   {
       return true;
   }
    return false;
}

//发送数据
bool TcpMediator::SendData(long ISendIp, char *buf, int nLen)
{
    if(m_pNet->SendData(ISendIp,buf,nLen))
        return true;

    return false;
}

//关闭网络
void TcpMediator::CloseNet()
{
    m_pNet->UninitNet();
}

//接收数据
void TcpMediator::DealData(long ISendIp, char *buf, int nLen)
{
           //TODO:通过信号把接收到的数据发给ckernel
       Q_EMIT SIG_ReadTcpData(buf,nLen);
}
