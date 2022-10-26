#include "CKernel.h"
#include "tcpServerMediator.h"
#include "UdpMediator.h"
#include<list>
//#include <unistd.h>
//#include <sys/stat.h>
//#include <fcntl.h>


//定义计算数组下标的宏
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

//打开服务器
bool CKernel::startServer()
{
	cout << "CKernel::startServer" << endl;
	//1;连接数据库
	if (!sql.ConnectMySql("127.0.0.1", "root", "mmm959825", "0103sql")) {
		cout << "连接数据库失败" << endl;
		return false;
	}
	//2:打开网络
	if (!p_pMediator->OpenNet()) {
		cout << "打开网络失败" << endl;
		return false;
	}
	if (!m_pUdpMediator->OpenNet()) {
		cout << "打开网络失败" << endl;
		return false;
	}
	return true;
}

//关闭服务器
void CKernel::closeServer()
{
	cout << "CKernel::closeServer" << endl;
	//1:断开数据库连接
	sql.DisConnect();
	//2:关闭网络，回收对象
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

//初始化协议映射表
void CKernel::setProtocolMap()
{

	cout << "CKernel::setProtocolMap" << endl;
	//1:初始化数组
	memset(m_netProtocolMap, 0, sizeof(m_netProtocolMap));

	//2:绑定协议头和对应的处理函数
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

//获取用户的好友信息（包括自己）
void CKernel::getUserFriendInfo(int userId)
{
	//1:获取自己的信息
	STRU_TCP_FRIEND_INFO userInfo;
	getUserInfoById(&userInfo, userId);

	//2：把自己的消息发给客户端
	if (m_mapIdToSocket.find(userId) == m_mapIdToSocket.end()) {
		cout << "用户的套接字不存在" << endl;
		return;
	}
	p_pMediator->SendData(m_mapIdToSocket[userId], (char*)&userInfo, sizeof(userInfo));

	//3:查询自己的好友的id列表
	list<string> listRest;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select idB from t_friend where idA = '%d';", userId);
	if (!sql.SelectMySql(sqlBuf, 1, listRest)) {
		cout << "查询数据库失败,sql: " << sqlBuf << endl;
		return;
	}

	//4:遍历查询结果
	int friendId = 0;
	while (listRest.size() > 0) {
		//5:获取好友的id
		friendId = atoi(listRest.front().c_str());
		listRest.pop_front();
		STRU_TCP_FRIEND_INFO friendInfo;;
		//6:根据好友的id获取好友的信息
		getUserInfoById(&friendInfo, friendId);

		//把好友的信息发给客户端
		p_pMediator->SendData(m_mapIdToSocket[userId], (char*)&friendInfo, sizeof(friendInfo));

		//如果好友在线，给好友发送当前客户端上线的通知
		if (m_mapIdToSocket.find(friendId) != m_mapIdToSocket.end()) {
			p_pMediator->SendData(m_mapIdToSocket[friendId], (char*)&userInfo, sizeof(userInfo));
		}
	}
}


//根据id获取用户信息
void CKernel::getUserInfoById(STRU_TCP_FRIEND_INFO* info, int id)
{
	//查询好友信息
	list<string> listRest;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select name, feeling, icon from t_user where id = '%d';", id);
	if (!sql.SelectMySql(sqlBuf, 3, listRest)) {
		cout << "查询数据失败，sql: " << sqlBuf << endl;
		return;
	}

	//给结构体赋值
	if (listRest.size() > 0) {
		//取昵称
		strcpy_s(info->name, listRest.front().c_str());
		listRest.pop_front();

		//取签名
		strcpy_s(info->feeling, listRest.front().c_str());
		listRest.pop_front();

		//取头像id
		info->iconId = atoi(listRest.front().c_str());
		listRest.pop_front();
	}
	info-> userId = id;
	if (m_mapIdToSocket.count(id) > 0) {
		//在线
		info->status = status_online;
	}
	else
	{
		//不在线
		info->status = status_offline;
	}
}

//处理所有接收到的数据
void CKernel::dealReadyData(long ISendIp, char* buf, int nLen)
{
	cout << "CKernel::dealReadyData" << endl;
	//1:取出协议头
	PackType type = *(PackType*)buf;

	//2:判断协议头是否在数组下标的有效范围内
	if (type >= _DEF_PROTOCOL_BASE + 1 && type <= _DEF_PROTOCOL_BASE + _DEF_PROTOCOL_COUNT) {
		//3:计算数组下标，取出下标对应的函数地址
		PFUN pf = NetProFunMap(type);
		//4:判断指针不为空，就调用指针指向的函数
		if (pf) {
			(this->*pf)(ISendIp, buf, nLen);
		}
	}
}

//处理注册请求
void CKernel::dealRegisterRq(long lSendIp, char* buf, int nLen)
{
	cout << "CKernel::dealRegisterRq" << endl;
	//1:拆包
	STRU_TCP_REGISTER_RQ* rq = (STRU_TCP_REGISTER_RQ*)buf;
	cout << "tel: " << rq->tel << endl;
	cout << "name:" << rq->name << endl;
	cout << "password:" << rq->password << endl;

	//2：检验参数是否合法，校验规则同客户端一致（省略）
	//3：查询手机号是否已经注册过
	list<string> listRest;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select tel from t_user where tel = '%s';", rq->tel);   //以给定格式写入字符数组
	if (!sql.SelectMySql(sqlBuf, 1, listRest)) {
		cout << "查询数据库失败, sql:" << sqlBuf << endl;
		return;
	}

	STRU_TCP_REGISTER_RS rs;
	if (listRest.size() > 0) {
		//手机号已经注册过，回复客户端
		rs.result = user_is_exist;
	}
	else {
		//手机号没有注册过
		//4：查询昵称是否已经注册过
		sprintf_s(sqlBuf, "select name from t_user where name = '%s';", rq->name);
		if (!sql.SelectMySql(sqlBuf, 1, listRest))
		{
			cout << "查询数据库失败, sql:" << sqlBuf << endl;
			return;
		}

		if (listRest.size() > 0) {
			//昵称已经注册过，回复客户端
			rs.result = user_is_exist;
		}
		else
		{
			//昵称没有注册过
			//5:存入数据库
			sprintf_s(sqlBuf, "insert into t_user (tel, password, name, icon, feeling) values ('%s', '%s', '%s', 1, '啥也没写');",
				rq->tel, rq->password, rq->name);
			if (!sql.UpdateMySql(sqlBuf)) {
				cout << "插入数据库失败，sql: " << sqlBuf << endl;
				return;
			}
			rs.result = register_sucess;
		}
	}

	//给客户端回复注册结果
	p_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
}

//处理登录请求   TODO:发送头像消息给客户端
void CKernel::dealLoginRq(long lSendIp, char* buf, int nLen)
{
	cout << "CKernel::dealLoginRq" << endl;
	//1:拆包
	STRU_TCP_LOGIN_RQ* rq = (STRU_TCP_LOGIN_RQ*)buf;
	cout << "tel: " << rq->tel << endl;
	cout << "password:" << rq->password << endl;



	STRU_TCP_LOGIN_RS rs;
	//2:查询用户是否存在
	list<string> listRest;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select id, password from t_user where tel = '%s';", rq->tel);
	if (!sql.SelectMySql(sqlBuf, 2, listRest)) {   //  参数 2 是指要查几列数据
		cout << "查询数据库失败, sql: " << sqlBuf << endl;
		return;
	}
	if (0 == listRest.size()) {
		//如果用户不存在
		rs.result = user_not_exist;
	}
	else {
		//如果用户存在
		// 先取出用户id
		int userId = atoi(listRest.front().c_str());  //atoi():string类型转int型
		listRest.pop_front();
		//再取密码
		string password = listRest.front();
		listRest.pop_front();  //每次数据取出后就给链表里的移除
		//3：判断用户输入的密码是否正确
		if (strcmp(rq->password,password.c_str()) != 0) {   //strcmp（）函数用于比较两个字符串是否相等
			//密码错误
			rs.result = password_error;

		}
		else {
			//密码正确
			rs.result = login_success;
			//保存用户id和对应的socket

			m_mapIdToSocket[userId] = lSendIp;
			//m_mapIdToSocket.insert(pair<int, SOCKET>(userId, lSendIp));

			rs.userId = userId;
			//给客户端回复登录结果
			p_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));

			//获取当前登录用户的好友信息（包括自己）
			getUserFriendInfo(userId);
			return;
		}
	}
	//4:给客户端回复登录结果
	p_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
	
}

void CKernel::dealChatRq(long lSendIp, char* buf, int nLen)
{
	
		cout << "CKernel::dealChatRq " << endl;
		//1:拆包
		STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)buf;

		//2:判断对端用户是否在线
		if (m_mapIdToSocket.count(rq->friendId) > 0) {
			//3:如果在线，就转发聊天请求
			p_pMediator->SendData(m_mapIdToSocket[rq->friendId], buf, nLen);
		}
		else
		{
			//4:如果不在线，就回复客户端好友不在线
			STRU_TCP_CHAT_RS rs;
			rs.friendId = rq->friendId;
			rs.userId = rq->userId;
			rs.result = send_error;
			p_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
		}

}

//处理下线请求
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
		//给他的好友发送他的下线通知：
		//查询所有好友
		list<string> listRest;
		char sqlBuf[1024] = "";
		sprintf_s(sqlBuf, "select idB from t_friend where idA = %d;", rq->userId);
		if (!sql.SelectMySql(sqlBuf, 1, listRest)) {   //  参数 2 是指要查几列数据
			cout << "查询数据库失败, sql: " << sqlBuf << endl;
			return;
		}
		//遍历好友发送下线通知
		int friendId = 0;
		while (0 < listRest.size())
		{
			friendId = atoi(listRest.front().c_str());   //atoi():char*转数字
			listRest.pop_front();
			//判断好友是否在线，如果在线则发送下线通知
			if (0 < m_mapIdToSocket.count(friendId))
			{
				p_pMediator->SendData(long(m_mapIdToSocket[friendId]), buf, nLen);
			}
		}
	}
}

void CKernel::dealaddFriendRq(long lSendIp, char* buf, int nLen)
{
	//拆包：
	STRU_TCP_ADD_FRIEND_RQ* rq = (STRU_TCP_ADD_FRIEND_RQ*)buf;
	//首先在数据库中查找看是否有此人：
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
		//发送添加好友回复告诉对方没有这个人
		STRU_TCP_ADD_FRIEND_RS rs;
		rs.result = no_this_user;
		p_pMediator->SendData(m_mapIdToSocket[lSendIp], (char*)&rs, sizeof(rs));
	}
	//判断对方是否在线
	if (0 < m_mapIdToSocket.count(friendId))
	{
		//如果在线，向他发送好友请求
		p_pMediator->SendData(m_mapIdToSocket[friendId], buf, nLen);
	}
	else {
		//如果不在线，给添加者发送添加好友回复
		STRU_TCP_ADD_FRIEND_RS rs;
		rs.result = user_notlogin;
		p_pMediator->SendData(m_mapIdToSocket[lSendIp], (char*)&rs, sizeof(rs));
	}

}

//处理添加好友回复
void CKernel::dealaddFriendRs(long lSendIp, char* buf, int nLen)
{
	cout << "处理添加好友请求" << endl;
	STRU_TCP_ADD_FRIEND_RS* rs = (STRU_TCP_ADD_FRIEND_RS*)buf;
	//1:判断添加好友是否成功
	if (add_success == rs->result)
	{
		//写入数据库（注意写入数据库是双向的，即两边分别称为一次idA）
		char sqlBuf[1024] = "";
		sprintf_s(sqlBuf, "start transaction;");
		if (!sql.UpdateMySql(sqlBuf))  //这里只是借用这个函数执行传递信息，其实底层都是用mysql_query实现的，都是将sql语句送到数据库执行
		{
			cout << "添加好友失败" << endl;
			return ;
		}
		memset(sqlBuf, 0, 1024);
		sprintf_s(sqlBuf, "insert into t_friend values ('%d' , '%d' );", rs->userId, rs->friendId);
		if (!sql.UpdateMySql(sqlBuf))  //这里只是借用这个函数执行传递信息，其实底层都是用mysql_query实现的，都是将sql语句送到数据库执行
		{
			cout << "添加好友失败" << endl;
			return;
		}
		memset(sqlBuf, 0, 1024);
		sprintf_s(sqlBuf, "insert into t_friend values ( '%d' , '%d' );", rs->friendId, rs->userId);
		if (!sql.UpdateMySql(sqlBuf))  //这里只是借用这个函数执行传递信息，其实底层都是用mysql_query实现的，都是将sql语句送到数据库执行
		{
			cout << "添加好友失败" << endl;
			return;
		}
		//memset(sqlBuf, 0, 1024);
		//sprintf_s(sqlBuf,"rollback;");  //rollback是事务中的回滚操作
		//if (!sql.UpdateMySql(sqlBuf))  //这里只是借用这个函数执行传递信息，其实底层都是用mysql_query实现的，都是将sql语句送到数据库执行
		//{
		//	cout << "添加好友失败" << endl;
		//	return;
		//}
		memset(sqlBuf, 0, 1024);
		sprintf_s(sqlBuf, "commit;");
		if (!sql.UpdateMySql(sqlBuf))  //这里只是借用这个函数执行传递信息，其实底层都是用mysql_query实现的，都是将sql语句送到数据库执行
		{
			cout << "添加好友失败" << endl;
			return;
		}

		//添加成功后发信息告诉friendId；
		if (0 < m_mapIdToSocket.count(rs->friendId))
			p_pMediator->SendData(m_mapIdToSocket[rs->friendId], buf, nLen);

		//并将新好友的信息发送给对方
		STRU_TCP_FRIEND_INFO userinfo;
		getUserInfoById(&userinfo, rs->friendId);
		if (0 < m_mapIdToSocket.count(rs->userId))
			p_pMediator->SendData(m_mapIdToSocket[rs->userId], (char*)&userinfo, sizeof(userinfo));

		getUserInfoById(&userinfo, rs->userId);
		if (0 < m_mapIdToSocket.count(rs->friendId))
			p_pMediator->SendData(m_mapIdToSocket[rs->friendId], (char*)&userinfo, sizeof(userinfo));

		/*
		事务样例：
		start transaction;
		update bank set money = money -50 where name = '狄仁杰';
		update bank set money = money + 50 where name = '李元芳';
		select * from bank;

		rollback;

		commit;
		*/
	}
	else {
		//添加失败
		if (0 < m_mapIdToSocket.count(rs->friendId))
			p_pMediator->SendData(m_mapIdToSocket[rs->friendId], buf, nLen);
	}
}

//============================================================================================
void CKernel::dealReadyUdpData(long lSendIp, char* buf, int nLen)
{
	//根据数据类型，调用不同的函数（可以与处理tcp数据的函数合并，但为了看起来更直观，就分开写了）
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

//处理udp上线请求
void CKernel::dealUdpOnlineRq(long lSendIp, char* buf, int nLen)
{
	cout << lSendIp << "dealUdpOnlineRq" << endl;

	STRU_ONLINE* rq = (STRU_ONLINE*)buf;
	m_mapIdToSockAddr_in[rq->m_id] = rq->m_id;

}

//处理udp聊天请求
void CKernel::dealUdpChatRq(long lSendIp, char* buf, int nLen)
{
	//1：拆包，查看是发给谁的
	STRU_CHAT* rq = (STRU_CHAT*)buf;

	if (0 >= m_mapIdToSockAddr_in.count(rq->ISendIp))
	{
		cout << rq->ISendIp << "不存在" << endl;
		return;
	}
	cout << "发给：" << m_mapIdToSockAddr_in[rq->ISendIp] << endl;

	m_pUdpMediator->SendData(m_mapIdToSockAddr_in[rq->ISendIp], buf, nLen);

}

//处理udp生命包请求
void CKernel::dealLifeRq(long lSendIp, char* buf, int nLen)
{
	STRU_OFFLINE* rq = (STRU_OFFLINE*)buf;  //因为生命包并无实质内容，所以生命包是由下线请求包改的，即type改变，
	m_mapIdToSockAddr_in[rq->m_id] = lSendIp;
	return;
}

//处理文件信息请求或者回复
void CKernel::dealFileInfoRq_Rs(long ISendIp, char* buf, int nLen)
{

}

//处理视频通话请求
void CKernel::dealVideoRq(long ISendIp, char* buf, int nLen)
{
	STRU_VIDEO_RQ* rq = (STRU_VIDEO_RQ*)buf;
	//查看是给谁发送的视频通话请求包
	cout << "这是一个视频通话请求包" << endl;
	if (m_mapIdToSocket.count(rq->ISendIp))
	{
		p_pMediator->SendData(m_mapIdToSocket[rq->ISendIp], buf, nLen);
	}
}

//处理视频通话回复
void CKernel::dealVideoRs(long ISendIp, char* buf, int nLen)
{
	STRU_VIDEO_RS* rq = (STRU_VIDEO_RS*)buf;
	cout << "这是一个视频通话回复包" << endl;
	//查看是给谁发送的视频通话请求包
	if (m_mapIdToSocket.count(rq->ISendIp))
	{
		p_pMediator->SendData(m_mapIdToSocket[rq->ISendIp], buf, nLen);
	}
}

//处理视频通话包传输
void CKernel::dealVideoInfo(long ISendIp, char* buf, int nLen)
{
	cout << "这是一个视频信息包" << endl;
	if (m_mapIdToSockAddr_in.count(*(((int*)buf) + 1)))   //这里直接对指针操作，数据包中的第二个四字节空间存放的就是目标主机id
	{
		m_pUdpMediator->SendData(m_mapIdToSockAddr_in[*(((int*)buf) + 1)], buf, nLen);
	}
}

//处理语音通话请求
void CKernel::dealVoidRq(long ISendIp, char* buf, int nLen)
{
	STRU_VOID_RQ* rq = (STRU_VOID_RQ*)buf;
	if (m_mapIdToSocket.count(rq->ISendIp))
	{
		p_pMediator->SendData(m_mapIdToSocket[rq->ISendIp], buf, nLen);
	}
}

//处理语音通话回复
void CKernel::dealVoidRs(long ISendIp, char* buf, int nLen)
{
	STRU_VOID_RS* rs = (STRU_VOID_RS*)buf;
	if (m_mapIdToSocket.count(rs->ISendIp))
	{
		p_pMediator->SendData(m_mapIdToSocket[rs->ISendIp], buf, nLen);
	}
}

//处理挂断语音通话申请
void CKernel::dealCloseVoid(long ISendIp, char* buf, int nLen)
{
	cout << "这是一个挂断语音通话的申请" << endl;
	STRU_CLOSE_VOID* rq = (STRU_CLOSE_VOID*)buf;
	p_pMediator->SendData(m_mapIdToSocket[rq->ISendIp], buf, nLen);
}

//处理书籍上架申请（书籍信息）
void CKernel::dealBooksImgInfo(long ISendIp, char* buf, int nLen)
{

	//先取出书籍名称和上传者编号
	int id = *((int*)buf+1);    //最前面是type，所以这里要加1
	char bookName[_DEF_BOOK_NAME_SIZE];
	memcpy(bookName, (int*)buf + 2, _DEF_BOOK_NAME_SIZE);  //从第三个字节开始后面的_DEF_BOOK_NAME_SIZE个字节是书名
	char* bookImg = buf + 4 + 4 + _DEF_BOOK_NAME_SIZE;    //跳过type,id,bookName
	int bookImgSize = nLen - 4 - 4 - _DEF_BOOK_NAME_SIZE;  //减去type，id，和bookName
	cout << id << "         " << bookName << endl;
	

	//将图片信息存入文件夹
	//先将文件名拼出来
	char fileName[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
	char num[5];
	cout << id << endl;
	_itoa_s(id, num,10);
	strcat_s(fileName, num);  //id拼接进路径
	strcat_s(fileName, bookName);  //书名拼接进路径
	strcat_s(fileName, ".jpg");  //后缀拼进；路径
	cout << fileName << endl;
	FILE* fp;
	if ( 0 == fopen_s(&fp, fileName, "rb") )  //以只读方式打开文件如果打开成功则返回0
	{
		//如果不是返回NULL，则表名该文件已经存在
		//TODO:发送一个数据包告诉上传者他已经上传过改本书籍，请勿重复上传（此处不发，收到书籍信息的时候发）

		cout << "此文件已存在" << endl;
		//将打开的文件关闭
		fclose(fp);
		return ;
	}
	//如果该文件不存在则证明数据库中没有该书籍的信息
	//将书籍图片存入文件夹中
	if (0 == fopen_s(&fp, fileName, "wb"))  //以二进制写的方式打开（创建）文件(此处也许应该以rb方式打开？此处暂不更改)
	{
		int number = fwrite(bookImg, bookImgSize, 1, fp);
		//将打开的文件关闭
		fclose(fp);
		cout << "图片大小为：" << bookImgSize << endl;
		//发送上架成功的返回包（此处不发，收到书籍信息的时候发）

	}
	/*
	size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)


可以看到这个函数的参数有四个：

第一个ptr是要写入的数据的头指针，无符号类型；

第二个参数size是大小，表示每个写入元素的大小，单位是字节；

第三个参数nmemb是个数，以上一个参数为单位的个数；

第四个参数stream就是文件指针，表示往哪里写。
	*/
}

//处理书籍上架申请（接收到的书籍的其他信息）
void CKernel::dealBooksInfo(long ISendIp, char* buf, int nLen)
{
	//拆包
	STRU_TCP_ADD_BOOKS* rq = (STRU_TCP_ADD_BOOKS*)buf;
	//根据书名和发送者编号拼出地址，查看文件夹中是否有这张书籍的图片
	char filePath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
	char id[5];
	_itoa_s(rq->id, id, 10);
	strcat_s(filePath, id);
	strcat_s(filePath, rq->bookName);
	strcat_s(filePath, ".jpg");
	cout << filePath << endl;
	//尝试打开该文件
	FILE* fp;
	STRU_TCP_ADD_BOOKS_RS rs;
	strcpy_s(rs.bookName, rq->bookName);
	if (0 == fopen_s(&fp, filePath, "rb"))  //以只读方式打开文件如果打开成功则返回0
	{
		cout << "打开成功" << endl;
		//将打开的文件关闭
		fclose(fp);
		//TODO:如果打开成功，则尝试将书籍信息写入数据库
		char sqlBuf[1024] = "";
		if (SCHOOL_BOOK == rq->isSchool)   //校内书籍   
		{                                                   //bookid是自增数据，这里不用管
			sprintf_s(sqlBuf, "insert into schoolbooksinfo (id , isSchool, bookName, bookArea, bookInfo, bookPrice, QQ, weChat) values ( %d,%d,'%s','%s','%s','%s' ,'%s','%s');", rq->id, rq->isSchool, rq->bookName, rq->bookArea, rq->bookInfo, rq->bookPrice, rq->QQ, rq->weChat);
		}
		else if (NO_SCHOOL_BOOK == rq->isSchool)   //校外书籍
		{													//bookid是自增数据，这里不用管
			sprintf_s(sqlBuf, "insert into noschoolbooksinfo (id , isSchool, bookName, bookArea, bookInfo, bookPrice, QQ, weChat) values ( %d,%d,'%s','%s','%s','%s' ,'%s','%s');", rq->id, rq->isSchool, rq->bookName, rq->bookArea, rq->bookInfo, rq->bookPrice, rq->QQ, rq->weChat);

		}
		else {   //其他物品											//bookid是自增数据，这里不用管
			sprintf_s(sqlBuf, "insert into othergoodsinfo (id , isSchool, bookName, bookArea, bookInfo, bookPrice, QQ, weChat) values ( %d,%d,'%s','%s','%s','%s' ,'%s','%s');", rq->id, rq->isSchool, rq->bookName, rq->bookArea, rq->bookInfo, rq->bookPrice, rq->QQ, rq->weChat);

		}
		if (!sql.UpdateMySql(sqlBuf)) {
			cout << "插入数据失败，sql: " << sqlBuf << endl;
			//如果插入数据失败默认其错误为与主键重复了
			//TODO:发送上架回复给用户，重复上架了
			rs.result = REPEAT_SUBMIT;
			p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));
			return;
		}
		else {
			//发送上架成功的消息给用户
			rs.result = SUBMIT_SUCCESS;
			p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));
			return;
		}


		return;
	}
	else {
		//TODO:打开失败则发送书籍上架错误信息（数据写入失败）给用户
		rs.result = SUBMIT_ERROR;
		p_pMediator->SendData(ISendIp,(char*)&rs, sizeof(rs));
	}
}

//处理收到的获取书籍信息的请求
void CKernel::dealBooksInfoRq(long ISendIp, char* buf, int nLen)
{
	STRU_TCP_BOOKS_INFO_RQ* rq = (STRU_TCP_BOOKS_INFO_RQ*)buf;
	//TODO:给他推数据库中的校内书籍，校外书籍，以及其他物品各10个（如果没有10个则按最大个数推）
	list<string> lst;
	char sqlBuf[1024] = "";

	if (SELECT_BOOK == rq->isSelect)   //如果是搜索书籍信息的请求
	{
		//判断搜索的是哪一种
		if (SCHOOL_BOOK == rq->isSchool)
		{
			sprintf_s(sqlBuf, "select * from (select * from schoolbooksinfo where bookName like '%s%s%s' ) book limit %d , 10;","%", rq->bookName,"%", rq->num);   //每次的偏移量为10,模糊查找
			if (!sql.SelectMySql(sqlBuf, 9, lst))   //表有9列数据
			{
				cout << "查询数据库失败" << endl;
			}
			else {
				//将书籍信息发送给客户端
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

					//根据id和bookName拼出书籍图片地址
					char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
					strcat_s(bookPath, id);
					strcat_s(bookPath, bookName);
					strcat_s(bookPath, ".jpg");   //将文件路径拼出来
					FILE* fp;
					if (0 == fopen_s(&fp, bookPath, "rb"))  //以二进制读的方式打开文件
					{
						//如果打开成功，则将文件信息读出

							// 这是一个存储文件(夹)信息的结构体，其中有文件大小和创建时间、访问时间、修改时间等
						struct stat statbuf;
						// 提供文件名字符串，获得文件属性结构体
						stat(bookPath, &statbuf);
						int fileSize = statbuf.st_size;
						char* img = new char[fileSize];
						int num = fread(img, fileSize,1, fp);   //将图片文件读入到img中
						//读完之后关闭文件
						fclose(fp);
						//将标志位和书籍的上传者和是否是教育书籍和书名塞进包里一起发给客户端
						int type = _DEF_PROTOCOL_TCP_SELECT_BOOKS_IMG_INFO_RS;
						int packSize = fileSize + 4 + 5 + 5;  //分别是：type,bookid,isSchool
						char* imgInfo = new  char[packSize]; //文件大小 + type大小 + bookid大小 +isSchool大小
						memcpy(imgInfo, (char*)&type, 4);  //先将type插入进去
						memcpy(imgInfo + 4, bookid, 5);   //再将bookid添加到尾部
						memcpy(imgInfo + 9, isSchool, 5);  //再将isSchool插入进去									
						memcpy(imgInfo + 14, img, fileSize);   //再将书籍图片信息添加到尾部
						//将此包发送给客户端
						p_pMediator->SendData(ISendIp, imgInfo, packSize);

						delete[]img;
						img = NULL;
						delete[]imgInfo;
						imgInfo = NULL;


						//图片信息发送完毕之后发送文字信息
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
					else   //如果没有读到图片也需要将list中的元素pop，避免死循环
					{
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
					}
					//如果打开图片文件失败则进行下一次循环


				}
			}
		}
		else if (NO_SCHOOL_BOOK == rq->isSchool)  //如果是校外书籍
		{
			sprintf_s(sqlBuf, "select * from (select * from noschoolbooksinfo where bookName like '%s%s%s' ) book limit %d , 10;", "%",rq->bookName,"%", rq->num);   //每次的偏移量为10
			if (!sql.SelectMySql(sqlBuf, 9, lst))   //表有9列数据
			{
				cout << "查询数据库失败" << endl;
			}
			else {
				//将书籍信息发送给客户端
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

					//根据id和bookName拼出书籍图片地址
					char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
					strcat_s(bookPath, id);
					strcat_s(bookPath, bookName);
					strcat_s(bookPath, ".jpg");   //将文件路径拼出来
					FILE* fp;
					if (0 == fopen_s(&fp, bookPath, "rb"))  //以二进制读的方式打开文件
					{
						//如果打开成功，则将文件信息读出

							// 这是一个存储文件(夹)信息的结构体，其中有文件大小和创建时间、访问时间、修改时间等
						struct stat statbuf;
						// 提供文件名字符串，获得文件属性结构体
						stat(bookPath, &statbuf);
						int fileSize = statbuf.st_size;
						char* img = new char[fileSize];
						int num = fread(img, fileSize,1, fp);   //将图片文件读入到img中
						//读完之后关闭文件
						fclose(fp);
						//将标志位和书籍的上传者和是否是教育书籍和书名塞进包里一起发给客户端
						int type = _DEF_PROTOCOL_TCP_SELECT_BOOKS_IMG_INFO_RS;
						int packSize = fileSize + 4 + 5 + 5;  //分别是：type,bookid,isSchool
						char* imgInfo = new  char[packSize]; //文件大小 + type大小 + bookid大小 +isSchool大小
						memcpy(imgInfo, (char*)&type, 4);  //先将type插入进去
						memcpy(imgInfo + 4, bookid, 5);   //再将bookid添加到尾部
						memcpy(imgInfo + 9, isSchool, 5);  //再将isSchool插入进去									
						memcpy(imgInfo + 14, img, fileSize);   //再将书籍图片信息添加到尾部
						//将此包发送给客户端
						p_pMediator->SendData(ISendIp, imgInfo, packSize);

						delete[]img;
						img = NULL;
						delete[]imgInfo;
						imgInfo = NULL;


						//图片信息发送完毕之后发送文字信息
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
					else   //如果没有读到图片也需要将list中的元素pop，避免死循环
					{
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
					}
					//如果打开图片文件失败则进行下一次循环


				}
			}
		}

		else if (OTHER_GOODS == rq->isSchool)  //如果是其他物品
		{

		sprintf_s(sqlBuf, "select * from (select * from othergoodsinfo where bookName like '%s%s%s' ) book limit %d , 10;","%", rq->bookName,"%", rq->num);   //每次的偏移量为10
		if (!sql.SelectMySql(sqlBuf, 9, lst))   //表有9列数据
		{
			cout << "查询数据库失败" << endl;
		}
		else {
			//将书籍信息发送给客户端
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

				//根据id和bookName拼出书籍图片地址
				char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
				strcat_s(bookPath, id);
				strcat_s(bookPath, bookName);
				strcat_s(bookPath, ".jpg");   //将文件路径拼出来
				FILE* fp;
				if (0 == fopen_s(&fp, bookPath, "rb"))  //以二进制读的方式打开文件
				{
					//如果打开成功，则将文件信息读出

						// 这是一个存储文件(夹)信息的结构体，其中有文件大小和创建时间、访问时间、修改时间等
					struct stat statbuf;
					// 提供文件名字符串，获得文件属性结构体
					stat(bookPath, &statbuf);
					int fileSize = statbuf.st_size;
					char* img = new char[fileSize];
					int num = fread(img, fileSize,1, fp);   //将图片文件读入到img中
					//读完之后关闭文件
					fclose(fp);
					//将标志位和书籍的上传者和是否是教育书籍和书名塞进包里一起发给客户端
					int type = _DEF_PROTOCOL_TCP_SELECT_BOOKS_IMG_INFO_RS;
					int packSize = fileSize + 4 + 5 + 5;  //分别是：type,bookid,isSchool
					char* imgInfo = new  char[packSize]; //文件大小 + type大小 + bookid大小 +isSchool大小
					memcpy(imgInfo, (char*)&type, 4);  //先将type插入进去
					memcpy(imgInfo + 4, bookid, 5);   //再将bookid添加到尾部
					memcpy(imgInfo + 9, isSchool, 5);  //再将isSchool插入进去									
					memcpy(imgInfo + 14, img, fileSize);   //再将书籍图片信息添加到尾部
					//将此包发送给客户端
					p_pMediator->SendData(ISendIp, imgInfo, packSize);

					delete[]img;
					img = NULL;
					delete[]imgInfo;
					imgInfo = NULL;


					//图片信息发送完毕之后发送文字信息
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
				else   //如果没有读到图片也需要将list中的元素pop，避免死循环
				{
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
				}
				//如果打开图片文件失败则进行下一次循环


			}
		}

		}
	}



	else {     //如果不是搜索包
		if (SCHOOL_BOOK == rq->isSchool || ALL_BOOK_AND_OTHER == rq->isSchool)   //如果查询的是校内书籍或者所有
		{
			sprintf_s(sqlBuf, "select * from schoolbooksinfo limit %d , 10 ;", rq->num);   //每次的偏移量为10
			if (!sql.SelectMySql(sqlBuf, 9, lst))   //表有9列数据
			{
				cout << "查询数据库失败" << endl;
			}
			else {
				//将书籍信息发送给客户端
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

					//根据id和bookName拼出书籍图片地址
					char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
					strcat_s(bookPath, id);
					strcat_s(bookPath, bookName);
					strcat_s(bookPath, ".jpg");   //将文件路径拼出来
					FILE* fp;
					if (0 == fopen_s(&fp, bookPath, "rb"))  //以二进制读的方式打开文件
					{
						//如果打开成功，则将文件信息读出

							// 这是一个存储文件(夹)信息的结构体，其中有文件大小和创建时间、访问时间、修改时间等
						struct stat statbuf;
						// 提供文件名字符串，获得文件属性结构体
						stat(bookPath, &statbuf);
						int fileSize = statbuf.st_size;
						char* img = new char[fileSize];
						int num = fread(img, fileSize,1, fp);   //将图片文件读入到img中
						//读完之后关闭文件
						fclose(fp);
						//将标志位和书籍的上传者和是否是教育书籍和书名塞进包里一起发给客户端
						int type = _DEF_PROTOCOL_TCP_BOOKS_IMG_INFO_RS;
						int packSize = fileSize + 4 + 5 + 5;  //分别是：type,bookid,isSchool
						char* imgInfo = new  char[packSize]; //文件大小 + type大小 + bookid大小 +isSchool大小
						memcpy(imgInfo, (char*)&type, 4);  //先将type插入进去
						memcpy(imgInfo + 4, bookid, 5);   //再将bookid添加到尾部
						memcpy(imgInfo + 9, isSchool, 5);  //再将isSchool插入进去									
						memcpy(imgInfo + 14, img, fileSize);   //再将书籍图片信息添加到尾部
						//将此包发送给客户端
						p_pMediator->SendData(ISendIp, imgInfo, packSize);

						delete[]img;
						img = NULL;
						delete[]imgInfo;
						imgInfo = NULL;


						//图片信息发送完毕之后发送文字信息
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
					else   //如果没有读到图片也需要将list中的元素pop，避免死循环
					{
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
					}
					//如果打开图片文件失败则进行下一次循环


				}
			}
		}


		if (NO_SCHOOL_BOOK == rq->isSchool || ALL_BOOK_AND_OTHER == rq->isSchool)   //如果查询的是校内书籍或者所有
		{
			sprintf_s(sqlBuf, "select * from noschoolbooksinfo limit %d , 10 ;", rq->num);   //每次的偏移量为10
			if (!sql.SelectMySql(sqlBuf, 9, lst))   //表有9列数据
			{
				cout << "查询数据库失败" << endl;
			}
			else {
				//将书籍信息发送给客户端
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

					//根据id和bookName拼出书籍图片地址
					char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
					strcat_s(bookPath, id);
					strcat_s(bookPath, bookName);
					strcat_s(bookPath, ".jpg");   //将文件路径拼出来
					FILE* fp;
					if (0 == fopen_s(&fp, bookPath, "rb"))  //以二进制读的方式打开文件
					{
						//如果打开成功，则将文件信息读出

							// 这是一个存储文件(夹)信息的结构体，其中有文件大小和创建时间、访问时间、修改时间等
						struct stat statbuf;
						// 提供文件名字符串，获得文件属性结构体
						stat(bookPath, &statbuf);
						int fileSize = statbuf.st_size;
						char* img = new char[fileSize];
						int num = fread(img, fileSize,1, fp);   //将图片文件读入到img中
						//读完之后关闭文件
						fclose(fp);
						//将标志位和书籍的上传者和是否是教育书籍和书名塞进包里一起发给客户端
						int type = _DEF_PROTOCOL_TCP_BOOKS_IMG_INFO_RS;
						int packSize = fileSize + 4 + 5 + 5;  //分别是：type,bookid,isSchool
						char* imgInfo = new  char[packSize]; //文件大小 + type大小 + bookid大小 +isSchool大小
						memcpy(imgInfo, (char*)&type, 4);  //先将type插入进去
						memcpy(imgInfo + 4, bookid, 5);   //再将bookid添加到尾部
						memcpy(imgInfo + 9, isSchool, 5);  //再将isSchool插入进去									
						memcpy(imgInfo + 14, img, fileSize);   //再将书籍图片信息添加到尾部
						//将此包发送给客户端
						p_pMediator->SendData(ISendIp, imgInfo, packSize);

						delete[]img;
						img = NULL;
						delete[]imgInfo;
						imgInfo = NULL;


						//图片信息发送完毕之后发送文字信息
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
					else   //如果没有读到图片也需要将list中的元素pop，避免死循环
					{
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
					}
					//如果打开图片文件失败则进行下一次循环


				}
			}
		}



		if (OTHER_GOODS == rq->isSchool || ALL_BOOK_AND_OTHER == rq->isSchool)   //如果查询的是校内书籍或者所有
		{
			sprintf_s(sqlBuf, "select * from othergoodsinfo limit %d , 10 ;", rq->num);   //每次的偏移量为10
			if (!sql.SelectMySql(sqlBuf, 9, lst))   //表有9列数据
			{
				cout << "查询数据库失败" << endl;
			}
			else {
				//将书籍信息发送给客户端
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

					//根据id和bookName拼出书籍图片地址
					char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
					strcat_s(bookPath, id);
					strcat_s(bookPath, bookName);
					strcat_s(bookPath, ".jpg");   //将文件路径拼出来
					FILE* fp;
					if (0 == fopen_s(&fp, bookPath, "rb"))  //以二进制读的方式打开文件
					{
						//如果打开成功，则将文件信息读出

							// 这是一个存储文件(夹)信息的结构体，其中有文件大小和创建时间、访问时间、修改时间等
						struct stat statbuf;
						// 提供文件名字符串，获得文件属性结构体
						stat(bookPath, &statbuf);
						int fileSize = statbuf.st_size;
						char* img = new char[fileSize];
						int num = fread(img, fileSize,1, fp);   //将图片文件读入到img中
						//读完之后关闭文件
						fclose(fp);
						//将标志位和书籍的上传者和是否是教育书籍和书名塞进包里一起发给客户端
						int type = _DEF_PROTOCOL_TCP_BOOKS_IMG_INFO_RS;
						int packSize = fileSize + 4 + 5 + 5;  //分别是：type,bookid,isSchool
						char* imgInfo = new  char[packSize]; //文件大小 + type大小 + bookid大小 +isSchool大小
						memcpy(imgInfo, (char*)&type, 4);  //先将type插入进去
						memcpy(imgInfo + 4, bookid, 5);   //再将bookid添加到尾部
						memcpy(imgInfo + 9, isSchool, 5);  //再将isSchool插入进去									
						memcpy(imgInfo + 14, img, fileSize);   //再将书籍图片信息添加到尾部
						//将此包发送给客户端
						p_pMediator->SendData(ISendIp, imgInfo, packSize);

						delete[]img;
						img = NULL;
						delete[]imgInfo;
						imgInfo = NULL;


						//图片信息发送完毕之后发送文字信息
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
					else   //如果没有读到图片也需要将list中的元素pop，避免死循环
					{
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
						lst.pop_front();
					}
					//如果打开图片文件失败则进行下一次循环


				}
			}
		}
	}

}

 //处理收到的获取某主机上架所有信息的请求
void CKernel::dealAllBooksInfoRq(long ISendIp, char* buf, int nLen)
{
	//拆包
	STRU_TCP_SELECT_MY_BOOKS_INFO_RQ* rq = (STRU_TCP_SELECT_MY_BOOKS_INFO_RQ*)buf;
	list<string> lst;
	char sqlBuf[1024] = "";
		sprintf_s(sqlBuf, "select * from schoolbooksInfo where id = %d;", rq->id);  //查询这个表中目标id的所有书籍信息
		if (!sql.SelectMySql(sqlBuf, 9, lst))   //表有9列数据
		{
			cout << "查询数据库失败" << endl;
		}
		else {
			//将书籍信息发送给客户端
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

				//根据id和bookName拼出书籍图片地址
				char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
				strcat_s(bookPath, id);
				strcat_s(bookPath, bookName);
				strcat_s(bookPath, ".jpg");   //将文件路径拼出来
				FILE* fp;
				if (0 == fopen_s(&fp, bookPath, "rb"))  //以二进制读的方式打开文件
				{
					//如果打开成功，则将文件信息读出

						// 这是一个存储文件(夹)信息的结构体，其中有文件大小和创建时间、访问时间、修改时间等
					struct stat statbuf;
					// 提供文件名字符串，获得文件属性结构体
					stat(bookPath, &statbuf);
					int fileSize = statbuf.st_size;
					char* img = new char[fileSize];
					int num = fread(img, fileSize,1, fp);   //将图片文件读入到img中
					//读完之后关闭文件
					fclose(fp);
					//将标志位和书籍的上传者和是否是教育书籍和书名塞进包里一起发给客户端
					int type = _DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_IMG_INFO_RS;
					int packSize = fileSize + 4 + 5 + 5;  //分别是：type,bookid,isSchool
					char* imgInfo = new  char[packSize]; //文件大小 + type大小 + bookid大小 +isSchool大小
					memcpy(imgInfo, (char*)&type, 4);  //先将type插入进去
					memcpy(imgInfo + 4, bookid, 5);   //再将bookid添加到尾部
					memcpy(imgInfo + 9, isSchool, 5);  //再将isSchool插入进去									
					memcpy(imgInfo + 14, img, fileSize);   //再将书籍图片信息添加到尾部
					//将此包发送给客户端
					p_pMediator->SendData(ISendIp, imgInfo, packSize);

					delete[]img;
					img = NULL;
					delete[]imgInfo;
					imgInfo = NULL;


					//图片信息发送完毕之后发送文字信息
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
				else   //如果没有读到图片也需要将list中的元素pop，避免死循环
				{
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
				}
				//如果打开图片文件失败则进行下一次循环


			}
		}



		sprintf_s(sqlBuf, "select * from noschoolbooksInfo where id = %d;", rq->id);  //查询非校内书籍表中目标id的所有书籍信息
		if (!sql.SelectMySql(sqlBuf, 9, lst))   //表有9列数据
		{
			cout << "查询数据库失败" << endl;
		}
		else {
			//将书籍信息发送给客户端
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

				//根据id和bookName拼出书籍图片地址
				char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
				strcat_s(bookPath, id);
				strcat_s(bookPath, bookName);
				strcat_s(bookPath, ".jpg");   //将文件路径拼出来
				FILE* fp;
				if (0 == fopen_s(&fp, bookPath, "rb"))  //以二进制读的方式打开文件
				{
					//如果打开成功，则将文件信息读出

						// 这是一个存储文件(夹)信息的结构体，其中有文件大小和创建时间、访问时间、修改时间等
					struct stat statbuf;
					// 提供文件名字符串，获得文件属性结构体
					stat(bookPath, &statbuf);
					int fileSize = statbuf.st_size;
					char* img = new char[fileSize];
					int num = fread(img, fileSize,1, fp);   //将图片文件读入到img中
					//读完之后关闭文件
					fclose(fp);
					//将标志位和书籍的上传者和是否是教育书籍和书名塞进包里一起发给客户端
					int type = _DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_IMG_INFO_RS;
					int packSize = fileSize + 4 + 5 + 5;  //分别是：type,bookid,isSchool
					char* imgInfo = new  char[packSize]; //文件大小 + type大小 + bookid大小 +isSchool大小
					memcpy(imgInfo, (char*)&type, 4);  //先将type插入进去
					memcpy(imgInfo + 4, bookid, 5);   //再将bookid添加到尾部
					memcpy(imgInfo + 9, isSchool, 5);  //再将isSchool插入进去									
					memcpy(imgInfo + 14, img, fileSize);   //再将书籍图片信息添加到尾部
					//将此包发送给客户端
					p_pMediator->SendData(ISendIp, imgInfo, packSize);

					delete[]img;
					img = NULL;
					delete[]imgInfo;
					imgInfo = NULL;


					//图片信息发送完毕之后发送文字信息
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
				else   //如果没有读到图片也需要将list中的元素pop，避免死循环
				{
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
				}
				//如果打开图片文件失败则进行下一次循环


			}
		}




		sprintf_s(sqlBuf, "select * from othergoodsinfo where id = %d;", rq->id);  //查询其他物品表中目标id的所有书籍信息
		if (!sql.SelectMySql(sqlBuf, 9, lst))   //表有9列数据
		{
			cout << "查询数据库失败" << endl;
		}
		else {
			//将书籍信息发送给客户端
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

				//根据id和bookName拼出书籍图片地址
				char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
				strcat_s(bookPath, id);
				strcat_s(bookPath, bookName);
				strcat_s(bookPath, ".jpg");   //将文件路径拼出来
				FILE* fp;
				if (0 == fopen_s(&fp, bookPath, "rb"))  //以二进制读的方式打开文件
				{
					//如果打开成功，则将文件信息读出

						// 这是一个存储文件(夹)信息的结构体，其中有文件大小和创建时间、访问时间、修改时间等
					struct stat statbuf;
					// 提供文件名字符串，获得文件属性结构体
					stat(bookPath, &statbuf);
					int fileSize = statbuf.st_size;
					char* img = new char[fileSize];
					int num = fread(img, fileSize,1, fp);   //将图片文件读入到img中
					//读完之后关闭文件
					fclose(fp);
					//将标志位和书籍的上传者和是否是教育书籍和书名塞进包里一起发给客户端
					int type = _DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_IMG_INFO_RS;
					int packSize = fileSize + 4 + 5 + 5;  //分别是：type,bookid,isSchool
					char* imgInfo = new  char[packSize]; //文件大小 + type大小 + bookid大小 +isSchool大小
					memcpy(imgInfo, (char*)&type, 4);  //先将type插入进去
					memcpy(imgInfo + 4, bookid, 5);   //再将bookid添加到尾部
					memcpy(imgInfo + 9, isSchool, 5);  //再将isSchool插入进去									
					memcpy(imgInfo + 14, img, fileSize);   //再将书籍图片信息添加到尾部
					//将此包发送给客户端
					p_pMediator->SendData(ISendIp, imgInfo, packSize);

					delete[]img;
					img = NULL;
					delete[]imgInfo;
					imgInfo = NULL;


					//图片信息发送完毕之后发送文字信息
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
				else   //如果没有读到图片也需要将list中的元素pop，避免死循环
				{
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
					lst.pop_front();
				}
				//如果打开图片文件失败则进行下一次循环


			}
		}
	
}

//处理下架目标书籍信息的请求
void CKernel::dealDeleteBooksInfoRq(long ISendIp, char* buf, int nLen)
{
	//拆包
	STRU_TCP_DELETE_BOOKS_INFO_RQ* rq = (STRU_TCP_DELETE_BOOKS_INFO_RQ*)buf;
	list<string> lst;
	char sqlBuf[1024] = "";
	STRU_TCP_DELETE_BOOKS_INFO_RS rs;
	rs.isSchool = rq->isSchool;
	//首先根据id和bookName拼出文件地址，
	char bookPath[_DEF_BOOK_NAME_SIZE + 20] = ".//img//";
	char id[5] = "";
	_itoa_s(rq->id, id, 10);
	strcat_s(bookPath, id);
	strcat_s(bookPath, rq->bookName);
	strcat_s(bookPath, ".jpg");
	//尝试打开，如果打开成功再删除
	FILE* fp;
	if (0 == fopen_s(&fp, bookPath, "rb"))
	{
		//如果打开成功，则继续测试sql语句是否能执行成功
		//先关闭
		fclose(fp);


		if (SCHOOL_BOOK == rq->isSchool)
		{
			//首先查询这本书的bookid,然后再删除
			sprintf_s(sqlBuf, "select bookid from schoolbooksinfo where id = %d and bookName = '%s';", rq->id, rq->bookName);  //删除目标表中的目标书籍信息
			if (sql.SelectMySql(sqlBuf, 1, lst))  //如果数据库查询成功
			{
				//删除数据库
				sprintf_s(sqlBuf, "delete from schoolbooksinfo where id = %d and bookName = '%s';", rq->id, rq->bookName);  //删除目标表中的目标书籍信息
			//如果删除成功，再删除文件，然后发送下架成功的包给客户端
				if (sql.UpdateMySql(sqlBuf))  //如果数据库删除成功
				{
					//删除文件
					remove(bookPath);
					//发送删除成功的信息给客户端
					rs.isDeleteSuccess = DELETE_SUCCESS;
					rs.bookid = atoi(lst.front().c_str());
					lst.pop_front();
				}
				else {
					//如果删除失败、
					rs.isDeleteSuccess = DELETE_DEFAULT;
				}
			}
			else {
				//如果查询失败
				rs.isDeleteSuccess = DELETE_DEFAULT;
			}
		}
		else if (NO_SCHOOL_BOOK == rq->isSchool)
		{
			//首先查询这本书的bookid,然后再删除
			sprintf_s(sqlBuf, "select bookid from noschoolbooksinfo where id = %d and bookName = '%s';", rq->id, rq->bookName);  //删除目标表中的目标书籍信息
			if (sql.SelectMySql(sqlBuf, 1, lst))  //如果数据库查询成功
			{
				//删除数据库
				sprintf_s(sqlBuf, "delete from noschoolbooksinfo where id = %d and bookName = '%s';", rq->id, rq->bookName);  //删除目标表中的目标书籍信息
			//如果删除成功，再删除文件，然后发送下架成功的包给客户端
				if (sql.UpdateMySql(sqlBuf))  //如果数据库删除成功
				{
					//删除文件
					remove(bookPath);
					//发送删除成功的信息给客户端
					rs.isDeleteSuccess = DELETE_SUCCESS;
					rs.bookid = atoi(lst.front().c_str());
					lst.pop_front();
				}
				else {
					//如果删除失败、
					rs.isDeleteSuccess = DELETE_DEFAULT;
				}
			}
			else {
				//如果查询失败
				rs.isDeleteSuccess = DELETE_DEFAULT;
			}
		}

		else if (OTHER_GOODS == rq->isSchool)
		{
			//首先查询这本书的bookid,然后再删除
			sprintf_s(sqlBuf, "select bookid from othergoodsinfo where id = %d and bookName = '%s';", rq->id, rq->bookName);  //删除目标表中的目标书籍信息
			if (sql.SelectMySql(sqlBuf, 1, lst))  //如果数据库查询成功
			{
				//删除数据库
				sprintf_s(sqlBuf, "delete from othergoodsinfo where id = %d and bookName = '%s';", rq->id, rq->bookName);  //删除目标表中的目标书籍信息
			//如果删除成功，再删除文件，然后发送下架成功的包给客户端
				if (sql.UpdateMySql(sqlBuf))  //如果数据库删除成功
				{
					//删除文件
					remove(bookPath);
					//发送删除成功的信息给客户端
					rs.isDeleteSuccess = DELETE_SUCCESS;
					rs.bookid = atoi(lst.front().c_str());
					lst.pop_front();
				}
				else {
					//如果删除失败、
					rs.isDeleteSuccess = DELETE_DEFAULT;
				}
			}
			else {
				//如果查询失败
				rs.isDeleteSuccess = DELETE_DEFAULT;
			}
		}




	}
	else {   //如果打不开
		rs.isDeleteSuccess = DELETE_DEFAULT;  //删除失败

	}

	strcpy_s(rs.bookName, rq->bookName);
	p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));
}

//处理收到的更换头像的请求
void CKernel::dealAlterIconRq(long ISendIp, char* buf, int nLen)
{
	//前四个字节是type
	//4~7字节是客户端id
	//最后是头像图片信息
	int id = *(((int*)buf) + 1);
	//头像路径为  .//icon//id.jpg
	char iconPath[30] = ".//icon//";
	char pid[5];
	_itoa_s(id, pid, 10);  //将id转换成字符串形式
	strcat_s(iconPath, pid);
	strcat_s(iconPath, ".jpg");   //在字符串末尾添加
	

	STRU_TCP_ALTER_ICON_RS rs;
	//将图片信息存入文件
	FILE* fp;
	if (0 == fopen_s(&fp, iconPath, "wb"))
	{
		if (nLen > 8)
		{
			int num = fwrite(buf + 8, nLen - 8, 1, fp);
			if (num == 1)
			{
				cout << "写入成功" << endl;
				//发送更改成功的回复给客户端
				rs.isSuccess = true;
			}
			else
			{
				//发送更改失败的回复给客户端
				rs.isSuccess = false;
			}
		}
		else
		{
			//发送更改失败的回复给客户端
			rs.isSuccess = false;
		}
		fclose(fp);
	}
	else {
		//发送更改失败的回复给客户端
		rs.isSuccess = false;
	}

	p_pMediator->SendData(ISendIp, (char*)&rs, sizeof(rs));

}



//处理语音通话信息包
void CKernel::dealVoidInfo(long ISendIp, char* buf, int nLen)
{
	STRU_VOID_TRAN* rq = (STRU_VOID_TRAN*)buf;
	if (m_mapIdToSockAddr_in.count(rq->ISendIp))
	{
		m_pUdpMediator->SendData(rq->ISendIp, buf, nLen);
	}
}

