#include "tcpServerMediator.h"
#include "CKernel.h"

TcpServerMediator::TcpServerMediator(CKernel* kernel)
{
    m_pNet = new tcpServer(this);   //结合UdpNet的构造函数来看
    p_Kernel = kernel;

}

TcpServerMediator::~TcpServerMediator()
{

}

//打开网络：建立连接
bool TcpServerMediator::OpenNet()
{
    if (m_pNet->InitNet())
    {
        return true;
    }
    return false;
}

//发送数据
bool TcpServerMediator::SendData(long ISendIp, char* buf, int nLen)
{
    if (m_pNet->SendData(ISendIp, buf, nLen))
        return true;

    return false;
}

//关闭网络
void TcpServerMediator::CloseNet()
{
    m_pNet->UninitNet();
}

//接收数据
void TcpServerMediator::DealData(long ISendIp, char* buf, int nLen)
{
    //TODO:通过信号把接收到的数据发给ckernel
    cout << "TcpServerMediator::DealData " << buf << endl;
    //测试代码
    //SendData(ISendIp, buf, nLen);
//Q_EMIT SIG_ReadData(0,buf,0);

    //把接受到的数据发给kernel
    p_Kernel->dealReadyData(ISendIp, buf, nLen);
}
