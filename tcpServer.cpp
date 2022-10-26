#include "tcpServer.h"
#include<process.h>

tcpServer::tcpServer(INetMediator* pMediator)
{
	m_pNediator = pMediator;
}

tcpServer::~tcpServer()
{

}

bool tcpServer::InitNet()
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

    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  //创建套接字
    if (INVALID_SOCKET == m_sock) {
        std::cout << "socket error:" << WSAGetLastError() << std::endl;

        return false;
    }
    else {
        cout << "socket success" << endl;
    }

    //3：绑定ip地址
    //定义服务器地址信息
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_DEF_TCP_PORT);
    serverAddr.sin_addr.S_un.S_addr = inet_addr _DEF_SERVER_IP;

    err = bind(m_sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (SOCKET_ERROR == err) {
        cout << "bind error:" << WSAGetLastError() << endl;
        closesocket(m_sock);
        WSACleanup();
        return false;
    }
    else {
        cout << "bind success" << endl;
    }

    //4:监听
    err = listen(m_sock, 10);  //10为挂起连接队列的最大长度(监听有无新连接请求,当连接请求量超过系统处理量则先挂起)
    if (SOCKET_ERROR == err) {
        cout << "listen error:" << WSAGetLastError() << endl;
        closesocket(m_sock);
        WSACleanup();
        return false;
    }
    else
    {
        cout << "listen success" << endl;
    }

    //5:创建一个接受连接的线程
    m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvConnectThread,(void*)this, 0, NULL);
    return true;

}


bool tcpServer::SendData(long ISendIp, char* buf, int nLen)
{
    // Tcp是面向字节流的，所以会产生粘包问题：
    //1：判断输入参数合法性
    if (!buf || nLen <= 0) {
        cout << "参数不合法 " << endl;
        return false;
    }

    //2：发送数据包长度
    if (send(ISendIp, (char*)&nLen, sizeof(int), 0) <= 0) {   //ISendIp使sockt
        cout << "TcpServerNet::SendData error: " << WSAGetLastError() << endl;
        return false;
    }

    //3：发送包内容
    if (send(ISendIp, buf, nLen, 0) <= 0) {
        cout << "TcpServerNet::SendData error: " << WSAGetLastError() << endl;
        return false;
    }

    return true;
}

//关闭网络
void tcpServer::UninitNet()
{
    //1:关闭线程：设置退出标志位为true，等一段时间等线程自己退出，关闭句柄
    m_isStop = true;
    if (m_handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100)) {
            TerminateThread(m_handle, -1);
        }
        CloseHandle(m_handle);
        m_handle = NULL;
    }

    for (auto ite = m_mapSocketToHandle.begin(); ite != m_mapSocketToHandle.end();) {
        HANDLE handle = ite->second;
        if (handle) {
            if (WAIT_TIMEOUT == WaitForSingleObject(handle, 100)) {
                TerminateThread(handle, -1);
            }
            CloseHandle(handle);
            handle = NULL;
        }
        //移除失效的map节点
        ite = m_mapSocketToHandle.erase(ite);
    }

    //2:关闭socket
    if (m_sock && INVALID_SOCKET != m_sock) {
        closesocket(m_sock);
    }

    for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();)
    {
        SOCKET sock = ite->second;
        if (sock && INVALID_SOCKET != sock) {
            closesocket(sock);
        }
        //移除失效的map节点
        ite = m_mapThreadIdToSocket.erase(ite);
    }

    //3:卸载库
    WSACleanup();
}

//线程函数：调用接收数据的函数RecvData
unsigned __stdcall tcpServer::RecvThread(void* IpVoid)
{
    tcpServer* pThis = (tcpServer*)IpVoid;
    pThis->RecvData();
    return 1;
}


//接收数据
void tcpServer::RecvData()
{
    cout << " tcpServer::RecvData " << endl;
    //1:睡一会儿，因为线程是创建即运行，但是把socket存入map中需要一点时间，所以需要线程等一会儿再取socket
    Sleep(10);
    //2:取出socket
    SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];  //GetCurrentThreadId()函数可以得到函数所在线程中的线程id
    //3:判断socket合法性
    if (!sock && INVALID_SOCKET == sock) {
        cout << "tcpServer::RecvData socket error " << endl;
        return;
    }
    //4:接收数据
  
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
        nRecvNum = recv(sock, (char*)&nPackSize, sizeof(int), 0);
        if (nRecvNum > 0) {
            //接收包内容
            char* packBuf = new char[nPackSize];
            offset = 0;
            while (nPackSize > 0) {
                nRecvNum = recv(sock, packBuf + offset, nPackSize, 0);
                if (nRecvNum > 0) {
                    offset += nRecvNum;
                    nPackSize -= nRecvNum;
                }
            }
            //传给中介者
            if (m_pNediator) {
                m_pNediator->DealData(sock, packBuf, offset);
                //TODO:回收空间 delete[] -- kernel知道什么时候使用完数据，再回收
            }
        }
        else if (0 == nRecvNum || 10054 == WSAGetLastError()) {
            cout << "TcpServerNet::RecvData close connection" << endl;
            break;
        }
        else {
            cout << "TcpServerNet::RecvData error" << WSAGetLastError() << endl;
        }
    }
}

//线程函数：接收客户端的连接
unsigned __stdcall tcpServer::RecvConnectThread(void* IpVoid)
{
    cout << " tcpServer::RecvConnectThread " << endl;
    tcpServer* pThis = (tcpServer*)IpVoid;
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    unsigned int threadId = 0;
    while (!pThis->m_isStop)
    {
        cout << "TcpServerNet::AcceptThread accept" << endl;
        //接收连接
        SOCKET sockWaiter = accept(pThis->m_sock, (sockaddr*)&clientAddr, &clientAddrSize);
        cout << "threadId: " << threadId << endl;
        //创建一个接收数据的线程
        HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, (void*)pThis, 0, &threadId);
        //保存socket到map， key是线程id
        pThis->m_mapThreadIdToSocket[threadId] = sockWaiter;
        //保存线程句柄
        pThis->m_mapSocketToHandle[sockWaiter] = handle;
    }
	return 1;
}

