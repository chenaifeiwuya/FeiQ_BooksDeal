#include<iostream>
#include"tcpServerMediator.h"
#include"./mediator/tcpclientmediator.h"
#include"CKernel.h"
using namespace std;

int main()
{
	////�򿪷����
	//INetMediator* m_server = new TcpServerMediator;
	//if (!m_server->OpenNet()) {
	//	cout << "�򿪷����ʧ�� " << endl;
	//	system("pause");
	//	return 1;
	//}

	//�򿪷����
	CKernel kernel;
	if (!kernel.startServer()) {
		cout << "�򿪷�����ʧ��" << endl;
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
	//�򿪿ͻ���
	//INetMediator* m_client = new TcpMediator;
	//if (!m_client->OpenNet())
	//{
	//	cout << "�򿪿ͻ���ʧ��" << endl;
	//	system("pause");
	//	return 1;
	//}

	//�ͻ��˸�����˷���һ���ַ���
	//m_client->SendData(0, "hello world", strlen("hello world") + 1);


	system("pause");
	return 0;
}