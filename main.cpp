#include<iostream>
#include"tcpServerMediator.h"
#include"./mediator/tcpclientmediator.h"
#include"CKernel.h"
using namespace std;

int main()
{
	////打开服务端
	//INetMediator* m_server = new TcpServerMediator;
	//if (!m_server->OpenNet()) {
	//	cout << "打开服务端失败 " << endl;
	//	system("pause");
	//	return 1;
	//}

	//打开服务端
	CKernel kernel;
	if (!kernel.startServer()) {
		cout << "打开服务器失败" << endl;
	}
	else {
		while (1) {
			Sleep(5000);
			cout << "server is running" << endl;
		}
	}

	//while (1) {
	//	Sleep(5000);
	//	cout << "Server is running"<<endl;
	//}
	//打开客户端
	//INetMediator* m_client = new TcpMediator;
	//if (!m_client->OpenNet())
	//{
	//	cout << "打开客户端失败" << endl;
	//	system("pause");
	//	return 1;
	//}

	//客户端给服务端发送一个字符串
	//m_client->SendData(0, "hello world", strlen("hello world") + 1);


	system("pause");
	return 0;
}