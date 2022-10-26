#include"UdpNet.h"
#include"UdpMediator.h"
#include"packDef.h"
#include<process.h>


//导入网络函数库
//#pragma comment(lib,"Ws2_32.lib")

UdpNet::UdpNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_handle(0),m_isStop(false)
{
	m_pNediator = pMediator;


}




UdpNet::~UdpNet() {
	UninitNet();
}

//初始化网络，加载库，创建套接字，绑定ip地址，创建一个线程（接收数据）
bool UdpNet::InitNet()
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

//	//申请广播权限
//	BOOL bval = TRUE;
//	setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&bval, sizeof(bval));




	//3.选地址-----绑定ip地址
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(_DEF_UDP_PORT);        //将整型变量从主机字节序变成网络字节序
    sockAddr.sin_addr.S_un.S_addr = inet_addr _DEF_SERVER_IP;
     err = bind(m_sock, (sockaddr*)&sockAddr, sizeof(sockAddr));
    if (SOCKET_ERROR == err) {
        std::cout << "bind error: " << WSAGetLastError() << std::endl;
        return false;
    }
    else
    {
        std::cout << "bind success. " << std::endl;
    }

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
bool UdpNet::SendData(long lSendIp, char* buf, int nLen)
{
	if (m_mapIdToSockAddr_in.count(lSendIp) > 0)
	{
		sockaddr_in addr = m_mapIdToSockAddr_in[lSendIp];

		//        QString ip=QString("192.168.1.100");
			//addr.sin_addr.S_un.S_addr = inet_addr((ip.c_str())) ;   //将ip地址固定为服务器的ip地址
		if (sendto(m_sock, buf, nLen, 0, (sockaddr*)&addr, sizeof(addr)) <= 0) {
			cout << "UdpNet::SendData" << WSAGetLastError() << endl;
			return false;
		}
		return true;
	}
	cout << " 无此人 " << endl;
	return false;


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
    char recvBuf[10 *1024 ] = "";
	sockaddr_in addrFrom;
	int addrFromSize = sizeof(addrFrom);
	while (!m_isStop) {
		nRecvNum = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addrFrom, &addrFromSize);
		if (nRecvNum > 0) {
			//接收到数据，new一个新空间，用来保存数据，防止recvBuf被再次写入，上一次接收到的数据就没有了
			char* packBuf = new char[nRecvNum];
			memcpy(packBuf, recvBuf, nRecvNum);

			if (_DEF_PROTOCOL_ONLINE_RQ == *(int*)recvBuf || _DEF_PROTOCOL_LIVE == *(int*)recvBuf)   //如果是上线请求包或者是生命包
			{
				m_mapIdToSockAddr_in[*((int*)recvBuf+1)] = addrFrom;  //保存地址信息
				m_pNediator->DealData(*((int*)recvBuf + 1), packBuf, nRecvNum);
				continue;
		
			}
			//传给中介者

				if (m_pNediator) {
					m_pNediator->DealData(addrFrom.sin_addr.S_un.S_addr, packBuf, nRecvNum);
					//TODO：回收空间delete[]----kernel知道什么时候使用完数据，再回收
				}
			
		}
	}



}


