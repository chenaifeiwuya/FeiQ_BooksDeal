#include "CKernel.h"
#include "tcpServerMediator.h"
#include "UdpMediator.h"
#include<list>
//#include <unistd.h>
//#include <sys/stat.h>
//#include <fcntl.h>


//������������±�ĺ�
#define NetProFunMap(a) m_netProtocolMap[a - (_DEF_PROTOCOL_BASE + 1)]

CKernel::CKernel()
{

	setProtocolMap();
	p_pMediator = new TcpServerMediator(this);
	m_pUdpMediator = new UdpMediator(this);

}
CKernel::~CKernel() {
	closeServer();
}

//�򿪷�����
bool CKernel::startServer()
{
	cout << "CKernel::startServer" << endl;
	//1;�������ݿ�
	if (!sql.ConnectMySql("127.0.0.1", "root", "mmm959825", "0103sql")) {
		cout << "�������ݿ�ʧ��" << endl;
		return false;
	}
	//2:������
	if (!p_pMediator->OpenNet()) {
		cout << "������ʧ��" << endl;
		return false;
	}
	if (!m_pUdpMediator->OpenNet()) {
		cout << "������ʧ��" << endl;
		return false;
	}
	return true;
}

//�رշ�����
void CKernel::closeServer()
{
	cout << "CKernel::closeServer" << endl;
	//1:�Ͽ����ݿ�����
	sql.DisConnect();
	//2:�ر����磬���ն���
	if (p_pMediator) {
		p_pMediator->CloseNet();
		delete p_pMediator;
		p_pMediator = NULL;
	}
	if (m_pUdpMediator) {
		m_pUdpMediator->CloseNet();
		delete m_pUdpMediator;
		m_pUdpMediator = NULL;
	}
}

//��ʼ��Э��ӳ���
void CKernel::setProtocolMap()
{

	cout << "CKernel::setProtocolMap" << endl;
	//1:��ʼ������
	memset(m_netProtocolMap, 0, sizeof(m_netProtocolMap));

	//2:��Э��ͷ�Ͷ�Ӧ�Ĵ�����
	NetProFunMap(_DEF_PROTOCOL_TCP_REGISTER_RQ)    = &CKernel::dealRegisterRq;

	NetProFunMap(_DEF_PROTOCOL_TCP_LOGIN_RQ)       = &CKernel::dealLoginRq;

	NetProFunMap(_DEF_PROTOCOL_TCP_CHAT_RQ)        = &CKernel::dealChatRq;

	NetProFunMap(_DEF_PROTOCOL_TCP_OFFLINE_RQ) = &CKernel::dealoffLineRq;

	NetProFunMap(_DEF_PROTOCOL_TCP_ADD_FRIEND_RQ) = &CKernel::dealaddFriendRq;

	NetProFunMap(_DEF_PROTOCOL_TCP_ADD_FRIEND_RS) = &CKernel::dealaddFriendRs;

	NetProFunMap(_DEF_PROTOCOL_TCP_VIDEO_RQ) = &CKernel::dealVideoRq;

	NetProFunMap(_DEF_PROTOCOL_TCP_VIDEO_RS) = &CKernel::dealVideoRs;

	NetProFunMap(_DEF_PROTOCOL_TCP_VOID_RQ) = &CKernel::dealVoidRq;

	NetProFunMap(_DEF_PROTOCOL_TCP_VOID_RS) = &CKernel::dealVoidRs;

	NetProFunMap(_DEF_PROTOCOL_TCP_CLOSE_VOID_RQ) = &CKernel::dealCloseVoid;

	NetProFunMap(_DEF_PROTOCOL_TCP_ADD_BOOKS_IMG_RQ) = &CKernel::dealBooksImgInfo;

	NetProFunMap(_DEF_PROTOCOL_TCP_ADD_BOOKS_RQ) = &CKernel::dealBooksInfo;

	NetProFunMap(_DEF_PROTOCOL_TCP_BOOKS_INFO_RQ ) = &CKernel::dealBooksInfoRq;

	NetProFunMap(_DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_INFO_RQ) = &CKernel::dealAllBooksInfoRq;

	NetProFunMap(_DEF_PROTOCOL_TCP_DELETE_BOOKS_RQ) = &CKernel::dealDeleteBooksInfoRq;

		NetProFunMap(_DEF_PROTOCOL_TCP_ALTER_ICON_RQ) = &CKernel::dealAlterIconRq;







}

//��ȡ�û��ĺ�����Ϣ�������Լ���
void CKernel::getUserFriendInfo(int userId)
{
	//1:��ȡ�Լ�����Ϣ
	STRU_TCP_FRIEND_INFO userInfo;
	getUserInfoById(&userInfo, userId);

	//2�����Լ�����Ϣ�����ͻ���
	if (m_mapIdToSocket.find(userId) == m_mapIdToSocket.end()) {
		cout << "�û����׽��ֲ�����" << endl;
		return;
	}
	p_pMediator->SendData(m_mapIdToSocket[userId], (char*)&userInfo, sizeof(userInfo));

	//3:��ѯ�Լ��ĺ��ѵ�id�б�
	list<string> listRest;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select idB from t_friend where idA = '%d';", userId);
	if (!sql.SelectMySql(sqlBuf, 1, listRest)) {
		cout << "��ѯ���ݿ�ʧ��,sql: " << sqlBuf << endl;
		return;
	}

	//4:������ѯ���
	int friendId = 0;
	while (listRest.size() > 0) {
		//5:��ȡ���ѵ�id
		friendId = atoi(listRest.front().c_str());
		listRest.pop_front();
		STRU_TCP_FRIEND_INFO friendInfo;;
		//6:���ݺ��ѵ�id��ȡ���ѵ���Ϣ
		getUserInfoById(&friendInfo, friendId);

		//�Ѻ��ѵ���Ϣ�����ͻ���
		p_pMediator->SendData(m_mapIdToSocket[userId], (char*)&friendInfo, sizeof(friendInfo));

		//����������ߣ������ѷ��͵�ǰ�ͻ������ߵ�֪ͨ
		if (m_mapIdToSocket.find(friendId) != m_mapIdToSocket.end()) {
			p_pMediator->SendData(m_mapIdToSocket[friendId], (char*)&userInfo, sizeof(userInfo));
		}
	}
}


//����id��ȡ�û���Ϣ
void CKernel::getUserInfoById(STRU_TCP_FRIEND_INFO* info, int id)
{
	//��ѯ������Ϣ
	list<string> listRest;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select name, feeling, icon from t_user where id = '%d';", id);
	if (!sql.SelectMySql(sqlBuf, 3, listRest)) {
		cout << "��ѯ����ʧ�ܣ�sql: " << sqlBuf << endl;
		return;
	}

	//���ṹ�帳ֵ
	if (listRest.size() > 0) {
		//ȡ�ǳ�
		strcpy_s(info->name, listRest.front().c_str());
		listRest.pop_front();

		//ȡǩ��
		strcpy_s(info->feeling, listRest.front().c_str());
		listRest.pop_front();

		//ȡͷ��id
		info->iconId = atoi(listRest.front().c_str());
		listRest.pop_front();
	}
	info-> userId = id;
	if (m_mapIdToSocket.count(id) > 0) {
		//����
		info->status = status_online;
	}
	else
	{
		//������
		info->status = status_offline;
	}
}

//�������н��յ�������
void CKernel::dealReadyData(long ISendIp, char* buf, int nLen)
{
	cout << "CKernel::dealReadyData" << endl;
	//1:ȡ��Э��ͷ
	PackType type = *(PackType*)buf;

	//2:�ж�Э��ͷ�Ƿ��������±����Ч��Χ��
	if (type >= _DEF_PROTOCOL_BASE + 1 && type <= _DEF_PROTOCOL_BASE + _DEF_PROTOCOL_COUNT) {
		//3:���������±꣬ȡ���±��Ӧ�ĺ�����ַ
		PFUN pf = NetProFunMap(type);
		//4:�ж�ָ�벻Ϊ�գ��͵���ָ��ָ��ĺ���
		if (pf) {
			(this->*pf)(ISendIp, buf, nLen);
		}
	}
}

//����ע������
void CKernel::dealRegisterRq(long lSendIp, char* buf, int nLen)
{
	cout << "CKernel::dealRegisterRq" << endl;
	//1:���
	STRU_TCP_REGISTER_RQ* rq = (STRU_TCP_REGISTER_RQ*)buf;
	cout << "tel: " << rq->tel << endl;
	cout << "name:" << rq->name << endl;
	cout << "password:" << rq->password << endl;

	//2����������Ƿ�Ϸ���У�����ͬ�ͻ���һ�£�ʡ�ԣ�
	//3����ѯ�ֻ����Ƿ��Ѿ�ע���
	list<string> listRest;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select tel from t_user where tel = '%s';", rq->tel);   //�Ը�����ʽд���ַ�����
	if (!sql.SelectMySql(sqlBuf, 1, listRest)) {
		cout << "��ѯ���ݿ�ʧ��, sql:" << sqlBuf << endl;
		return;
	}

	STRU_TCP_REGISTER_RS rs;
	if (listRest.size() > 0) {
		//�ֻ����Ѿ�ע������ظ��ͻ���
		rs.result = user_is_exist;
	}
	else {
		//�ֻ���û��ע���
		//4����ѯ�ǳ��Ƿ��Ѿ�ע���
		sprintf_s(sqlBuf, "select name from t_user where name = '%s';", rq->name);
		if (!sql.SelectMySql(sqlBuf, 1, listRest))
		{
			cout << "��ѯ���ݿ�ʧ��, sql:" << sqlBuf << endl;
			return;
		}

		if (listRest.size() > 0) {
			//�ǳ��Ѿ�ע������ظ��ͻ���
			rs.result = user_is_exist;
		}
		else
		{
			//�ǳ�û��ע���
			//5:�������ݿ�
			sprintf_s(sqlBuf, "insert into t_user (tel, password, name, icon, feeling) values ('%s', '%s', '%s', 1, 'ɶҲûд');",
				rq->tel, rq->password, rq->name);
			if (!sql.UpdateMySql(sqlBuf)) {
				cout << "�������ݿ�ʧ�ܣ�sql: " << sqlBuf << endl;
				return;
			}
			rs.result = register_sucess;
		}
	}

	//���ͻ��˻ظ�ע����
	p_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
}

//�����¼����   TODO:����ͷ����Ϣ���ͻ���
void CKernel::dealLoginRq(long lSendIp, char* buf, int nLen)
{
	cout << "CKernel::dealLoginRq" << endl;
	//1:���
	STRU_TCP_LOGIN_RQ* rq = (STRU_TCP_LOGIN_RQ*)buf;
	cout << "tel: " << rq->tel << endl;
	cout << "password:" << rq->password << endl;



	STRU_TCP_LOGIN_RS rs;
	//2:��ѯ�û��Ƿ����
	list<string> listRest;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select id, password from t_user where tel = '%s';", rq->tel);
	if (!sql.SelectMySql(sqlBuf, 2, listRest)) {   //  ���� 2 ��ָҪ�鼸������
		cout << "��ѯ���ݿ�ʧ��, sql: " << sqlBuf << endl;
		return;
	}
	if (0 == listRest.size()) {
		//����û�������
		rs.result = user_not_exist;
	}
	else {
		//����û�����
		// ��ȡ���û�id
		int userId = atoi(listRest.front().c_str());  //atoi():string����תint��
		listRest.pop_front();
		//��ȡ����
		string password = listRest.front();
		listRest.pop_front();  //ÿ������ȡ����͸���������Ƴ�
		//3���ж��û�����������Ƿ���ȷ
		if (strcmp(rq->password,password.c_str()) != 0) {   //strcmp�����������ڱȽ������ַ����Ƿ����
			//�������
			rs.result = password_error;

		}
		else {
			//������ȷ
			rs.result = login_success;
			//�����û�id�Ͷ�Ӧ��socket

			m_mapIdToSocket[userId] = lSendIp;
			//m_mapIdToSocket.insert(pair<int, SOCKET>(userId, lSendIp));

			rs.userId = userId;
			//���ͻ��˻ظ���¼���
			p_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));

			//��ȡ��ǰ��¼�û��ĺ�����Ϣ�������Լ���
			getUserFriendInfo(userId);
			return;
		}
	}
	//4:���ͻ��˻ظ���¼���
	p_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
	
}

void CKernel::dealChatRq(long lSendIp, char* buf, int nLen)
{
	
		cout << "CKernel::dealChatRq " << endl;
		//1:���
		STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)buf;

		//2:�ж϶Զ��û��Ƿ�����
		if (m_mapIdToSocket.count(rq->friendId) > 0) {
			//3:������ߣ���ת����������
			p_pMediator->SendData(m_mapIdToSocket[rq->friendId], buf, nLen);
		}
		else
		{
			//4:��������ߣ��ͻظ��ͻ��˺��Ѳ�����
			STRU_TCP_CHAT_RS rs;
			rs.friendId = rq->friendId;
			rs.userId = rq->userId;
			rs.result = send_error;
			p_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
		}

}

//������������
void CKernel::dealoffLineRq(long lSendIp, char* buf, int nLen)
{

	STRU_TCP_OFFLINE_RQ* rq = (STRU_TCP_OFFLINE_RQ*)buf;
	if (0 < m_mapIdToSocket.count(rq->userId))
	{
		m_mapIdToSocket.erase(rq->userId);
		if (0 < m_mapIdToSockAddr_in.count(rq->userId))
		{
			m_mapIdToSockAddr_in.erase(rq->userId);
		}
		//�����ĺ��ѷ�����������֪ͨ��
		//��ѯ���к���
		list<string> listRest;
		char sqlBuf[1024] = "";
		sprintf_s(sqlBuf, "select idB from t_friend where idA = %d;", rq->userId);
		if (!sql.SelectMySql(sqlBuf, 1, listRest)) {   //  ���� 2 ��ָҪ�鼸������
			cout << "��ѯ���ݿ�ʧ��, sql: " << sqlBuf << endl;
			return;
		}
		//�������ѷ�������֪ͨ
		int friendId = 0;
		while (0 < listRest.size())
		{
			friendId = atoi(listRest.front().c_str());   //atoi():char*ת����
			listRest.pop_front();
			//�жϺ����Ƿ����ߣ����������������֪ͨ
			if (0 < m_mapIdToSocket.count(friendId))
			{
				p_pMediator->SendData(long(m_mapIdToSocket[friendId]), buf, nLen);
			}
		}
	}
}

void CKernel::dealaddFriendRq(long lSendIp, char* buf, int nLen)
{
	//�����
	STRU_TCP_ADD_FRIEND_RQ* rq = (STRU_TCP_ADD_FRIEND_RQ*)buf;
	//���������ݿ��в��ҿ��Ƿ��д��ˣ�
	list<string> listRest;
	char sqlBuf[1024] = "";
	int friendId;
	sprintf_s(sqlBuf, "select id from t_user where name = '%s';", rq->friendName);
	if (sql.SelectMySql(sqlBuf, 1, listRest))
	{
		if (0 < listRest.size())
		{
			friendId =atoi( listRest.front().c_str());
			listRest.pop_front();
		}
	}
	else {
		//������Ӻ��ѻظ����߶Է�û�������
		STRU_TCP_ADD_FRIEND_RS rs;
		rs.result = no_this_user;
		p_pMediator->SendData(m_mapIdToSocket[lSendIp], (char*)&rs, sizeof(rs));
	}
	//�ж϶Է��Ƿ�����
	if (0 < m_mapIdToSocket.count(friendId))
	{
		//������ߣ��������ͺ�������
		p_pMediator->SendData(m_mapIdToSocket[friendId], buf, nLen);
	}
	else {
		//��������ߣ�������߷�����Ӻ��ѻظ�
		STRU_TCP_ADD_FRIEND_RS rs;
		rs.result = user_notlogin;
		p_pMediator->SendData(m_mapIdToSocket[lSendIp], (char*)&rs, sizeof(rs));
	}

}

//������Ӻ��ѻظ�
void CKernel::dealaddFriendRs(long lSendIp, char* buf, int nLen)
{
	cout << "������Ӻ�������" << endl;
	STRU_TCP_ADD_FRIEND_RS* rs = (STRU_TCP_ADD_FRIEND_RS*)buf;
	//1:�ж���Ӻ����Ƿ�ɹ�
	if (add_success == rs->result)
	{
		//д�����ݿ⣨ע��д�����ݿ���˫��ģ������߷ֱ��Ϊһ��idA��
		char sqlBuf[1024] = "";
		sprintf_s(sqlBuf, "start transaction;");
		if (!sql.UpdateMySql(sqlBuf))  //����ֻ�ǽ����������ִ�д�����Ϣ����ʵ�ײ㶼����mysql_queryʵ�ֵģ����ǽ�sql����͵����ݿ�ִ��
		{
			cout << "��Ӻ���ʧ��" << endl;
			return ;
		}
		memset(sqlBuf, 0, 1024);
		sprintf_s(sqlBuf, "insert into t_friend values ('%d' , '%d' );", rs->userId, rs->friendId);
		if (!sql.UpdateMySql(sqlBuf))  //����ֻ�ǽ����������ִ�д�����Ϣ����ʵ�ײ㶼����mysql_queryʵ�ֵģ����ǽ�sql����͵����ݿ�ִ��
		{
			cout << "��Ӻ���ʧ��" << endl;
			return;
		}
		memset(sqlBuf, 0, 1024);
		sprintf_s(sqlBuf, "insert into t_friend values ( '%d' , '%d' );", rs->friendId, rs->userId);
		if (!sql.UpdateMySql(sqlBuf))  //����ֻ�ǽ����������ִ�д�����Ϣ����ʵ�ײ㶼����mysql_queryʵ�ֵģ����ǽ�sql����͵����ݿ�ִ��
		{
			cout << "��Ӻ���ʧ��" << endl;
			return;
		}
		//memset(sqlBuf, 0, 1024);
		//sprintf_s(sqlBuf,"rollback;");  //rollback�������еĻع�����
		//if (!sql.UpdateMySql(sqlBuf))  //����ֻ�ǽ����������ִ�д�����Ϣ����ʵ�ײ㶼����mysql_queryʵ�ֵģ����ǽ�sql����͵����ݿ�ִ��
		//{
		//	cout << "��Ӻ���ʧ��" << endl;
		//	return;
		//}
		memset(sqlBuf, 0, 1024);
		sprintf_s(sqlBuf, "commit;");
		if (!sql.UpdateMySql(sqlBuf))  //����ֻ�ǽ����������ִ�д�����Ϣ����ʵ�ײ㶼����mysql_queryʵ�ֵģ����ǽ�sql����͵����ݿ�ִ��
		{
			cout << "��Ӻ���ʧ��" << endl;
			return;
		}

		//��ӳɹ�����Ϣ����friendId��
		if (0 < m_mapIdToSocket.count(rs->friendId))
			p_pMediator->SendData(m_mapIdToSocket[rs->friendId], buf, nLen);

		//�����º��ѵ���Ϣ���͸��Է�
		STRU_TCP_FRIEND_INFO userinfo;
		getUserInfoById(&userinfo, rs->friendId);
		if (0 < m_mapIdToSocket.count(rs->userId))
			p_pMediator->SendData(m_mapIdToSocket[rs->userId], (char*)&userinfo, sizeof(userinfo));

		getUserInfoById(&userinfo, rs->userId);
		if (0 < m_mapIdToSocket.count(rs->friendId))
			p_pMediator->SendData(m_mapIdToSocket[rs->friendId], (char*)&userinfo, sizeof(userinfo));

		/*
		����������
		start transaction;
		update bank set money = money -50 where name = '���ʽ�';
		update bank set money = money + 50 where name = '��Ԫ��';
		select * from bank;

		rollback;

		commit;
		*/
	}
	else {
		//���ʧ��
		if (0 < m_mapIdToSocket.count(rs->friendId))
			p_pMediator->SendData(m_mapIdToSocket[rs->friendId], buf, nLen);
	}
}

//============================================================================================
void CKernel::dealReadyUdpData(long lSendIp, char* buf, int nLen)
{
	//�����������ͣ����ò�ͬ�ĺ����������봦��tcp���ݵĺ����ϲ�����Ϊ�˿�������ֱ�ۣ��ͷֿ�д�ˣ�
	int  a = *(int*)buf;
	switch (a)
	{
	case _DEF_PROTOCOL_ONLINE_RQ:
	{
		dealUdpOnlineRq(lSendIp, buf, nLen);
	}
	break;
	case _DEF_PROTOCOL_CHAT_RQ:
	{
		dealUdpChatRq(lSendIp, buf, nLen);
	}
	break;
	case _DEF_PROTOCOL_LIVE:
	{
		dealLifeRq(lSendIp, buf, nLen);
	}
	break;
	case _DEF_VIDEO_TRAN:
	{
		dealVideoInfo(lSendIp, buf, nLen);
	}
	case _DEF_VOID_TRAN:
	{
		dealVoidInfo(lSendIp, buf, nLen);
	}
	default:
		break;
	}
	delete[] buf;


}

//����udp��������
void CKernel::dealUdpOnlineRq(long lSendIp, char* buf, int nLen)
{
	cout << lSendIp << "dealUdpOnlineRq" << endl;

	STRU_ONLINE* rq = (STRU_ONLINE*)buf;
	m_mapIdToSockAddr_in[rq->m_id] = rq->m_id;

}

//����udp��������
void CKernel::dealUdpChatRq(long lSendIp, char* buf, int nLen)
{
	//1��������鿴�Ƿ���˭��
	STRU_CHAT* rq = (STRU_CHAT*)buf;

	if (0 >= m_mapIdToSockAddr_in.count(rq->ISendIp))
	{
		cout << rq->ISendIp << "������" << endl;
		return;
	}
	cout << "������" << m_mapIdToSockAddr_in[rq->ISendIp] << endl;

	m_pUdpMediator->SendData(m_mapIdToSockAddr_in[rq->ISendIp], buf, nLen);

}

//����udp����������
void CKernel::dealLifeRq(long lSendIp, char* buf, int nLen)
{
	STRU_OFFLINE* rq = (STRU_OFFLINE*)buf;  //��Ϊ����������ʵ�����ݣ�������������������������ĵģ���type�ı䣬
	m_mapIdToSockAddr_in[rq->m_id] = lSendIp;
	return;
}

//�����ļ���Ϣ������߻ظ�
void CKernel::dealFileInfoRq_Rs(long ISendIp, char* buf, int nLen)
{

}

//������Ƶͨ������
void CKernel::dealVideoRq(long ISendIp, char* buf, int nLen)
{
	STRU_VIDEO_RQ* rq = (STRU_VIDEO_RQ*)buf;
	//�鿴�Ǹ�˭���͵���Ƶͨ�������
	cout << "����һ����Ƶͨ�������" << endl;
	if (m_mapIdToSocket.count(rq->ISendIp))
	{
		p_pMediator->SendData(m_mapIdToSocket[rq->ISendIp], buf, nLen);
	}
}

//������Ƶͨ���ظ�
void CKernel::dealVideoRs(long ISendIp, char* buf, int nLen)
{
	STRU_VIDEO_RS* rq = (STRU_VIDEO_RS*)buf;
	cout << "����һ����Ƶͨ���ظ���" << endl;
	//�鿴�Ǹ�˭���͵���Ƶͨ�������
	if (m_mapIdToSocket.count(rq->ISendIp))
	{
		p_pMediator->SendData(m_mapIdToSocket[rq->ISendIp], buf, nLen);
	}
}

//������Ƶͨ��������
void CKernel::dealVideoInfo(long ISendIp, char* buf, int nLen)
{
	cout << "����һ����Ƶ��Ϣ��" << endl;
	if (m_mapIdToSockAddr_in.count(*(((int*)buf) + 1)))   //����ֱ�Ӷ�ָ����������ݰ��еĵڶ������ֽڿռ��ŵľ���Ŀ������id
	{
		m_pUdpMediator->SendData(m_mapIdToSockAddr_in[*(((int*)buf) + 1)], buf, nLen);
	}
}

//��������ͨ������
void CKernel::dealVoidRq(long ISendIp, char* buf, int nLen)
{
	STRU_VOID_RQ* rq = (STRU_VOID_RQ*)buf;
	if (m_mapIdToSocket.count(rq->ISendIp))
	{
		p_pMediator->SendData(m_mapIdToSocket[rq->ISendIp], buf, nLen);
	}
}

//��������ͨ���ظ�
void CKernel::dealVoidRs(long ISendIp, char* buf, int nLen)
{
	STRU_VOID_RS* rs = (STRU_VOID_RS*)buf;
	if (m_mapIdToSocket.count(rs->ISendIp))
	{
		p_pMediator->SendData(m_mapIdToSocket[rs->ISendIp], buf, nLen);
	}
}

//����Ҷ�����ͨ������
void CKernel::dealCloseVoid(long ISendIp, char* buf, int nLen)
{
	cout << "����һ���Ҷ�����ͨ��������" << endl;
	STRU_CLOSE_VOID* rq = (STRU_CLOSE_VOID*)buf;
	p_pMediator->SendData(m_mapIdToSocket[rq->ISendIp], buf, nLen);
}

//�����鼮�ϼ����루�鼮��Ϣ��
void CKernel::dealBooksImgInfo(long ISendIp, char* buf, int nLen)
{

	//��ȡ���鼮���ƺ��ϴ��߱��
	int id = *((int*)buf+1);    //��ǰ����type����������Ҫ��1
	char bookName[_DEF_BOOK_NAME_SIZE];
	memcpy(bookName, (int*)buf + 2, _DEF_BOOK_NAME_SIZE);  //�ӵ������ֽڿ�ʼ�����_DEF_BOOK_NAME_SIZE���ֽ�������
	char* bookImg = buf + 4 + 4 + _DEF_BOOK_NAME_SIZE;    //����type,id,bookName
	int bookImgSize = nLen - 4 - 4 - _DEF_BOOK_NAME_SIZE;  //��ȥtype��id����bookName
	cout << id << "         " << bookName << endl;
	

	//��ͼƬ��Ϣ�����ļ���
	//�Ƚ��ļ���ƴ����
	char fileName[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
	char num[5];
	cout << id << endl;
	_itoa_s(id, num,10);
	strcat_s(fileName, num);  //idƴ�ӽ�·��
	strcat_s(fileName, bookName);  //����ƴ�ӽ�·��
	strcat_s(fileName, ".jpg");  //��׺ƴ����·��
	cout << fileName << endl;
	FILE* fp;
	if ( 0 == fopen_s(&fp, fileName, "rb") )  //��ֻ����ʽ���ļ�����򿪳ɹ��򷵻�0
	{
		//������Ƿ���NULL����������ļ��Ѿ�����
		//TODO:����һ�����ݰ������ϴ������Ѿ��ϴ����ı��鼮�������ظ��ϴ����˴��������յ��鼮��Ϣ��ʱ�򷢣�

		cout << "���ļ��Ѵ���" << endl;
		//���򿪵��ļ��ر�
		fclose(fp);
		return ;
	}
	//������ļ���������֤�����ݿ���û�и��鼮����Ϣ
	//���鼮ͼƬ�����ļ�����
	if (0 == fopen_s(&fp, fileName, "wb"))  //�Զ�����д�ķ�ʽ�򿪣��������ļ�(�˴�Ҳ��Ӧ����rb��ʽ�򿪣��˴��ݲ�����)
	{
		int number = fwrite(bookImg, bookImgSize, 1, fp);
		//���򿪵��ļ��ر�
		fclose(fp);
		cout << "ͼƬ��СΪ��" << bookImgSize << endl;
		//�����ϼܳɹ��ķ��ذ����˴��������յ��鼮��Ϣ��ʱ�򷢣�

	}
	/*
	size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)


���Կ�����������Ĳ������ĸ���

��һ��ptr��Ҫд������ݵ�ͷָ�룬�޷������ͣ�

�ڶ�������size�Ǵ�С����ʾÿ��д��Ԫ�صĴ�С����λ���ֽڣ�

����������nmemb�Ǹ���������һ������Ϊ��λ�ĸ�����

���ĸ�����stream�����ļ�ָ�룬��ʾ������д��
	*/
}

//�����鼮�ϼ����루���յ����鼮��������Ϣ��
void CKernel::dealBooksInfo(long ISendIp, char* buf, int nLen)
{
	//���
	STRU_TCP_ADD_BOOKS* rq = (STRU_TCP_ADD_BOOKS*)buf;
	//���������ͷ����߱��ƴ����ַ���鿴�ļ������Ƿ��������鼮��ͼƬ
	char filePath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
	char id[5];
	_itoa_s(rq->id, id, 10);
	strcat_s(filePath, id);
	strcat_s(filePath, rq->bookName);
	strcat_s(filePath, ".jpg");
	cout << filePath << endl;
	//���Դ򿪸��ļ�
	FILE* fp;
	STRU_TCP_ADD_BOOKS_RS rs;
	strcpy_s(rs.bookName, rq->bookName);
	if (0 == fopen_s(&fp, filePath, "rb"))  //��ֻ����ʽ���ļ�����򿪳ɹ��򷵻�0
	{
		cout << "�򿪳ɹ�" << endl;
		//���򿪵��ļ��ر�
		fclose(fp);
		//TODO:����򿪳ɹ������Խ��鼮��Ϣд�����ݿ�
		char sqlBuf[1024] = "";
		if (SCHOOL_BOOK == rq->isSchool)   //У���鼮   
		{                                                   //bookid���������ݣ����ﲻ�ù�
			sprintf_s(sqlBuf, "insert into schoolbooksinfo (id , isSchool, bookName, bookArea, bookInfo, bookPrice, QQ, weChat) values ( %d,%d,'%s','%s','%s','%s' ,'%s','%s');", rq->id, rq->isSchool, rq->bookName, rq->bookArea, rq->bookInfo, rq->bookPrice, rq->QQ, rq->weChat);
		}
		else if (NO_SCHOOL_BOOK == rq->isSchool)   //У���鼮
		{													//bookid���������ݣ����ﲻ�ù�
			sprintf_s(sqlBuf, "insert into noschoolbooksinfo (id , isSchool, bookName, bookArea, bookInfo, bookPrice, QQ, weChat) values ( %d,%d,'%s','%s','%s','%s' ,'%s','%s');", rq->id, rq->isSchool, rq->bookName, rq->bookArea, rq->bookInfo, rq->bookPrice, rq->QQ, rq->weChat);

		}
		else {   //������Ʒ											//bookid���������ݣ����ﲻ�ù�
			sprintf_s(sqlBuf, "insert into othergoodsinfo (id , isSchool, bookName, bookArea, bookInfo, bookPrice, QQ, weChat) values ( %d,%d,'%s','%s','%s','%s' ,'%s','%s');", rq->id, rq->isSchool, rq->bookName, rq->bookArea, rq->bookInfo, rq->bookPrice, rq->QQ, rq->weChat);

		}
		if (!sql.UpdateMySql(sqlBuf)) {
			cout << "��������ʧ�ܣ�sql: " << sqlBuf << endl;
			//�����������ʧ��Ĭ�������Ϊ�������ظ���
			//TODO:�����ϼܻظ����û����ظ��ϼ���
			rs.result = REPEAT_SUBMIT;
			p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));
			return;
		}
		else {
			//�����ϼܳɹ�����Ϣ���û�
			rs.result = SUBMIT_SUCCESS;
			p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));
			return;
		}


		return;
	}
	else {
		//TODO:��ʧ�������鼮�ϼܴ�����Ϣ������д��ʧ�ܣ����û�
		rs.result = SUBMIT_ERROR;
		p_pMediator->SendData(ISendIp,(char*)&rs, sizeof(rs));
	}
}

//�����յ��Ļ�ȡ�鼮��Ϣ������
void CKernel::dealBooksInfoRq(long ISendIp, char* buf, int nLen)
{
	STRU_TCP_BOOKS_INFO_RQ* rq = (STRU_TCP_BOOKS_INFO_RQ*)buf;
	//TODO:���������ݿ��е�У���鼮��У���鼮���Լ�������Ʒ��10�������û��10�����������ƣ�
	list<string> lst;
	char sqlBuf[1024] = "";

	if (SELECT_BOOK == rq->isSelect)   //����������鼮��Ϣ������
	{
		//�ж�����������һ��
		if (SCHOOL_BOOK == rq->isSchool)
		{
			sprintf_s(sqlBuf, "select * from (select * from schoolbooksinfo where bookName like '%s%s%s' ) book limit %d , 10;","%", rq->bookName,"%", rq->num);   //ÿ�ε�ƫ����Ϊ10,ģ������
			if (!sql.SelectMySql(sqlBuf, 9, lst))   //����9������
			{
				cout << "��ѯ���ݿ�ʧ��" << endl;
			}
			else {
				//���鼮��Ϣ���͸��ͻ���
				while (lst.size() > 0)
				{
					STRU_TCP_BOOKS_TEXT_INFO_RS rs;
					char bookid[5] = "";
					strcpy_s(bookid, lst.front().c_str());
					lst.pop_front();
					char id[5] = "";
					strcpy_s(id, lst.front().c_str());
					lst.pop_front();
					char isSchool[5] = "";
					strcpy_s(isSchool, lst.front().c_str());
					lst.pop_front();
					char bookName[_DEF_BOOK_NAME_SIZE];
					strcpy_s(bookName, lst.front().c_str());
					lst.pop_front();

					//����id��bookNameƴ���鼮ͼƬ��ַ
					char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
					strcat_s(bookPath, id);
					strcat_s(bookPath, bookName);
					strcat_s(bookPath, ".jpg");   //���ļ�·��ƴ����
					FILE* fp;
					if (0 == fopen_s(&fp, bookPath, "rb"))  //�Զ����ƶ��ķ�ʽ���ļ�
					{
						//����򿪳ɹ������ļ���Ϣ����

							// ����һ���洢�ļ�(��)��Ϣ�Ľṹ�壬�������ļ���С�ʹ���ʱ�䡢����ʱ�䡢�޸�ʱ���
						struct stat statbuf;
						// �ṩ�ļ����ַ���������ļ����Խṹ��
						stat(bookPath, &statbuf);
						int fileSize = statbuf.st_size;
						char* img = new char[fileSize];
						int num = fread(img, fileSize,1, fp);   //��ͼƬ�ļ����뵽img��
						//����֮��ر��ļ�
						fclose(fp);
						//����־λ���鼮���ϴ��ߺ��Ƿ��ǽ����鼮��������������һ�𷢸��ͻ���
						int type = _DEF_PROTOCOL_TCP_SELECT_BOOKS_IMG_INFO_RS;
						int packSize = fileSize + 4 + 5 + 5;  //�ֱ��ǣ�type,bookid,isSchool
						char* imgInfo = new  char[packSize]; //�ļ���С + type��С + bookid��С +isSchool��С
						memcpy(imgInfo, (char*)&type, 4);  //�Ƚ�type�����ȥ
						memcpy(imgInfo + 4, bookid, 5);   //�ٽ�bookid��ӵ�β��
						memcpy(imgInfo + 9, isSchool, 5);  //�ٽ�isSchool�����ȥ									
						memcpy(imgInfo + 14, img, fileSize);   //�ٽ��鼮ͼƬ��Ϣ��ӵ�β��
						//���˰����͸��ͻ���
						p_pMediator->SendData(ISendIp, imgInfo, packSize);

						delete[]img;
						img = NULL;
						delete[]imgInfo;
						imgInfo = NULL;


						//ͼƬ��Ϣ�������֮����������Ϣ
						rs.bookid = atoi(bookid);
						rs.id = atoi(id);
						rs.isSchool = atoi(isSchool);
						strcpy_s(rs.bookName, bookName);
						strcpy_s(rs.bookArea, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.bookInfo, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.bookPrice, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.QQ, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.weChat, lst.front().c_str());
						lst.pop_front();
						rs.type = _DEF_PROTOCOL_TCP_SELECT_BOOKS_TEXT_INFO_RS;
						p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));

					}
					else   //���û�ж���ͼƬҲ��Ҫ��list�е�Ԫ��pop��������ѭ��
					{
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
					}
					//�����ͼƬ�ļ�ʧ���������һ��ѭ��


				}
			}
		}
		else if (NO_SCHOOL_BOOK == rq->isSchool)  //�����У���鼮
		{
			sprintf_s(sqlBuf, "select * from (select * from noschoolbooksinfo where bookName like '%s%s%s' ) book limit %d , 10;", "%",rq->bookName,"%", rq->num);   //ÿ�ε�ƫ����Ϊ10
			if (!sql.SelectMySql(sqlBuf, 9, lst))   //����9������
			{
				cout << "��ѯ���ݿ�ʧ��" << endl;
			}
			else {
				//���鼮��Ϣ���͸��ͻ���
				while (lst.size() > 0)
				{
					STRU_TCP_BOOKS_TEXT_INFO_RS rs;
					char bookid[5] = "";
					strcpy_s(bookid, lst.front().c_str());
					lst.pop_front();
					char id[5] = "";
					strcpy_s(id, lst.front().c_str());
					lst.pop_front();
					char isSchool[5] = "";
					strcpy_s(isSchool, lst.front().c_str());
					lst.pop_front();
					char bookName[_DEF_BOOK_NAME_SIZE];
					strcpy_s(bookName, lst.front().c_str());
					lst.pop_front();

					//����id��bookNameƴ���鼮ͼƬ��ַ
					char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
					strcat_s(bookPath, id);
					strcat_s(bookPath, bookName);
					strcat_s(bookPath, ".jpg");   //���ļ�·��ƴ����
					FILE* fp;
					if (0 == fopen_s(&fp, bookPath, "rb"))  //�Զ����ƶ��ķ�ʽ���ļ�
					{
						//����򿪳ɹ������ļ���Ϣ����

							// ����һ���洢�ļ�(��)��Ϣ�Ľṹ�壬�������ļ���С�ʹ���ʱ�䡢����ʱ�䡢�޸�ʱ���
						struct stat statbuf;
						// �ṩ�ļ����ַ���������ļ����Խṹ��
						stat(bookPath, &statbuf);
						int fileSize = statbuf.st_size;
						char* img = new char[fileSize];
						int num = fread(img, fileSize,1, fp);   //��ͼƬ�ļ����뵽img��
						//����֮��ر��ļ�
						fclose(fp);
						//����־λ���鼮���ϴ��ߺ��Ƿ��ǽ����鼮��������������һ�𷢸��ͻ���
						int type = _DEF_PROTOCOL_TCP_SELECT_BOOKS_IMG_INFO_RS;
						int packSize = fileSize + 4 + 5 + 5;  //�ֱ��ǣ�type,bookid,isSchool
						char* imgInfo = new  char[packSize]; //�ļ���С + type��С + bookid��С +isSchool��С
						memcpy(imgInfo, (char*)&type, 4);  //�Ƚ�type�����ȥ
						memcpy(imgInfo + 4, bookid, 5);   //�ٽ�bookid��ӵ�β��
						memcpy(imgInfo + 9, isSchool, 5);  //�ٽ�isSchool�����ȥ									
						memcpy(imgInfo + 14, img, fileSize);   //�ٽ��鼮ͼƬ��Ϣ��ӵ�β��
						//���˰����͸��ͻ���
						p_pMediator->SendData(ISendIp, imgInfo, packSize);

						delete[]img;
						img = NULL;
						delete[]imgInfo;
						imgInfo = NULL;


						//ͼƬ��Ϣ�������֮����������Ϣ
						rs.bookid = atoi(bookid);
						rs.id = atoi(id);
						rs.isSchool = atoi(isSchool);
						strcpy_s(rs.bookName, bookName);
						strcpy_s(rs.bookArea, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.bookInfo, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.bookPrice, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.QQ, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.weChat, lst.front().c_str());
						lst.pop_front();
						rs.type = _DEF_PROTOCOL_TCP_SELECT_BOOKS_TEXT_INFO_RS;
						p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));

					}
					else   //���û�ж���ͼƬҲ��Ҫ��list�е�Ԫ��pop��������ѭ��
					{
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
					}
					//�����ͼƬ�ļ�ʧ���������һ��ѭ��


				}
			}
		}

		else if (OTHER_GOODS == rq->isSchool)  //�����������Ʒ
		{

		sprintf_s(sqlBuf, "select * from (select * from othergoodsinfo where bookName like '%s%s%s' ) book limit %d , 10;","%", rq->bookName,"%", rq->num);   //ÿ�ε�ƫ����Ϊ10
		if (!sql.SelectMySql(sqlBuf, 9, lst))   //����9������
		{
			cout << "��ѯ���ݿ�ʧ��" << endl;
		}
		else {
			//���鼮��Ϣ���͸��ͻ���
			while (lst.size() > 0)
			{
				STRU_TCP_BOOKS_TEXT_INFO_RS rs;
				char bookid[5] = "";
				strcpy_s(bookid, lst.front().c_str());
				lst.pop_front();
				char id[5] = "";
				strcpy_s(id, lst.front().c_str());
				lst.pop_front();
				char isSchool[5] = "";
				strcpy_s(isSchool, lst.front().c_str());
				lst.pop_front();
				char bookName[_DEF_BOOK_NAME_SIZE];
				strcpy_s(bookName, lst.front().c_str());
				lst.pop_front();

				//����id��bookNameƴ���鼮ͼƬ��ַ
				char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
				strcat_s(bookPath, id);
				strcat_s(bookPath, bookName);
				strcat_s(bookPath, ".jpg");   //���ļ�·��ƴ����
				FILE* fp;
				if (0 == fopen_s(&fp, bookPath, "rb"))  //�Զ����ƶ��ķ�ʽ���ļ�
				{
					//����򿪳ɹ������ļ���Ϣ����

						// ����һ���洢�ļ�(��)��Ϣ�Ľṹ�壬�������ļ���С�ʹ���ʱ�䡢����ʱ�䡢�޸�ʱ���
					struct stat statbuf;
					// �ṩ�ļ����ַ���������ļ����Խṹ��
					stat(bookPath, &statbuf);
					int fileSize = statbuf.st_size;
					char* img = new char[fileSize];
					int num = fread(img, fileSize,1, fp);   //��ͼƬ�ļ����뵽img��
					//����֮��ر��ļ�
					fclose(fp);
					//����־λ���鼮���ϴ��ߺ��Ƿ��ǽ����鼮��������������һ�𷢸��ͻ���
					int type = _DEF_PROTOCOL_TCP_SELECT_BOOKS_IMG_INFO_RS;
					int packSize = fileSize + 4 + 5 + 5;  //�ֱ��ǣ�type,bookid,isSchool
					char* imgInfo = new  char[packSize]; //�ļ���С + type��С + bookid��С +isSchool��С
					memcpy(imgInfo, (char*)&type, 4);  //�Ƚ�type�����ȥ
					memcpy(imgInfo + 4, bookid, 5);   //�ٽ�bookid��ӵ�β��
					memcpy(imgInfo + 9, isSchool, 5);  //�ٽ�isSchool�����ȥ									
					memcpy(imgInfo + 14, img, fileSize);   //�ٽ��鼮ͼƬ��Ϣ��ӵ�β��
					//���˰����͸��ͻ���
					p_pMediator->SendData(ISendIp, imgInfo, packSize);

					delete[]img;
					img = NULL;
					delete[]imgInfo;
					imgInfo = NULL;


					//ͼƬ��Ϣ�������֮����������Ϣ
					rs.bookid = atoi(bookid);
					rs.id = atoi(id);
					rs.isSchool = atoi(isSchool);
					strcpy_s(rs.bookName, bookName);
					strcpy_s(rs.bookArea, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.bookInfo, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.bookPrice, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.QQ, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.weChat, lst.front().c_str());
					lst.pop_front();
					rs.type = _DEF_PROTOCOL_TCP_SELECT_BOOKS_TEXT_INFO_RS;
					p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));

				}
				else   //���û�ж���ͼƬҲ��Ҫ��list�е�Ԫ��pop��������ѭ��
				{
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
				}
				//�����ͼƬ�ļ�ʧ���������һ��ѭ��


			}
		}

		}
	}



	else {     //�������������
		if (SCHOOL_BOOK == rq->isSchool || ALL_BOOK_AND_OTHER == rq->isSchool)   //�����ѯ����У���鼮��������
		{
			sprintf_s(sqlBuf, "select * from schoolbooksinfo limit %d , 10 ;", rq->num);   //ÿ�ε�ƫ����Ϊ10
			if (!sql.SelectMySql(sqlBuf, 9, lst))   //����9������
			{
				cout << "��ѯ���ݿ�ʧ��" << endl;
			}
			else {
				//���鼮��Ϣ���͸��ͻ���
				while (lst.size() > 0)
				{
					STRU_TCP_BOOKS_TEXT_INFO_RS rs;
					char bookid[5] = "";
					strcpy_s(bookid, lst.front().c_str());
					lst.pop_front();
					char id[5] = "";
					strcpy_s(id, lst.front().c_str());
					lst.pop_front();
					char isSchool[5] = "";
					strcpy_s(isSchool, lst.front().c_str());
					lst.pop_front();
					char bookName[_DEF_BOOK_NAME_SIZE];
					strcpy_s(bookName, lst.front().c_str());
					lst.pop_front();

					//����id��bookNameƴ���鼮ͼƬ��ַ
					char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
					strcat_s(bookPath, id);
					strcat_s(bookPath, bookName);
					strcat_s(bookPath, ".jpg");   //���ļ�·��ƴ����
					FILE* fp;
					if (0 == fopen_s(&fp, bookPath, "rb"))  //�Զ����ƶ��ķ�ʽ���ļ�
					{
						//����򿪳ɹ������ļ���Ϣ����

							// ����һ���洢�ļ�(��)��Ϣ�Ľṹ�壬�������ļ���С�ʹ���ʱ�䡢����ʱ�䡢�޸�ʱ���
						struct stat statbuf;
						// �ṩ�ļ����ַ���������ļ����Խṹ��
						stat(bookPath, &statbuf);
						int fileSize = statbuf.st_size;
						char* img = new char[fileSize];
						int num = fread(img, fileSize,1, fp);   //��ͼƬ�ļ����뵽img��
						//����֮��ر��ļ�
						fclose(fp);
						//����־λ���鼮���ϴ��ߺ��Ƿ��ǽ����鼮��������������һ�𷢸��ͻ���
						int type = _DEF_PROTOCOL_TCP_BOOKS_IMG_INFO_RS;
						int packSize = fileSize + 4 + 5 + 5;  //�ֱ��ǣ�type,bookid,isSchool
						char* imgInfo = new  char[packSize]; //�ļ���С + type��С + bookid��С +isSchool��С
						memcpy(imgInfo, (char*)&type, 4);  //�Ƚ�type�����ȥ
						memcpy(imgInfo + 4, bookid, 5);   //�ٽ�bookid��ӵ�β��
						memcpy(imgInfo + 9, isSchool, 5);  //�ٽ�isSchool�����ȥ									
						memcpy(imgInfo + 14, img, fileSize);   //�ٽ��鼮ͼƬ��Ϣ��ӵ�β��
						//���˰����͸��ͻ���
						p_pMediator->SendData(ISendIp, imgInfo, packSize);

						delete[]img;
						img = NULL;
						delete[]imgInfo;
						imgInfo = NULL;


						//ͼƬ��Ϣ�������֮����������Ϣ
						rs.bookid = atoi(bookid);
						rs.id = atoi(id);
						rs.isSchool = atoi(isSchool);
						strcpy_s(rs.bookName, bookName);
						strcpy_s(rs.bookArea, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.bookInfo, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.bookPrice, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.QQ, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.weChat, lst.front().c_str());
						lst.pop_front();

						p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));

					}
					else   //���û�ж���ͼƬҲ��Ҫ��list�е�Ԫ��pop��������ѭ��
					{
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
					}
					//�����ͼƬ�ļ�ʧ���������һ��ѭ��


				}
			}
		}


		if (NO_SCHOOL_BOOK == rq->isSchool || ALL_BOOK_AND_OTHER == rq->isSchool)   //�����ѯ����У���鼮��������
		{
			sprintf_s(sqlBuf, "select * from noschoolbooksinfo limit %d , 10 ;", rq->num);   //ÿ�ε�ƫ����Ϊ10
			if (!sql.SelectMySql(sqlBuf, 9, lst))   //����9������
			{
				cout << "��ѯ���ݿ�ʧ��" << endl;
			}
			else {
				//���鼮��Ϣ���͸��ͻ���
				while (lst.size() > 0)
				{
					STRU_TCP_BOOKS_TEXT_INFO_RS rs;
					char bookid[5] = "";
					strcpy_s(bookid, lst.front().c_str());
					lst.pop_front();
					char id[5] = "";
					strcpy_s(id, lst.front().c_str());
					lst.pop_front();
					char isSchool[5] = "";
					strcpy_s(isSchool, lst.front().c_str());
					lst.pop_front();
					char bookName[_DEF_BOOK_NAME_SIZE];
					strcpy_s(bookName, lst.front().c_str());
					lst.pop_front();

					//����id��bookNameƴ���鼮ͼƬ��ַ
					char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
					strcat_s(bookPath, id);
					strcat_s(bookPath, bookName);
					strcat_s(bookPath, ".jpg");   //���ļ�·��ƴ����
					FILE* fp;
					if (0 == fopen_s(&fp, bookPath, "rb"))  //�Զ����ƶ��ķ�ʽ���ļ�
					{
						//����򿪳ɹ������ļ���Ϣ����

							// ����һ���洢�ļ�(��)��Ϣ�Ľṹ�壬�������ļ���С�ʹ���ʱ�䡢����ʱ�䡢�޸�ʱ���
						struct stat statbuf;
						// �ṩ�ļ����ַ���������ļ����Խṹ��
						stat(bookPath, &statbuf);
						int fileSize = statbuf.st_size;
						char* img = new char[fileSize];
						int num = fread(img, fileSize,1, fp);   //��ͼƬ�ļ����뵽img��
						//����֮��ر��ļ�
						fclose(fp);
						//����־λ���鼮���ϴ��ߺ��Ƿ��ǽ����鼮��������������һ�𷢸��ͻ���
						int type = _DEF_PROTOCOL_TCP_BOOKS_IMG_INFO_RS;
						int packSize = fileSize + 4 + 5 + 5;  //�ֱ��ǣ�type,bookid,isSchool
						char* imgInfo = new  char[packSize]; //�ļ���С + type��С + bookid��С +isSchool��С
						memcpy(imgInfo, (char*)&type, 4);  //�Ƚ�type�����ȥ
						memcpy(imgInfo + 4, bookid, 5);   //�ٽ�bookid��ӵ�β��
						memcpy(imgInfo + 9, isSchool, 5);  //�ٽ�isSchool�����ȥ									
						memcpy(imgInfo + 14, img, fileSize);   //�ٽ��鼮ͼƬ��Ϣ��ӵ�β��
						//���˰����͸��ͻ���
						p_pMediator->SendData(ISendIp, imgInfo, packSize);

						delete[]img;
						img = NULL;
						delete[]imgInfo;
						imgInfo = NULL;


						//ͼƬ��Ϣ�������֮����������Ϣ
						rs.bookid = atoi(bookid);
						rs.id = atoi(id);
						rs.isSchool = atoi(isSchool);
						strcpy_s(rs.bookName, bookName);
						strcpy_s(rs.bookArea, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.bookInfo, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.bookPrice, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.QQ, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.weChat, lst.front().c_str());
						lst.pop_front();

						p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));

					}
					else   //���û�ж���ͼƬҲ��Ҫ��list�е�Ԫ��pop��������ѭ��
					{
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
					}
					//�����ͼƬ�ļ�ʧ���������һ��ѭ��


				}
			}
		}



		if (OTHER_GOODS == rq->isSchool || ALL_BOOK_AND_OTHER == rq->isSchool)   //�����ѯ����У���鼮��������
		{
			sprintf_s(sqlBuf, "select * from othergoodsinfo limit %d , 10 ;", rq->num);   //ÿ�ε�ƫ����Ϊ10
			if (!sql.SelectMySql(sqlBuf, 9, lst))   //����9������
			{
				cout << "��ѯ���ݿ�ʧ��" << endl;
			}
			else {
				//���鼮��Ϣ���͸��ͻ���
				while (lst.size() > 0)
				{
					STRU_TCP_BOOKS_TEXT_INFO_RS rs;
					char bookid[5] = "";
					strcpy_s(bookid, lst.front().c_str());
					lst.pop_front();
					char id[5] = "";
					strcpy_s(id, lst.front().c_str());
					lst.pop_front();
					char isSchool[5] = "";
					strcpy_s(isSchool, lst.front().c_str());
					lst.pop_front();
					char bookName[_DEF_BOOK_NAME_SIZE];
					strcpy_s(bookName, lst.front().c_str());
					lst.pop_front();

					//����id��bookNameƴ���鼮ͼƬ��ַ
					char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
					strcat_s(bookPath, id);
					strcat_s(bookPath, bookName);
					strcat_s(bookPath, ".jpg");   //���ļ�·��ƴ����
					FILE* fp;
					if (0 == fopen_s(&fp, bookPath, "rb"))  //�Զ����ƶ��ķ�ʽ���ļ�
					{
						//����򿪳ɹ������ļ���Ϣ����

							// ����һ���洢�ļ�(��)��Ϣ�Ľṹ�壬�������ļ���С�ʹ���ʱ�䡢����ʱ�䡢�޸�ʱ���
						struct stat statbuf;
						// �ṩ�ļ����ַ���������ļ����Խṹ��
						stat(bookPath, &statbuf);
						int fileSize = statbuf.st_size;
						char* img = new char[fileSize];
						int num = fread(img, fileSize,1, fp);   //��ͼƬ�ļ����뵽img��
						//����֮��ر��ļ�
						fclose(fp);
						//����־λ���鼮���ϴ��ߺ��Ƿ��ǽ����鼮��������������һ�𷢸��ͻ���
						int type = _DEF_PROTOCOL_TCP_BOOKS_IMG_INFO_RS;
						int packSize = fileSize + 4 + 5 + 5;  //�ֱ��ǣ�type,bookid,isSchool
						char* imgInfo = new  char[packSize]; //�ļ���С + type��С + bookid��С +isSchool��С
						memcpy(imgInfo, (char*)&type, 4);  //�Ƚ�type�����ȥ
						memcpy(imgInfo + 4, bookid, 5);   //�ٽ�bookid��ӵ�β��
						memcpy(imgInfo + 9, isSchool, 5);  //�ٽ�isSchool�����ȥ									
						memcpy(imgInfo + 14, img, fileSize);   //�ٽ��鼮ͼƬ��Ϣ��ӵ�β��
						//���˰����͸��ͻ���
						p_pMediator->SendData(ISendIp, imgInfo, packSize);

						delete[]img;
						img = NULL;
						delete[]imgInfo;
						imgInfo = NULL;


						//ͼƬ��Ϣ�������֮����������Ϣ
						rs.bookid = atoi(bookid);
						rs.id = atoi(id);
						rs.isSchool = atoi(isSchool);
						strcpy_s(rs.bookName, bookName);
						strcpy_s(rs.bookArea, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.bookInfo, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.bookPrice, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.QQ, lst.front().c_str());
						lst.pop_front();
						strcpy_s(rs.weChat, lst.front().c_str());
						lst.pop_front();

						p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));

					}
					else   //���û�ж���ͼƬҲ��Ҫ��list�е�Ԫ��pop��������ѭ��
					{
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
					}
					//�����ͼƬ�ļ�ʧ���������һ��ѭ��


				}
			}
		}
	}

}

 //�����յ��Ļ�ȡĳ�����ϼ�������Ϣ������
void CKernel::dealAllBooksInfoRq(long ISendIp, char* buf, int nLen)
{
	//���
	STRU_TCP_SELECT_MY_BOOKS_INFO_RQ* rq = (STRU_TCP_SELECT_MY_BOOKS_INFO_RQ*)buf;
	list<string> lst;
	char sqlBuf[1024] = "";
		sprintf_s(sqlBuf, "select * from schoolbooksInfo where id = %d;", rq->id);  //��ѯ�������Ŀ��id�������鼮��Ϣ
		if (!sql.SelectMySql(sqlBuf, 9, lst))   //����9������
		{
			cout << "��ѯ���ݿ�ʧ��" << endl;
		}
		else {
			//���鼮��Ϣ���͸��ͻ���
			while (lst.size() > 0)
			{
				STRU_TCP_BOOKS_TEXT_INFO_RS rs;
				char bookid[5] = "";
				strcpy_s(bookid, lst.front().c_str());
				lst.pop_front();
				char id[5] = "";
				strcpy_s(id, lst.front().c_str());
				lst.pop_front();
				char isSchool[5] = "";
				strcpy_s(isSchool, lst.front().c_str());
				lst.pop_front();
				char bookName[_DEF_BOOK_NAME_SIZE];
				strcpy_s(bookName, lst.front().c_str());
				lst.pop_front();

				//����id��bookNameƴ���鼮ͼƬ��ַ
				char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
				strcat_s(bookPath, id);
				strcat_s(bookPath, bookName);
				strcat_s(bookPath, ".jpg");   //���ļ�·��ƴ����
				FILE* fp;
				if (0 == fopen_s(&fp, bookPath, "rb"))  //�Զ����ƶ��ķ�ʽ���ļ�
				{
					//����򿪳ɹ������ļ���Ϣ����

						// ����һ���洢�ļ�(��)��Ϣ�Ľṹ�壬�������ļ���С�ʹ���ʱ�䡢����ʱ�䡢�޸�ʱ���
					struct stat statbuf;
					// �ṩ�ļ����ַ���������ļ����Խṹ��
					stat(bookPath, &statbuf);
					int fileSize = statbuf.st_size;
					char* img = new char[fileSize];
					int num = fread(img, fileSize,1, fp);   //��ͼƬ�ļ����뵽img��
					//����֮��ر��ļ�
					fclose(fp);
					//����־λ���鼮���ϴ��ߺ��Ƿ��ǽ����鼮��������������һ�𷢸��ͻ���
					int type = _DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_IMG_INFO_RS;
					int packSize = fileSize + 4 + 5 + 5;  //�ֱ��ǣ�type,bookid,isSchool
					char* imgInfo = new  char[packSize]; //�ļ���С + type��С + bookid��С +isSchool��С
					memcpy(imgInfo, (char*)&type, 4);  //�Ƚ�type�����ȥ
					memcpy(imgInfo + 4, bookid, 5);   //�ٽ�bookid��ӵ�β��
					memcpy(imgInfo + 9, isSchool, 5);  //�ٽ�isSchool�����ȥ									
					memcpy(imgInfo + 14, img, fileSize);   //�ٽ��鼮ͼƬ��Ϣ��ӵ�β��
					//���˰����͸��ͻ���
					p_pMediator->SendData(ISendIp, imgInfo, packSize);

					delete[]img;
					img = NULL;
					delete[]imgInfo;
					imgInfo = NULL;


					//ͼƬ��Ϣ�������֮����������Ϣ
					rs.bookid = atoi(bookid);
					rs.id = atoi(id);
					rs.isSchool = atoi(isSchool);
					strcpy_s(rs.bookName, bookName);
					strcpy_s(rs.bookArea, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.bookInfo, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.bookPrice, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.QQ, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.weChat, lst.front().c_str());
					lst.pop_front();
					rs.type = _DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_TEXT_INFO_RS;
					p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));

				}
				else   //���û�ж���ͼƬҲ��Ҫ��list�е�Ԫ��pop��������ѭ��
				{
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
				}
				//�����ͼƬ�ļ�ʧ���������һ��ѭ��


			}
		}



		sprintf_s(sqlBuf, "select * from noschoolbooksInfo where id = %d;", rq->id);  //��ѯ��У���鼮����Ŀ��id�������鼮��Ϣ
		if (!sql.SelectMySql(sqlBuf, 9, lst))   //����9������
		{
			cout << "��ѯ���ݿ�ʧ��" << endl;
		}
		else {
			//���鼮��Ϣ���͸��ͻ���
			while (lst.size() > 0)
			{
				STRU_TCP_BOOKS_TEXT_INFO_RS rs;
				char bookid[5] = "";
				strcpy_s(bookid, lst.front().c_str());
				lst.pop_front();
				char id[5] = "";
				strcpy_s(id, lst.front().c_str());
				lst.pop_front();
				char isSchool[5] = "";
				strcpy_s(isSchool, lst.front().c_str());
				lst.pop_front();
				char bookName[_DEF_BOOK_NAME_SIZE];
				strcpy_s(bookName, lst.front().c_str());
				lst.pop_front();

				//����id��bookNameƴ���鼮ͼƬ��ַ
				char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
				strcat_s(bookPath, id);
				strcat_s(bookPath, bookName);
				strcat_s(bookPath, ".jpg");   //���ļ�·��ƴ����
				FILE* fp;
				if (0 == fopen_s(&fp, bookPath, "rb"))  //�Զ����ƶ��ķ�ʽ���ļ�
				{
					//����򿪳ɹ������ļ���Ϣ����

						// ����һ���洢�ļ�(��)��Ϣ�Ľṹ�壬�������ļ���С�ʹ���ʱ�䡢����ʱ�䡢�޸�ʱ���
					struct stat statbuf;
					// �ṩ�ļ����ַ���������ļ����Խṹ��
					stat(bookPath, &statbuf);
					int fileSize = statbuf.st_size;
					char* img = new char[fileSize];
					int num = fread(img, fileSize,1, fp);   //��ͼƬ�ļ����뵽img��
					//����֮��ر��ļ�
					fclose(fp);
					//����־λ���鼮���ϴ��ߺ��Ƿ��ǽ����鼮��������������һ�𷢸��ͻ���
					int type = _DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_IMG_INFO_RS;
					int packSize = fileSize + 4 + 5 + 5;  //�ֱ��ǣ�type,bookid,isSchool
					char* imgInfo = new  char[packSize]; //�ļ���С + type��С + bookid��С +isSchool��С
					memcpy(imgInfo, (char*)&type, 4);  //�Ƚ�type�����ȥ
					memcpy(imgInfo + 4, bookid, 5);   //�ٽ�bookid��ӵ�β��
					memcpy(imgInfo + 9, isSchool, 5);  //�ٽ�isSchool�����ȥ									
					memcpy(imgInfo + 14, img, fileSize);   //�ٽ��鼮ͼƬ��Ϣ��ӵ�β��
					//���˰����͸��ͻ���
					p_pMediator->SendData(ISendIp, imgInfo, packSize);

					delete[]img;
					img = NULL;
					delete[]imgInfo;
					imgInfo = NULL;


					//ͼƬ��Ϣ�������֮����������Ϣ
					rs.bookid = atoi(bookid);
					rs.id = atoi(id);
					rs.isSchool = atoi(isSchool);
					strcpy_s(rs.bookName, bookName);
					strcpy_s(rs.bookArea, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.bookInfo, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.bookPrice, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.QQ, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.weChat, lst.front().c_str());
					lst.pop_front();
					rs.type = _DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_TEXT_INFO_RS;
					p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));

				}
				else   //���û�ж���ͼƬҲ��Ҫ��list�е�Ԫ��pop��������ѭ��
				{
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
				}
				//�����ͼƬ�ļ�ʧ���������һ��ѭ��


			}
		}




		sprintf_s(sqlBuf, "select * from othergoodsinfo where id = %d;", rq->id);  //��ѯ������Ʒ����Ŀ��id�������鼮��Ϣ
		if (!sql.SelectMySql(sqlBuf, 9, lst))   //����9������
		{
			cout << "��ѯ���ݿ�ʧ��" << endl;
		}
		else {
			//���鼮��Ϣ���͸��ͻ���
			while (lst.size() > 0)
			{
				STRU_TCP_BOOKS_TEXT_INFO_RS rs;
				char bookid[5] = "";
				strcpy_s(bookid, lst.front().c_str());
				lst.pop_front();
				char id[5] = "";
				strcpy_s(id, lst.front().c_str());
				lst.pop_front();
				char isSchool[5] = "";
				strcpy_s(isSchool, lst.front().c_str());
				lst.pop_front();
				char bookName[_DEF_BOOK_NAME_SIZE];
				strcpy_s(bookName, lst.front().c_str());
				lst.pop_front();

				//����id��bookNameƴ���鼮ͼƬ��ַ
				char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
				strcat_s(bookPath, id);
				strcat_s(bookPath, bookName);
				strcat_s(bookPath, ".jpg");   //���ļ�·��ƴ����
				FILE* fp;
				if (0 == fopen_s(&fp, bookPath, "rb"))  //�Զ����ƶ��ķ�ʽ���ļ�
				{
					//����򿪳ɹ������ļ���Ϣ����

						// ����һ���洢�ļ�(��)��Ϣ�Ľṹ�壬�������ļ���С�ʹ���ʱ�䡢����ʱ�䡢�޸�ʱ���
					struct stat statbuf;
					// �ṩ�ļ����ַ���������ļ����Խṹ��
					stat(bookPath, &statbuf);
					int fileSize = statbuf.st_size;
					char* img = new char[fileSize];
					int num = fread(img, fileSize,1, fp);   //��ͼƬ�ļ����뵽img��
					//����֮��ر��ļ�
					fclose(fp);
					//����־λ���鼮���ϴ��ߺ��Ƿ��ǽ����鼮��������������һ�𷢸��ͻ���
					int type = _DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_IMG_INFO_RS;
					int packSize = fileSize + 4 + 5 + 5;  //�ֱ��ǣ�type,bookid,isSchool
					char* imgInfo = new  char[packSize]; //�ļ���С + type��С + bookid��С +isSchool��С
					memcpy(imgInfo, (char*)&type, 4);  //�Ƚ�type�����ȥ
					memcpy(imgInfo + 4, bookid, 5);   //�ٽ�bookid��ӵ�β��
					memcpy(imgInfo + 9, isSchool, 5);  //�ٽ�isSchool�����ȥ									
					memcpy(imgInfo + 14, img, fileSize);   //�ٽ��鼮ͼƬ��Ϣ��ӵ�β��
					//���˰����͸��ͻ���
					p_pMediator->SendData(ISendIp, imgInfo, packSize);

					delete[]img;
					img = NULL;
					delete[]imgInfo;
					imgInfo = NULL;


					//ͼƬ��Ϣ�������֮����������Ϣ
					rs.bookid = atoi(bookid);
					rs.id = atoi(id);
					rs.isSchool = atoi(isSchool);
					strcpy_s(rs.bookName, bookName);
					strcpy_s(rs.bookArea, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.bookInfo, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.bookPrice, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.QQ, lst.front().c_str());
					lst.pop_front();
					strcpy_s(rs.weChat, lst.front().c_str());
					lst.pop_front();
					rs.type = _DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_TEXT_INFO_RS;
					p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));

				}
				else   //���û�ж���ͼƬҲ��Ҫ��list�е�Ԫ��pop��������ѭ��
				{
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
				}
				//�����ͼƬ�ļ�ʧ���������һ��ѭ��


			}
		}
	
}

//�����¼�Ŀ���鼮��Ϣ������
void CKernel::dealDeleteBooksInfoRq(long ISendIp, char* buf, int nLen)
{
	//���
	STRU_TCP_DELETE_BOOKS_INFO_RQ* rq = (STRU_TCP_DELETE_BOOKS_INFO_RQ*)buf;
	list<string> lst;
	char sqlBuf[1024] = "";
	STRU_TCP_DELETE_BOOKS_INFO_RS rs;
	rs.isSchool = rq->isSchool;
	//���ȸ���id��bookNameƴ���ļ���ַ��
	char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
	char id[5] = "";
	_itoa_s(rq->id, id, 10);
	strcat_s(bookPath, id);
	strcat_s(bookPath, rq->bookName);
	strcat_s(bookPath, ".jpg");
	//���Դ򿪣�����򿪳ɹ���ɾ��
	FILE* fp;
	if (0 == fopen_s(&fp, bookPath, "rb"))
	{
		//����򿪳ɹ������������sql����Ƿ���ִ�гɹ�
		//�ȹر�
		fclose(fp);


		if (SCHOOL_BOOK == rq->isSchool)
		{
			//���Ȳ�ѯ�Ȿ���bookid,Ȼ����ɾ��
			sprintf_s(sqlBuf, "select bookid from schoolbooksinfo where id = %d and bookName = '%s';", rq->id, rq->bookName);  //ɾ��Ŀ����е�Ŀ���鼮��Ϣ
			if (sql.SelectMySql(sqlBuf, 1, lst))  //������ݿ��ѯ�ɹ�
			{
				//ɾ�����ݿ�
				sprintf_s(sqlBuf, "delete from schoolbooksinfo where id = %d and bookName = '%s';", rq->id, rq->bookName);  //ɾ��Ŀ����е�Ŀ���鼮��Ϣ
			//���ɾ���ɹ�����ɾ���ļ���Ȼ�����¼ܳɹ��İ����ͻ���
				if (sql.UpdateMySql(sqlBuf))  //������ݿ�ɾ���ɹ�
				{
					//ɾ���ļ�
					remove(bookPath);
					//����ɾ���ɹ�����Ϣ���ͻ���
					rs.isDeleteSuccess = DELETE_SUCCESS;
					rs.bookid = atoi(lst.front().c_str());
					lst.pop_front();
				}
				else {
					//���ɾ��ʧ�ܡ�
					rs.isDeleteSuccess = DELETE_DEFAULT;
				}
			}
			else {
				//�����ѯʧ��
				rs.isDeleteSuccess = DELETE_DEFAULT;
			}
		}
		else if (NO_SCHOOL_BOOK == rq->isSchool)
		{
			//���Ȳ�ѯ�Ȿ���bookid,Ȼ����ɾ��
			sprintf_s(sqlBuf, "select bookid from noschoolbooksinfo where id = %d and bookName = '%s';", rq->id, rq->bookName);  //ɾ��Ŀ����е�Ŀ���鼮��Ϣ
			if (sql.SelectMySql(sqlBuf, 1, lst))  //������ݿ��ѯ�ɹ�
			{
				//ɾ�����ݿ�
				sprintf_s(sqlBuf, "delete from noschoolbooksinfo where id = %d and bookName = '%s';", rq->id, rq->bookName);  //ɾ��Ŀ����е�Ŀ���鼮��Ϣ
			//���ɾ���ɹ�����ɾ���ļ���Ȼ�����¼ܳɹ��İ����ͻ���
				if (sql.UpdateMySql(sqlBuf))  //������ݿ�ɾ���ɹ�
				{
					//ɾ���ļ�
					remove(bookPath);
					//����ɾ���ɹ�����Ϣ���ͻ���
					rs.isDeleteSuccess = DELETE_SUCCESS;
					rs.bookid = atoi(lst.front().c_str());
					lst.pop_front();
				}
				else {
					//���ɾ��ʧ�ܡ�
					rs.isDeleteSuccess = DELETE_DEFAULT;
				}
			}
			else {
				//�����ѯʧ��
				rs.isDeleteSuccess = DELETE_DEFAULT;
			}
		}

		else if (OTHER_GOODS == rq->isSchool)
		{
			//���Ȳ�ѯ�Ȿ���bookid,Ȼ����ɾ��
			sprintf_s(sqlBuf, "select bookid from othergoodsinfo where id = %d and bookName = '%s';", rq->id, rq->bookName);  //ɾ��Ŀ����е�Ŀ���鼮��Ϣ
			if (sql.SelectMySql(sqlBuf, 1, lst))  //������ݿ��ѯ�ɹ�
			{
				//ɾ�����ݿ�
				sprintf_s(sqlBuf, "delete from othergoodsinfo where id = %d and bookName = '%s';", rq->id, rq->bookName);  //ɾ��Ŀ����е�Ŀ���鼮��Ϣ
			//���ɾ���ɹ�����ɾ���ļ���Ȼ�����¼ܳɹ��İ����ͻ���
				if (sql.UpdateMySql(sqlBuf))  //������ݿ�ɾ���ɹ�
				{
					//ɾ���ļ�
					remove(bookPath);
					//����ɾ���ɹ�����Ϣ���ͻ���
					rs.isDeleteSuccess = DELETE_SUCCESS;
					rs.bookid = atoi(lst.front().c_str());
					lst.pop_front();
				}
				else {
					//���ɾ��ʧ�ܡ�
					rs.isDeleteSuccess = DELETE_DEFAULT;
				}
			}
			else {
				//�����ѯʧ��
				rs.isDeleteSuccess = DELETE_DEFAULT;
			}
		}




	}
	else {   //����򲻿�
		rs.isDeleteSuccess = DELETE_DEFAULT;  //ɾ��ʧ��

	}

	strcpy_s(rs.bookName, rq->bookName);
	p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));
}

//�����յ��ĸ���ͷ�������
void CKernel::dealAlterIconRq(long ISendIp, char* buf, int nLen)
{
	//ǰ�ĸ��ֽ���type
	//4~7�ֽ��ǿͻ���id
	//�����ͷ��ͼƬ��Ϣ
	int id = *(((int*)buf) + 1);
	//ͷ��·��Ϊ  .//icon//id.jpg
	char iconPath[30] = ".//icon//";
	char pid[5];
	_itoa_s(id, pid, 10);  //��idת�����ַ�����ʽ
	strcat_s(iconPath, pid);
	strcat_s(iconPath, ".jpg");   //���ַ���ĩβ���
	

	STRU_TCP_ALTER_ICON_RS rs;
	//��ͼƬ��Ϣ�����ļ�
	FILE* fp;
	if (0 == fopen_s(&fp, iconPath, "wb"))
	{
		if (nLen > 8)
		{
			int num = fwrite(buf + 8, nLen - 8, 1, fp);
			if (num == 1)
			{
				cout << "д��ɹ�" << endl;
				//���͸��ĳɹ��Ļظ����ͻ���
				rs.isSuccess = true;
			}
			else
			{
				//���͸���ʧ�ܵĻظ����ͻ���
				rs.isSuccess = false;
			}
		}
		else
		{
			//���͸���ʧ�ܵĻظ����ͻ���
			rs.isSuccess = false;
		}
		fclose(fp);
	}
	else {
		//���͸���ʧ�ܵĻظ����ͻ���
		rs.isSuccess = false;
	}

	p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));

}



//��������ͨ����Ϣ��
void CKernel::dealVoidInfo(long ISendIp, char* buf, int nLen)
{
	STRU_VOID_TRAN* rq = (STRU_VOID_TRAN*)buf;
	if (m_mapIdToSockAddr_in.count(rq->ISendIp))
	{
		m_pUdpMediator->SendData(rq->ISendIp, buf, nLen);
	}
}

