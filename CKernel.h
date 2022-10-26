#pragma once
#include"tcpServerMediator.h"
#include "CMySql.h"
#include "pacKDef.h"
#include "INetMediator.h"
#include <iostream>
using namespace std;

//定义函数指针
class CKernel;
typedef void (CKernel::* PFUN)(long lSendIp, char* buf, int nLen);

class CKernel
{
public:
	CKernel();
	~CKernel();
	//===========TCP部分================================================================================================================
	//打开服务器
	 bool startServer();
	//关闭服务器
	void closeServer();
	//初始化协议映射表
	void setProtocolMap();
	//获取用户的好友信息（包括自己）
	void getUserFriendInfo(int userId);
	//根据id获取用户信息
	void getUserInfoById(STRU_TCP_FRIEND_INFO* info, int id);
	//处理所有接收到的数据
	void dealReadyData(long ISendIp, char* buf, int nLen);
	//处理注册请求
	 void dealRegisterRq(long lSendIp, char* buf, int nLen);
	 //处理登录请求
	 void dealLoginRq(long lSendIp, char* buf, int nLen);
	 //处理聊天请求
	 void dealChatRq(long lSendIp, char* buf, int nLen);  //聊天请求用udp中的。此函数不用
	 //处理下线请求
	 void dealoffLineRq(long lSendIp, char* buf, int nLen);
	 //处理添加好友请求
	 void dealaddFriendRq(long lSendIp, char* buf, int nLen);
	 //处理添加好友回复
	 void dealaddFriendRs(long lSendIp, char* buf, int nLen);
	 //处理文件信息请求/回复
	 void dealFileInfoRq_Rs(long ISendIp, char* buf, int nLen);
	 //处理视频通话请求
	 void dealVideoRq(long ISendIp, char* buf, int nLen);
	 //处理视频通话回复
	 void dealVideoRs(long ISendIp, char* buf, int nLen);
	 //处理语音通话请求
	 void dealVoidRq(long ISendIp, char* buf, int nLen);
	 //处理语音通话回复
	 void dealVoidRs(long ISendIp, char* buf, int nLen);
	 //处理挂断语音通话申请
	 void dealCloseVoid(long ISendIp, char* buf, int nLen);
	 //处理书籍上架申请（接收到的书籍图片信息）
	 void dealBooksImgInfo(long ISendIp, char* buf, int nLen);
	 //处理书籍上架申请（接收到的书籍其他信息）
	 void dealBooksInfo(long ISendIp, char* buf, int nLen);
	 //处理收到的获取书籍信息的请求
	 void dealBooksInfoRq(long ISendIp, char* buf, int nLen); 
	 //处理收到的获取某主机上架所有信息的请求
	 void dealAllBooksInfoRq(long ISendIp, char* buf, int nLen);
	 //处理下架目标书籍信息的请求
	 void dealDeleteBooksInfoRq(long ISendIp, char* buf, int nLen);
	 //处理收到的更换头像的请求
	 void dealAlterIconRq(long ISendIp, char* buf, int nLen);
	 //==============UDP部分===========================================================================================================================
	 //处理所有接收到的udp消息：
	 void dealReadyUdpData(long ISendIp, char* buf, int nLen);
	 //处理udp上线请求 （因为要存储udp地址，所以此函数必须有）
	 void dealUdpOnlineRq(long ISendIp, char* buf, int nLen);
	 //处理聊天请求
	 void dealUdpChatRq(long ISendIp, char* buf, int nLen);
	 //处理生命包请求
	 void dealLifeRq(long ISendIp, char* buf, int nLen);
	 //处理视频通话信息/*（线程中处理）*/
	 void dealVideoInfo(long ISendIp, char* buf, int nLen);
	 //处理语音通话信息包
	 void dealVoidInfo(long ISendIp, char* buf, int nLen);

private:
	CMySql sql;
	//tcp中介者
	INetMediator* p_pMediator;
	//udp中介者
	INetMediator* m_pUdpMediator;
	//定义协议映射数组
	PFUN m_netProtocolMap[_DEF_PROTOCOL_COUNT];
	//保存用户id和对应的socket
	map<int, SOCKET> m_mapIdToSocket;
	//保存用户id对应的ip地址（UDP）
	map<int, long> m_mapIdToSockAddr_in;   //因为上线请求和下线请求是在tcp中实现的，所以此成员应同时配合着Tcp部分函数使用
	
};

