#include"UdpNet.h"
#include"UdpMediator.h"
#include"packDef.h"
#include<process.h>
#include<QDebug>

//导入网络函数库
//#pragma comment(lib,"Ws2_32.lib")

UdpNet::UdpNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_handle(0),m_isStop(false)
{
	m_pNediator = pMediator;
    memset(recvBuf,0,50*1024);
    addrFromSize=sizeof(sockaddr_in);

}




UdpNet::~UdpNet() {
	UninitNet();
}

//初始化网络，加载库，创建套接字，绑定ip地址，创建一个线程（接收数据）
bool UdpNet::InitNet(long ip)
{
	

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


	//2:雇人------创建套接字
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_sock)
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;

		return false;
	}
	else {
		std::cout << "socket success." << std::endl;
	}

    addr.sin_family = AF_INET;
//    addr.sin_port = htons(15725);  //将整型变量从主机字节序变成网络字节序
//    QString ip_=QString("103.46.128.46");
        addr.sin_port = htons(53421);  //将整型变量从主机字节序变成网络字节序
        QString ip_=QString _DEF_SERVER_IP;
    addr.sin_addr.S_un.S_addr = inet_addr((ip_.toStdString().c_str())) ;   //将ip地址固定为服务器的ip地址
//	//申请广播权限
//	BOOL bval = TRUE;
//	setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&bval, sizeof(bval));




//	//3.选地址-----绑定ip地址
//	sockaddr_in sockAddr;
//	sockAddr.sin_family = AF_INET;
//	sockAddr.sin_port = htons(_DEF_UDP_PORT);        //将整型变量从主机字节序变成网络字节序
//    sockAddr.sin_addr.S_un.S_addr = INADDR_ANY;
//     err = bind(m_sock, (sockaddr*)&sockAddr, sizeof(sockAddr));
//    if (SOCKET_ERROR == err) {
//        std::cout << "bind error: " << WSAGetLastError() << std::endl;
//        return false;
//    }
//    else
//    {
//        std::cout << "bind success. " << std::endl;
//    }

	//==================================================================
	//4:创建线程接收数据
	//CreateThread对应的结束线程的函数是ExitThread.
	//ExitThread在结束线程的时候，如果线程里面有c++运行时库的函数，比如strcpy，这类函数在执行的时候会申请空间，但不回收空间，
	//ExitThread在退出的时候也不会帮忙回收申请的空间，这样就会造成内存泄漏。
	//beginthreadex对应的结束线程的函数是_endthreadex。_endthreadex在结束线程的时候，会先回收线程中申请的空间，在调用ExitThread结束线程。

	m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, (void*)this, 0, NULL);

	return true;
}


//线程函数：调用接收数据的函数RecvData
unsigned __stdcall UdpNet::RecvThread(void* IpVoid)
{
	UdpNet* pThis = (UdpNet*)IpVoid;
	pThis->RecvData();
	return 1;
}



//发送数据
bool UdpNet::SendData(long ISendIp, char* buf, int nLen)
{

    qDebug()<<nLen;
    qDebug()<<__func__<<"aaaaaaaaa";


//    QString ip=QString("103.46.128.53");

	if (sendto(m_sock, buf, nLen, 0, (sockaddr*)&addr, sizeof(addr)) <= 0) {
		cout << "UdpNet::SendData" << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

//关闭网络
void UdpNet::UninitNet()
{
	//1:关闭线程
	m_isStop = true;
	if (m_handle) {
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100)) {
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = NULL;
	}
	//2：关闭socket
	if (m_sock && INVALID_SOCKET != m_sock) {
		closesocket(m_sock);
	}

	//3:卸载库
	WSACleanup();
}

//接收数据
void UdpNet::RecvData()
{


	int nRecvNum = 0;


	while (!m_isStop) {
        memcpy(recvBuf,"0",1);
        nRecvNum = recvfrom(m_sock, recvBuf, 60*1024 , 0, (sockaddr*)&addrFrom, &addrFromSize);
		if (nRecvNum > 0) {
            qDebug()<<"接收到数据了";
            qDebug()<<inet_ntoa(addrFrom.sin_addr);
			//接收到数据，new一个新空间，用来保存数据，防止recvBuf被再次写入，上一次接收到的数据就没有了
			char* packBuf = new char[nRecvNum];
			memcpy(packBuf, recvBuf, nRecvNum);
			//传给中介者
			if (m_pNediator) {

				m_pNediator->DealData(addrFrom.sin_addr.S_un.S_addr, packBuf, nRecvNum);
				//TODO：回收空间delete[]----kernel知道什么时候使用完数据，再回收
			}
		}
        else{

        }
	}



}


