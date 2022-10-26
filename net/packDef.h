#pragma once
#include<string.h>
#include<process.h>
/*
0号是保留端口

1-1024是固定端口

即被某些程序固定使用，一般程序员不能使用

端口号从1025到49151是注册端口：这些端口多数没有明确的定义服务对象，不同程序可根据实际需要自己定义，
*/
#define _DEF_UDP_PORT   53421
#define _DEF_TCP_PORT   53421
#define _DEF_SERVER_IP  ("10.51.133.127")
#define _DEF_MAX_SIZE   (100)   //上线请求和回复中的name最大空间
#define _DEF_CONTENT_MAX  (1024)  //发送buf的最大空间
#define _DEF_PROTOCOL_COUNT  (50)   //一共有几个协议（处理函数）









//协议头
#define _DEF_PROTOCOL_BASE        (1000)   //  ？

//上线请求和回复
#define _DEF_PROTOCOL_ONLINE_RQ   (_DEF_PROTOCOL_BASE + 1)
#define _DEF_PROTOCOL_ONLINE_RS   (_DEF_PROTOCOL_BASE + 2)

//聊天请求
#define _DEF_PROTOCOL_CHAT_RQ       (_DEF_PROTOCOL_BASE + 3)
//下线请求
#define _DEF_PROTOCOL_OFFLINE_RQ     (_DEF_PROTOCOL_BASE + 4)

//文件信息发送请求/回复
#define _DEF_PROTOCOL_FILE_INFO_RQ      (_DEF_PROTOCOL_BASE + 5)
#define _DEF_PROTOCOL_FILE_INFO_RS     (_DEF_PROTOCOL_BASE + 6)

//文件块 发送
#define _DEF_PROTOCOL_FILE_BLOCK_RQ      (_DEF_PROTOCOL_BASE + 7)
#define _DEF_PROTOCOL_FILE_BLOCK_RS     (_DEF_PROTOCOL_BASE + 8)

//因为路由器中端口与私网ip的映射是动态的，客户端如果长时间没给服务器发消息，当端口映射发生变化服务器就找不到客户端了。
//所以需要一个线程每隔一段时间就向服务器发送一条消息，告诉服务器自己的地址信息的变化！
#define _DEF_PROTOCOL_LIVE (_DEF_PROTOCOL_BASE + 9)

//视频通话请求 回复
#define _DEF_PROTOCOL_TCP_VIDEO_RQ      (_DEF_PROTOCOL_BASE +10)
#define _DEF_PROTOCOL_TCP_VIDEO_RS     (_DEF_PROTOCOL_BASE + 11)

//视频通话包
#define _DEF_VIDEO_TRAN      (_DEF_PROTOCOL_BASE +12)
//语音通话请求 回复
#define _DEF_PROTOCOL_TCP_VOID_RQ      (_DEF_PROTOCOL_BASE +13)
#define _DEF_PROTOCOL_TCP_VOID_RS     (_DEF_PROTOCOL_BASE + 14)

//语音包
#define _DEF_VOID_TRAN      (_DEF_PROTOCOL_BASE +15)

//TCP协议：标记包四什么包，标记是那个结构体
//注册请求
#define _DEF_PROTOCOL_TCP_REGISTER_RQ   (_DEF_PROTOCOL_BASE + 16)
//注册回复
#define _DEF_PROTOCOL_TCP_REGISTER_RS  (_DEF_PROTOCOL_BASE + 17)
//登录请求
#define _DEF_PROTOCOL_TCP_LOGIN_RQ   (_DEF_PROTOCOL_BASE + 18)
//登录回复
#define _DEF_PROTOCOL_TCP_LOGIN_RS   (_DEF_PROTOCOL_BASE + 19)
//获取好友信息
#define _DEF_PROTOCOL_TCP_FRIEND_INFO   (_DEF_PROTOCOL_BASE + 20)
//聊天请求
#define _DEF_PROTOCOL_TCP_CHAT_RQ   (_DEF_PROTOCOL_BASE + 21)
//聊天回复
#define _DEF_PROTOCOL_TCP_CHAT_RS   (_DEF_PROTOCOL_BASE + 22)
//添加好友请求
#define _DEF_PROTOCOL_TCP_ADD_FRIEND_RQ   (_DEF_PROTOCOL_BASE + 23)
//添加好友回复
#define _DEF_PROTOCOL_TCP_ADD_FRIEND_RS   (_DEF_PROTOCOL_BASE + 24)
//下线请求
#define _DEF_PROTOCOL_TCP_OFFLINE_RQ      (_DEF_PROTOCOL_BASE + 25)
//结束语音通话申请
#define _DEF_PROTOCOL_TCP_CLOSE_VOID_RQ     (_DEF_PROTOCOL_BASE + 26)


//书籍上架图片信息
#define _DEF_PROTOCOL_TCP_ADD_BOOKS_IMG_RQ     (_DEF_PROTOCOL_BASE + 27)
//书籍上架的其他信息
#define _DEF_PROTOCOL_TCP_ADD_BOOKS_RQ     (_DEF_PROTOCOL_BASE + 28)
//书籍上架的回复
#define _DEF_PROTOCOL_TCP_ADD_BOOKS_RS    (_DEF_PROTOCOL_BASE + 29)
//获取书籍信息的请求
#define _DEF_PROTOCOL_TCP_BOOKS_INFO_RQ   (_DEF_PROTOCOL_BASE + 30)
//书籍的图片信息回复
#define _DEF_PROTOCOL_TCP_BOOKS_IMG_INFO_RS   (_DEF_PROTOCOL_BASE + 31)
//书籍的文件信息的回复
#define _DEF_PROTOCOL_TCP_BOOKS_TEXT_INFO_RS   (_DEF_PROTOCOL_BASE + 32)
//书籍搜索结果的图片信息
#define _DEF_PROTOCOL_TCP_SELECT_BOOKS_IMG_INFO_RS   (_DEF_PROTOCOL_BASE + 33)
//书籍搜索结果的文字信息
#define _DEF_PROTOCOL_TCP_SELECT_BOOKS_TEXT_INFO_RS   (_DEF_PROTOCOL_BASE + 34)
//获取本机上架的所有物品的信息
#define _DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_INFO_RQ   (_DEF_PROTOCOL_BASE + 35)
//个人上架物品的图片信息回复
#define _DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_IMG_INFO_RS   (_DEF_PROTOCOL_BASE + 36)
//个人上架物品的文字信息的回复
#define _DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_TEXT_INFO_RS   (_DEF_PROTOCOL_BASE + 37)
//下架物品信息请求
#define _DEF_PROTOCOL_TCP_DELETE_BOOKS_RQ  (_DEF_PROTOCOL_BASE + 38)
//下架物品信息的回复
#define _DEF_PROTOCOL_TCP_DELETE_BOOKS_RS  (_DEF_PROTOCOL_BASE + 39)
//换头像申请
#define _DEF_PROTOCOL_TCP_ALTER_ICON_RQ  (_DEF_PROTOCOL_BASE + 40)
//换头像回复
#define _DEF_PROTOCOL_TCP_ALTER_ICON_RS  (_DEF_PROTOCOL_BASE + 41)




//p判断是否是学校的书籍
#define SCHOOL_BOOK (0)   //校内书籍
#define NO_SCHOOL_BOOK (1)   //校外书籍
#define OTHER_GOODS  (2)    //其他非书籍类用品
#define ALL_BOOK_AND_OTHER   (3)   //所有物品，包括书籍和非书籍物品
#define SELECT_BOOK   (4)    //搜索书籍信息的请求


// 定义请求结果	
// 注册结果	
#define register_sucess   (0)
#define user_is_exist     (1)
#define parameter_error   (2)
// 登录结果	
#define login_success     (0)
#define password_error     (1)
#define user_not_exist     (2)
// 添加好友结果	
#define add_success        (0)
#define user_refuse        (1)
#define no_this_user       (2)
#define user_notlogin      (3)
 //发送聊天请求结果#define user_offline(3)
#define send_success        (0)
#define send_error          (1)
// 好友状态	
#define status_online       (1)
#define status_offline       (0)

#define DEF_MAX_SIZE  100


// TCP协议结构体	
typedef int PackType;
// 注册请求:协议头、手机号、昵称、密码	
typedef struct STRU_TCP_REGISTER_RQ {
    STRU_TCP_REGISTER_RQ() :type(_DEF_PROTOCOL_TCP_REGISTER_RQ)
   {
    memset(tel, 0, DEF_MAX_SIZE);
    memset(name, 0, DEF_MAX_SIZE);
    memset(password, 0, DEF_MAX_SIZE);
    }
    PackType type;
    char tel[DEF_MAX_SIZE];
    char name[DEF_MAX_SIZE];
    char password[DEF_MAX_SIZE];
}STRU_TCP_REGISTER_RQ;

// 注册回复:协议头、注册结果(用户已经存在、注册成功)	
typedef struct STRU_TCP_REGISTER_RS {
    STRU_TCP_REGISTER_RS() : type(_DEF_PROTOCOL_TCP_REGISTER_RS), result(register_sucess)
    {

    }
    PackType type;
    int result;
}STRU_TCP_REGISTER_RS;


//登录请求：协议头
typedef struct STRU_TCP_LOGIN_RQ {
    STRU_TCP_LOGIN_RQ() :type(_DEF_PROTOCOL_TCP_LOGIN_RQ)
    {
        memset(tel, 0, _DEF_MAX_SIZE);
        memset(password, 0, _DEF_MAX_SIZE);
    }
    PackType type;
    char tel[_DEF_MAX_SIZE];
    char password[_DEF_MAX_SIZE];
}STRU_TCP_LOGIN_RQ;

//登录回复：协议头，登录结果
typedef struct STRU_TCP_LOGIN_RS {
    STRU_TCP_LOGIN_RS() :type(_DEF_PROTOCOL_TCP_LOGIN_RS),userId(0)
    {

    }
    PackType type;
    int userId;
    int result;
}STRU_TCP_LOGIN_RS;



//获取好友信息：协议头， 用户id， 昵称， 签名， 头像id， 状态
typedef struct STRU_TCP_FRIEND_INFO {
    STRU_TCP_FRIEND_INFO() :type(_DEF_PROTOCOL_TCP_FRIEND_INFO), userId(0), iconId(0), status(status_online)
    {

    }
    PackType type;
    int userId;
    int iconId;
    int status;
    char name[_DEF_MAX_SIZE];
    char feeling[_DEF_MAX_SIZE];
}STRU_TCP_FRIEND_INFO;

//聊天请求：协议头，聊天内容，我是谁userId， 发给谁friendId
typedef struct STRU_TCP_CHAT_RQ {
    STRU_TCP_CHAT_RQ() :type(_DEF_PROTOCOL_TCP_CHAT_RQ), userId(0), friendId(0)
    {
        memset(content, 0, _DEF_CONTENT_MAX);
    }
    PackType type;
    int userId;
    int friendId;
    char content[_DEF_CONTENT_MAX];
}STRU_TCP_CHAT_RQ;

//聊天回复：协议头，我是谁userId, 发给谁friend， 聊天结束
typedef struct STRU_TCP_CHAT_RS {
    STRU_TCP_CHAT_RS() :type(_DEF_PROTOCOL_TCP_CHAT_RS), userId(0), friendId(0), result(send_success)
    {

    }
    PackType type;
    int userId;
    int friendId;
    int result;
}STRU_TCP_CHAT_RS;


#define ACCEPT 0
#define REFUSE 1
#define BUSY   2
//音频通话请求
struct STRU_VOID_RQ
{
    STRU_VOID_RQ() :type(_DEF_PROTOCOL_TCP_VOID_RQ), IsAccept(ACCEPT)
    {
        memset(name, 0, 32);
    }
    int type;
    char name[32];
    long ISendIp;      //服务器分配的目标主机的编号
    long m_id;      //服务器分配的本地主机的编号
    int IsAccept;   //标志位

};

//语音/音频通话回复
struct STRU_VOID_RS
{
    STRU_VOID_RS() :type(_DEF_PROTOCOL_TCP_VOID_RS), IsAccept(ACCEPT)
    {
        memset(name, 0, 32);
    }
    int type;
    char name[32];
    long ISendIp;      //服务器分配的目标主机的编号
    long m_id;      //服务器分配的本地主机的编号
    int IsAccept;   //标志位

};

//音频/语音包：
struct STRU_VOID_TRAN
{
    STRU_VOID_TRAN() :type(_DEF_VOID_TRAN), lens(0)
    {
        memset(voidData, 0, 1024);
    }

    int type;
    int lens;
    long m_id;
    long ISendIp;
    char voidData[1024];
};


//添加好友请求：协议头
typedef struct STRU_TCP_ADD_FRIEND_RQ {
    STRU_TCP_ADD_FRIEND_RQ() : type(_DEF_PROTOCOL_TCP_ADD_FRIEND_RQ), userId(0)
    {
        memset(userName, 0, _DEF_MAX_SIZE);
        memset(friendName, 0, _DEF_MAX_SIZE);
    }
    PackType type;
    int userId;
    char userName[_DEF_MAX_SIZE];
    char friendName[_DEF_MAX_SIZE];
}STRU_TCP_ADD_FRIEND_RQ;

//添加好友回复：协议头
typedef struct STRU_TCP_ADD_FRIEND_RS {
    STRU_TCP_ADD_FRIEND_RS() : type(_DEF_PROTOCOL_TCP_ADD_FRIEND_RS), userId(0)
    {

        memset(friendName, 0, _DEF_MAX_SIZE);
    }
    PackType type;
    int userId;
    int friendId;
    int result;
    char friendName[_DEF_MAX_SIZE];
}STRU_TCP_ADD_FRIEND_RS;

//下线请求：协议头
typedef struct STRU_TCP_OFFLINE_RQ {
    STRU_TCP_OFFLINE_RQ():type(_DEF_PROTOCOL_TCP_OFFLINE_RQ), userId(0)
    {

    }
    PackType type;
    int userId;
}STRU_TCP_OFFLINE_RQ;


#define _MAX_FILE_PATH  (256)   //文件最大路径长度
#define _DEF_FILE_CONTENT_SIZE  (8*1024)   //文件块内容大小：8K

#define _file_accept (0)
#define _file_refuse (1)

#define ServerIP  "103.46.128.53"   //服务器ip地址
#define ServerHost (38612)           //服务器端口号




//协议结构
//上线请求和回复
//上线请求/回复：协议头，自己的名字
struct STRU_ONLINE
{
	//STRU_ONLINE() :type(_DEF_PROTOCOL_ONLINE_RQ)
	//{
	//	memset(name, 0, _DEF_MAX_SIZE);   //初始化字符数组name为全0
	//}
	int type;
    long m_id;      //服务器分配的本地主机的编号
	//unsigned long ip;
	//char name[_DEF_MAX_SIZE];
    //long ISendIp;      //服务器分配的目标主机的编号

};

//聊天请求：协议头，聊天内容
struct STRU_CHAT
{
	STRU_CHAT():type(_DEF_PROTOCOL_CHAT_RQ)
	{
		memset(content, 0, _DEF_CONTENT_MAX);
	}
	int type;
	char content[_DEF_CONTENT_MAX];
    long ISendIp;      //服务器分配的目标主机的编号
    long m_id;      //服务器分配的本地主机的编号

};


//下线请求：协议头
struct STRU_OFFLINE
{
	STRU_OFFLINE() :type(_DEF_PROTOCOL_OFFLINE_RQ)
	{

	}
	int type;
        long m_id;      //服务器分配的本地主机的编号
};


//文件信息发送 请求
 struct STRU_FILE_INFO_RQ
 {
     STRU_FILE_INFO_RQ() :ntype(_DEF_PROTOCOL_FILE_INFO_RQ), szFileSize(0)
     {
        memset(szFileID, 0, _MAX_FILE_PATH);  //初始化字符数组全为0
        memset(szFileName, 0, _MAX_FILE_PATH);
        memset(szFilePath, 0, _MAX_FILE_PATH);
     }
     int ntype;
     char szFileID[_MAX_FILE_PATH];     //文件标识
     char szFileName[_MAX_FILE_PATH];  //文件名
     char szFilePath[_MAX_FILE_PATH];  //文件路径
     long long szFileSize;   //文件大小
     char name[32];
       long ISendIp;   //告诉服务器目标主机的ip地址
 };

 //文件信息的回复
 struct STRU_FILE_INFO_RS
 {
     STRU_FILE_INFO_RS():ntype(_DEF_PROTOCOL_FILE_INFO_RS), nRes(_file_accept){
        memset(szFileID, 0, _MAX_FILE_PATH );
     }
     int ntype;
     char szFileID[_MAX_FILE_PATH];  //文件标识
     int nRes;  //是否接收
      char name[32];
    long ISendIp;   //告诉服务器目标主机的ip地址
 };


 //文件块请求
 struct STRU_FILE_BLOCK_RQ  //协议头  文件标识  文件块 块大小
 {
     STRU_FILE_BLOCK_RQ() :ntype(_DEF_PROTOCOL_FILE_BLOCK_RQ)
     {

     }
     int ntype;
     char szFileID[_MAX_FILE_PATH];    //文件标识
     char szFileContent[_DEF_FILE_CONTENT_SIZE];   //文件块
     int nBlockSize;    //块大小
     long ISendIp;   //告诉服务器目标主机的ip地址
 };


#define _file_block_recv_success (0)
#define _file_block_recv_fail    (1)
 //文件块接收结果
 struct STRU_FILE_BLOCK_RS   //协议头  文件标志  结果
 {
     STRU_FILE_BLOCK_RS():ntype(_DEF_PROTOCOL_FILE_BLOCK_RS),
         nRes(_file_block_recv_success)
     {
         memset(szFileID, 0, _MAX_FILE_PATH);
     }


     int ntype;
     char szFileID[_MAX_FILE_PATH];
     int nRes;
     long ISendIp;   //告诉服务器目标主机的ip地址
 };


 struct FileInfo  //做接收使用  跨函数 ：文件标志，文件名，文件路径，总大小，文件指针
{
     FileInfo():nPos(0),nFileSize(0),pFile(NULL)
     {
         memset(szFileID, 0, _MAX_FILE_PATH);
         memset(szFileName, 0, _MAX_FILE_PATH);
         memset(szFilePath, 0, _MAX_FILE_PATH);
     }

     char szFileID[_MAX_FILE_PATH];
     char szFileName[_MAX_FILE_PATH];
     char szFilePath[_MAX_FILE_PATH];
     long long nPos;   //当前在什么位置
     long long nFileSize;  //文件总共多大
     FILE* pFile;        //文件指针
 };


 //视频通话请求
 struct STRU_VIDEO_RQ
 {
     STRU_VIDEO_RQ() :type(_DEF_PROTOCOL_TCP_VIDEO_RQ), IsAccept(true)
     {
         memset(name, 0, 32);
     }
     int type;
     char name[32];
     long ISendIp;      //服务器分配的目标主机的编号
     long m_id;      //服务器分配的本地主机的编号
     bool IsAccept;   //标志位

 };

 //视频通话回复
 struct STRU_VIDEO_RS
 {
     STRU_VIDEO_RS() :type(_DEF_PROTOCOL_TCP_VIDEO_RS), IsAccept(true)
     {
         memset(name, 0, 32);
     }
     int type;
     char name[32];
     long ISendIp;      //服务器分配的目标主机的编号
     long m_id;      //服务器分配的本地主机的编号
     bool IsAccept;   //标志位

 };

 //挂断语音通话申请
 struct STRU_CLOSE_VOID
 {
     STRU_CLOSE_VOID() :type(_DEF_PROTOCOL_TCP_CLOSE_VOID_RQ)
     {

     }

     int type;
     long m_id;
     long ISendIp;
 };

 //视频通话信息包
 //struct STRU_VIDEO_TRAN
 //{
 //    STRU_VIDEO_TRAN() :type(_DEF_VIDEO_TRAN)
 //    {
 //                 content.resize(60*1000);
 //    }
 //    int type;
 //    long ISendIp;      //服务器分配的目标主机的编号
 //    long size;     //记录这个包的大小（因为c++11没有QByteArray，所以服务器发送这个包的时候需要知道他的大小）
 //    long m_id;      //服务器分配的本地主机的编号
 //    QByteArray content;
 //};

 //语音包：
 //struct STRU_VOID_TRAN
 //{
 //    STRU_VOID_TRAN() :type(_DEF_VOID_TRAN), lens(0)
 //    {
 //        memset(voidData, 0, 1024);
 //    }

 //    int type;
 //    int lens;
 //    long m_id;
 //    long ISendIp;
 //    char voidData[1024];
 //};

#define _DEF_BOOK_NAME_SIZE (40)   //书名大小
#define _DEF_BOOK_AREA_SIZE (30)   //地方
#define _DEF_BOOK_PRICE_SIZE (5)   //书的价格
#define _DEF_BOOK_INFO_SIZE (100)   //书的信息大小
#define _DEF_QQ_SIZE (15)   //QQ大小
#define _DEF_WECHAT_SIZE (20)   //QQ大小

 struct STRU_TCP_ADD_BOOKS {
     STRU_TCP_ADD_BOOKS() : type(_DEF_PROTOCOL_TCP_ADD_BOOKS_RQ), id(0), isSchool(SCHOOL_BOOK) {
         memset(bookName, 0, _DEF_BOOK_NAME_SIZE);
         memset(bookArea, 0, _DEF_BOOK_AREA_SIZE);
         memset(bookInfo, 0, _DEF_BOOK_INFO_SIZE);
         memset(bookPrice, 0, _DEF_BOOK_PRICE_SIZE);
         memset(QQ, 0, _DEF_QQ_SIZE);
         memset(weChat, 0, _DEF_WECHAT_SIZE);


     }

     PackType type;
     int id;
     int isSchool;
     char bookName[_DEF_BOOK_NAME_SIZE];
     char bookArea[_DEF_BOOK_AREA_SIZE];
     char bookInfo[_DEF_BOOK_INFO_SIZE];
     char bookPrice[_DEF_BOOK_PRICE_SIZE];
     char QQ[_DEF_QQ_SIZE];
     char weChat[_DEF_WECHAT_SIZE];

 };



#define REPEAT_SUBMIT (0)   //重复提交
#define SUBMIT_ERROR (1)   //上架失败
#define SUBMIT_SUCCESS (2)    //上架成功
 struct STRU_TCP_ADD_BOOKS_RS {
     STRU_TCP_ADD_BOOKS_RS() : type(_DEF_PROTOCOL_TCP_ADD_BOOKS_RS) {
         memset(bookName, 0, _DEF_BOOK_NAME_SIZE);
     }

     PackType type;
     char bookName[_DEF_BOOK_NAME_SIZE];
     int result;

 };


 struct STRU_TCP_BOOKS_INFO_RQ {
     STRU_TCP_BOOKS_INFO_RQ() : type(_DEF_PROTOCOL_TCP_BOOKS_INFO_RQ), num(0),isSchool(SCHOOL_BOOK) {
         memset(bookName, 0, _DEF_BOOK_NAME_SIZE);
     }

     PackType type;
     int num;      //num表示客户端已经有了多少个book信息，即需要从第几个信息开始发给客户端
     int isSchool;
     int isSelect;   //看这个包是否是搜索包
     char bookName[_DEF_BOOK_NAME_SIZE];
 };


 struct STRU_TCP_BOOKS_TEXT_INFO_RS {
     STRU_TCP_BOOKS_TEXT_INFO_RS() : type(_DEF_PROTOCOL_TCP_BOOKS_TEXT_INFO_RS), bookid(0) , id(0), isSchool(SCHOOL_BOOK) {
         memset(bookName, 0, _DEF_BOOK_NAME_SIZE);
         memset(bookArea, 0, _DEF_BOOK_AREA_SIZE);
         memset(bookInfo, 0, _DEF_BOOK_INFO_SIZE);
         memset(bookPrice, 0, _DEF_BOOK_PRICE_SIZE);
         memset(QQ, 0, _DEF_QQ_SIZE);
         memset(weChat, 0, _DEF_WECHAT_SIZE);


     }

     PackType type;
     int bookid;
     int id;
     int isSchool;
     char bookName[_DEF_BOOK_NAME_SIZE];
     char bookArea[_DEF_BOOK_AREA_SIZE];
     char bookInfo[_DEF_BOOK_INFO_SIZE];
     char bookPrice[_DEF_BOOK_PRICE_SIZE];
     char QQ[_DEF_QQ_SIZE];
     char weChat[_DEF_WECHAT_SIZE];

 };



 struct STRU_TCP_SELECT_MY_BOOKS_INFO_RQ {
     STRU_TCP_SELECT_MY_BOOKS_INFO_RQ() :type(_DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_INFO_RQ), id(0) {

     }

     PackType type;
     int id;
 };


 struct STRU_TCP_DELETE_BOOKS_INFO_RQ {
     STRU_TCP_DELETE_BOOKS_INFO_RQ() : type(_DEF_PROTOCOL_TCP_DELETE_BOOKS_RQ), id(0),isSchool(SCHOOL_BOOK) {
         memset(bookName, 0, _DEF_BOOK_NAME_SIZE);
     }
     PackType type;
     int id;
     int isSchool;   //判断在哪个表中
     char bookName[_DEF_BOOK_NAME_SIZE];
 };


#define DELETE_DEFAULT (0)    //删除失败
#define DELETE_SUCCESS (1)    //删除成功
 struct STRU_TCP_DELETE_BOOKS_INFO_RS {
     STRU_TCP_DELETE_BOOKS_INFO_RS() : type(_DEF_PROTOCOL_TCP_DELETE_BOOKS_RS),bookid(0),  isDeleteSuccess(DELETE_SUCCESS),isSchool(SCHOOL_BOOK){
         memset(bookName, 0, _DEF_BOOK_NAME_SIZE);
     }
     PackType type;
     int isDeleteSuccess;   //判断是否删除成功
     int isSchool;
     int bookid;
     char bookName[_DEF_BOOK_NAME_SIZE];
 };

 struct STRU_TCP_ALTER_ICON_RS {     //换头像的回复
     STRU_TCP_ALTER_ICON_RS() : type(_DEF_PROTOCOL_TCP_ALTER_ICON_RS), isSuccess(true) {

     }

     PackType type;
     bool isSuccess;
 };