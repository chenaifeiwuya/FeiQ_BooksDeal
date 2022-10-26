#include "tcpclientmediator.h"

TcpMediator::TcpMediator()
{
    m_pNet = new TcpNet(this);   //结合Tcpnet的构造函数来看
}

TcpMediator::~TcpMediator()
{

}

//打开网络：建立连接
bool TcpMediator::OpenNet()
{
   if(m_pNet->InitNet())
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
    cout << "TcpMediator::DealData " << buf << endl;
       //Q_EMIT SIG_ReadData(0,buf,0);
}
