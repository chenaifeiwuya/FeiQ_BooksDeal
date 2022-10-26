#include "tcpnet.h"
#include"tcpmediator.h"
#include<process.h>




TcpNet::TcpNet(INetMediator *pMediator)
{
   m_pNediator = pMediator;
//    INetMediator *pMediator;

}

TcpNet::~TcpNet()
{

}

//初始化网络

bool TcpNet::InitNet(long ISendlp)
{
qDebug()<<__func__;
//1:选项目-----加载库
WORD wVersionRequested;
WSADATA wsaData;  //WSADATA，一种数据结构。
//这个结构被用来存储被WSAStartup函数调用后返回的Windows Sockets数据。它包含Winsock.dll执行的数据。
int err;
wVersionRequested = MAKEWORD(2, 2);
err = WSAStartup(wVersionRequested, &wsaData);
if (err != 0)
{
    /*
    Tell the user that we could not find a suable
    */

    printf("WSAStartup failed with error: %d\n", err);
    return false;
}

if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
    /*
    Tell the user that we could not find a usable
    WinSock DLL.
    */
    printf("Could not find a usable version of Winsock.dll\n");

    return false;

}
else {
    printf("The Winsock 2.2 dll was found okay\n");
}

 //创建套接字
    m_sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == m_sock) {
        std::cout << "socket error:" << WSAGetLastError() <<std::endl;
        return false;
    }
    else {
        cout << "socket success" << endl;
    }

    //====================================================================
    //定义服务端地址信息
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(53421);
    serverAddr.sin_addr.S_un.S_addr = inet_addr _DEF_SERVER_IP;
//    serverAddr.sin_port = htons(24192);
//    serverAddr.sin_addr.S_un.S_addr = inet_addr("103.46.128.46");


    //建立连接：
     err=connect(m_sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if(SOCKET_ERROR==err)
     {
         cout << "SOCKET_ERROR:" << WSAGetLastError() << endl;
         closesocket(m_sock);
         return false;
     }
    else {
        cout << "connect success!" << endl;
    }


    m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, (void*)this, 0, NULL);

    return true;


}

//发送数据
bool TcpNet::SendData( long ISendIp, char* buf, int nLen)  //先发包大小，再发包内容
{

qDebug()<<__func__<<"pppppppppppppp";

//    //发送文件块
//    STRU_FILE_BLOCK_RQ blockRq;
//    long long nPos = 0;  //表示文件读到哪个位置了
//    int nReadLen = 0;
//    while(true){
//        //读文件发送
//        /*参数：
//         *1：缓冲区 2：一次读多少 3：读多少次  4：文件指针
//        */
//        //：返回值：成功读多少次，用于反应读出多少字节
//        nReadLen = fread(blockRq.szFileContent,1,_DEF_FILE_CONTENT_SIZE,pFile);
//        blockRq.nBlockSize = nReadLen;  //文件块大小


//        //触发条件退出
//        if(nReadLen < _DEF_FILE_CONTENT_SIZE)
//        {
//            fclose(pFile);
//            break;
//        }
//        Sleep(1);

//    }

//    //关闭套接字=================================
//    //关闭套接字
//        if (sock && INVALID_SOCKET != sock)
//        {
//            closesocket(sock);
//        }
        int nSendNum = 0;
        //先发包大小
        nSendNum = send(m_sock, (char*)&nLen, sizeof(int), 0);
        if (SOCKET_ERROR == nSendNum) {
            cout << "send error:" << WSAGetLastError() << endl;
                return false;
        }
        //再发包内容
    nSendNum = send(m_sock, buf, nLen, 0);
    qDebug()<<nLen;
    if (SOCKET_ERROR == nSendNum) {
        cout << "send error:" << WSAGetLastError() << endl;
            return false;
    }

    return true;
}

//关闭网络
void TcpNet::UninitNet()
{
    qDebug()<<__func__;
    //关闭套接字
    if (m_sock && INVALID_SOCKET != m_sock)
    {
        closesocket(m_sock);
    }

}

//TCP接收数据
void TcpNet::RecvData()
{
    qDebug()<<__func__;

    //收数据
    //TCP会产生粘包问题，所以采用先接收包大小，再接收包内容的方法
    //UDP不会产生粘包问题
   int nPackSize = 0;
   int offset = 0;
    while(!m_isStop)
    {

           int nRecvNum = recv(m_sock, (char*)&nPackSize, sizeof(int), 0); //先接收包大小，再接收包数据
            if (nRecvNum > 0) {


                char* packBuf=new char[nPackSize];
                int nLen = nPackSize;
                //再接收包内容
                while( nPackSize > 0)
                {
                      int nRecvNum = recv(m_sock,packBuf, nPackSize, 0);  //接收包内容
                                 qDebug()<<"收到数据啦，数据大小为："<<nRecvNum;
                      nPackSize -= nRecvNum;   //一直减直到npacksize等于0了就证明这个包接收完了
                }

    //            char* packBuf = new char[nRecvNum];
    //            memcpy(packBuf, recvBuf, nRecvNum);

                //通过中介者类将recvBuf转给ckernel

                m_pNediator->DealData(0, packBuf, nLen);

            }
            else if (0 == nRecvNum) {
                cout << "connect close" << endl;
                break;
            }
            else
            {
                cout << "recv error:" << WSAGetLastError() << endl;
                break;
            }

    }

}

//接收数据线程函数
unsigned TcpNet::RecvThread(void *IpVoid)
{
    qDebug()<<__func__;
    TcpNet* pThis=(TcpNet*)IpVoid;
    pThis->RecvData();
    return 1;
}
