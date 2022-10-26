#pragma once
#include"tcpServerMediator.h"
#include "CMySql.h"
#include "pacKDef.h"
#include "INetMediator.h"
#include <iostream>
using namespace std;

//���庯��ָ��
class CKernel;
typedef void (CKernel::* PFUN)(long lSendIp, char* buf, int nLen);

class CKernel
{
public:
	CKernel();
	~CKernel();
	//===========TCP����================================================================================================================
	//�򿪷�����
	 bool startServer();
	//�رշ�����
	void closeServer();
	//��ʼ��Э��ӳ���
	void setProtocolMap();
	//��ȡ�û��ĺ�����Ϣ�������Լ���
	void getUserFriendInfo(int userId);
	//����id��ȡ�û���Ϣ
	void getUserInfoById(STRU_TCP_FRIEND_INFO* info, int id);
	//�������н��յ�������
	void dealReadyData(long ISendIp, char* buf, int nLen);
	//����ע������
	 void dealRegisterRq(long lSendIp, char* buf, int nLen);
	 //�����¼����
	 void dealLoginRq(long lSendIp, char* buf, int nLen);
	 //������������
	 void dealChatRq(long lSendIp, char* buf, int nLen);  //����������udp�еġ��˺�������
	 //������������
	 void dealoffLineRq(long lSendIp, char* buf, int nLen);
	 //������Ӻ�������
	 void dealaddFriendRq(long lSendIp, char* buf, int nLen);
	 //������Ӻ��ѻظ�
	 void dealaddFriendRs(long lSendIp, char* buf, int nLen);
	 //�����ļ���Ϣ����/�ظ�
	 void dealFileInfoRq_Rs(long ISendIp, char* buf, int nLen);
	 //������Ƶͨ������
	 void dealVideoRq(long ISendIp, char* buf, int nLen);
	 //������Ƶͨ���ظ�
	 void dealVideoRs(long ISendIp, char* buf, int nLen);
	 //��������ͨ������
	 void dealVoidRq(long ISendIp, char* buf, int nLen);
	 //��������ͨ���ظ�
	 void dealVoidRs(long ISendIp, char* buf, int nLen);
	 //����Ҷ�����ͨ������
	 void dealCloseVoid(long ISendIp, char* buf, int nLen);
	 //�����鼮�ϼ����루���յ����鼮ͼƬ��Ϣ��
	 void dealBooksImgInfo(long ISendIp, char* buf, int nLen);
	 //�����鼮�ϼ����루���յ����鼮������Ϣ��
	 void dealBooksInfo(long ISendIp, char* buf, int nLen);
	 //�����յ��Ļ�ȡ�鼮��Ϣ������
	 void dealBooksInfoRq(long ISendIp, char* buf, int nLen); 
	 //�����յ��Ļ�ȡĳ�����ϼ�������Ϣ������
	 void dealAllBooksInfoRq(long ISendIp, char* buf, int nLen);
	 //�����¼�Ŀ���鼮��Ϣ������
	 void dealDeleteBooksInfoRq(long ISendIp, char* buf, int nLen);
	 //�����յ��ĸ���ͷ�������
	 void dealAlterIconRq(long ISendIp, char* buf, int nLen);
	 //==============UDP����===========================================================================================================================
	 //�������н��յ���udp��Ϣ��
	 void dealReadyUdpData(long ISendIp, char* buf, int nLen);
	 //����udp�������� ����ΪҪ�洢udp��ַ�����Դ˺��������У�
	 void dealUdpOnlineRq(long ISendIp, char* buf, int nLen);
	 //������������
	 void dealUdpChatRq(long ISendIp, char* buf, int nLen);
	 //��������������
	 void dealLifeRq(long ISendIp, char* buf, int nLen);
	 //������Ƶͨ����Ϣ/*���߳��д���*/
	 void dealVideoInfo(long ISendIp, char* buf, int nLen);
	 //��������ͨ����Ϣ��
	 void dealVoidInfo(long ISendIp, char* buf, int nLen);

private:
	CMySql sql;
	//tcp�н���
	INetMediator* p_pMediator;
	//udp�н���
	INetMediator* m_pUdpMediator;
	//����Э��ӳ������
	PFUN m_netProtocolMap[_DEF_PROTOCOL_COUNT];
	//�����û�id�Ͷ�Ӧ��socket
	map<int, SOCKET> m_mapIdToSocket;
	//�����û�id��Ӧ��ip��ַ��UDP��
	map<int, long> m_mapIdToSockAddr_in;   //��Ϊ���������������������tcp��ʵ�ֵģ����Դ˳�ԱӦͬʱ�����Tcp���ֺ���ʹ��
	
};

