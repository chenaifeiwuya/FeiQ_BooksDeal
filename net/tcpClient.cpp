#include "tcpclient.h"
#include"tcpclientmediator.h"
#include<process.h>




TcpNet::TcpNet(INetMediator *pMediator):m_sock(INVALID_SOCKET), m_handle(0), m_isStop(false)
{
   m_pNediator = pMediator;
//    INetMediator *pMediator;

}

TcpNet::~TcpNet()
{
    UninitNet();
}

//初始化网络:加载库，创建套接字，连接服务器，创建接收数据的线程

bool TcpNet::InitNet()
{
    //1：选项目--加载库
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        return false;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("Could not find a usable version of Winsock.dll\n");
        return false;
    }
    else {
        printf("The Winsock 2.2 dll was found okay\n");
    }

    //2:雇人--创建套接字

    m_sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  //创建套接字
    if (INVALID_SOCKET == m_sock) {
        std::cout << "socket error:" << WSAGetLastError() <<std::endl;

        return false;
    }
    else {
        cout << "socket success" << endl;
    }

    //3：连接服务器
    //定义服务器地址信息
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_DEF_TCP_PORT);
    serverAddr.sin_addr.S_un.S_addr = inet_addr _DEF_SERVER_IP;


    //建立连接：
    err=connect(m_sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if(SOCKET_ERROR==err)
     {
         cout << "SOCKET_ERROR:" << WSAGetLastError() << endl;
         closesocket(m_sock);
         WSACleanup();
         return false;
     }
    else {
        cout << "connect success!" << endl;
    }


    //创建接收数据的线程


    //线程函数：
    //1：参数1是安全等级，NULL就是默认安全等级
    //2：参数2是堆栈大小，0代表使用默认堆栈大小，为 1M
    //3:线程执行函数
    //4：参数4是线程执行函数的参数
    m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, (void*)this, 0, NULL);

    return true;


}

//线程函数: 调用接收数据的函数RecvData
unsigned TcpNet::RecvThread(void* IpVoid)
{

    TcpNet* pThis = (TcpNet*)IpVoid;
    pThis->RecvData();
    return 1;
}

//发送数据
bool TcpNet::SendData( long ISendIp, char* buf, int nLen)
{
   // Tcp是面向字节流的，所以会产生粘包问题：
    //1：判断输入参数合法性
    if (!buf || nLen <= 0) {
        cout << "参数不合法 " << endl;
        return false;
    }

    //2：发送数据包长度
    if (send(m_sock, (char*)&nLen, sizeof(int), 0) <= 0) {
        cout << "TcpClientNet::SendData error: " << WSAGetLastError() << endl;
        return false;
    }

    //3：发送包内容
    if (send(m_sock, buf, nLen, 0) <= 0) {
        cout << "TcpClientNet::SendData error: " << WSAGetLastError() << endl;
        return false;
    }

    return true;


}

//关闭网络
void TcpNet::UninitNet()
{
     //1:关闭线程：设置退出标志位为true， 等一段时间等线程自己退出，关闭句柄
    m_isStop = true;
    if (m_handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100)) {
            TerminateThread(m_handle, -1);
        }
        CloseHandle(m_handle);
        m_handle = NULL;
    }
    //关闭socket
    if (m_sock && INVALID_SOCKET != m_sock) {
        closesocket(m_sock);
    }
    //3:卸载库
    WSACleanup();
}

//TCP接收数据
void TcpNet::RecvData()
{
    //因为Tcp是面向字节流的，所以会有粘包问题。
    //解决方法：
    //1：消息定长
    //2：在在发送包前面先发送包大小
    //3：短连接
    //4：在包头或者结尾处设置标志位

    //切记offset清零！！！！！
    int nRecvNum = 0;
    int nPackSize = 0;
    int offset = 0;
    while (!m_isStop) {
        //先接收包的大小
        nRecvNum = recv(m_sock, (char*)&nPackSize, sizeof(int), 0);
        if (nRecvNum > 0) {
            //接收包内容
            char* packBuf = new char[nPackSize];
            offset = 0;
            while (nPackSize > 0) {
                nRecvNum = recv(m_sock, packBuf + offset, nPackSize, 0);
                if (nRecvNum > 0 ) {
                    offset += nRecvNum;
                    nPackSize -= nRecvNum;
                }
            }
            //传给中介者
            if (m_pNediator) {
                m_pNediator->DealData(m_sock, packBuf, offset);
                //TODO:回收空间 delete[] -- kernel知道什么时候使用完数据，再回收
            }
        }
    }

}


