#ifndef CKERNEL_H
#define CKERNEL_H
#include<QObject>
#include<QMap>
#include<QDebug>
#include<QMessageBox>
#include<winsock.h>
#include<QTime>
#include"feiqdialog.h"
#include"INetMediator.h"
#include"chatdialog.h"
#include"packDef.h"
#include"feiqdialog.h"
#include"UdpMediator.h"
#include"INet.h"
#include"tcpmediator.h"
#include"form.h"
#include "formfriend.h"
//#include"myvoid.h"
#include"logindlg.h"
#include"maindialg.h"
#include"useritem.h"
#include"cmysound.h"
#include"csecondhandbooks.h"
#include"addnewbooks.h"
#include"books.h"
#include"mybookswdg.h"


#define TCP_TYPE_PFUN(a)   dealTcpData[a - (_DEF_PROTOCOL_BASE + 1) ]    //下标从0开始
class CKernel;   //声明类
typedef void (CKernel::*pTcpFun)(char* , int);          //利用函数指针来对不同的数据调用不同的函数
class CKernel:public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = 0);
    ~CKernel();
signals:
//发送信号绘制窗口
   void SIG_painWindow(QImage image);


public slots:
    //处理接收到的所有网络数据
    void slot_ReadData(long lSendIp,char* buf,int nLen);
    //处理双击某一行的信号
//    void slot_userClicked(QString ip);
    //处理聊天内容
    void slot_sendMsg(int id,QString content);
    //处理关闭窗口的信号
//    void slot_closeWnd();

    //处理发送文件的信号
    void slot_sendFile(QString ip,QFileInfo FileInfo );




      //处理需要发送的视频图片信息
      void slot_sendMyImg(long id, QByteArray content);

      //处理需要发送的音频信息
      void slot_sendVoid(int lens ,char* data);


      //打开上传二手书的交易信息
      void slot_openSellBooks();





 //=============TCP==========================================================================
      //处理所有接收到的TCP数据
      void slot_ReadTcpData(char* buf, int nLen);
       //处理注册界面发来的注册信号
       void slot_register(QString name, QString tel, QString password);
       //处理登录界面发来的登陆信号
       void slot_Login(QString account, QString password);
        //处理主窗口发来的窗口关闭信号
       void slot_closeWnd();
       //处理添加好友
       void slot_addFriend();
       //处理发送视频通话请求
        void slot_videoRq(long id);
        //处理发送语音通话请求
        void slot_SoundRq(long id);
        //发送挂断语音通话申请
        void slot_closeVoid(long id);
        //发送关闭视频通话申请
        void slot_closeVideo(long id);
        //处理需要发送的二手书信息
        void slot_sendBooInfo(QByteArray byte,QString name, QString Are,QString info,QString price,QString QQ,QString weChat,int isSchool);
        //打开二手书交易窗口,并告诉服务器，让服务器将二手书的信息发给客户
        void slot_openBooks();
        //发送获取书籍信息的请求
        void slot_getBooksInfo(int isSchool , int num);
        //发送搜索书籍信息的请求
        void slot_selectBooksInfo(int isSchool, int num, QString bookName, bool isClear);
        //发送获取本机上架的所有物品信息的请求
        void slot_getMyBooksInfo();
        //发送某本书的下架请求
        void slot_deleteTargetBooks(int isSchool,QString bookName);
        //发送更换头像请求
        void slot_alterIconRq(QByteArray img);
//==================================================================================================
private:
    //发送文件线程
      static unsigned __stdcall SendFileBlockThread(void* IpVoid);

    //处理上线请求
    void dealOnlineRq(long lSendIp,char* buf,int nLen);
    //处理上线回复
     void dealOnlineRs(long lSendIp,char* buf,int nLen);
     //处理聊天请求
      void dealChatRq(long lSendIp,char* buf,int nLen);

        //处理接收到的视频文件
        void  dealVideoTran(long lSendIp,char* buf,int nLen);
        //处理接收到的音频文件
        void dealVoidTran(long lSendIp,char* buf,int nLen);
//    //处理下线请求
//       void dealOfflineRq(long lSendIp,char* buf,int nLen);
       //处理文件信息请求
       void dealFileRq(long lSendIp,char* buf,int nLen);
       //处理文件信息回复
         void dealFileRs(long lSendIp,char* buf,int nLen);
         //发送文件块儿
        void dealFileBlockRq(char* buf);

        //获取本机上网所使用的外网ip
        int GetInternetIP(string &Inernet_ip);

        //因为路由器中端口与私网ip的映射是动态的，所以需要一个线程每隔一段时间就向服务器发送一条消息，告诉服务器自己的地址信息的变化！
         static unsigned __stdcall LiveThread(void* IpVoid);
         //处理接收到的音频信息包
        dealVoidInfo(long lSendIp,char* buf,int nLen);

   //=================TCP===================================================================
        //初始化函数指针数组
         void InitTcpDealFun();
         //编码的互相转换
         void utf8_Gb2312_Change(QString &utf8, char *&gb2312, int len);  // *&表示指针的引用，即引用对象的类型是一个引用
        //处理注册回复
    void dealRegisterRs(char *buf, int nLen);
    //处理登录回复
    void dealLoginRs(char* buf, int nLen);
    //处理好友信息请求
    void dealFriendInfoRq(char* buf, int nLen);
    //处理下线请求
    void dealOfflineRq(char* buf, int nLen);
    //处理收到的添加好友请求
    void dealaddFriendRq(char* buf, int nLen);
    //TODO:处理添加好友回复
        void dealaddFriendRs(char* buf, int nLen);
        //处理视频通话请求
        void dealVideoRq(char* buf,int nLen);
        //处理视频通话请求的回复
          void dealVideoRs(char* buf,int nLen);

          //TODO:处理挂断视频通话

          //处理接收到的语音通话请求
        void dealVoidRq(char* buf, int nLen);
        //处理接收到的语音通话回复
       void dealVoidRs(char* buf, int nLen);
       //处理接收到的挂断语音通话申请
       void dealCloseVoid(char* buf, int nLen);
       //处理接收到的上架回复信息
       void dealAddBookRs(char* buf, int nLen);
       //处理收到的书籍图片信息
       void dealBooksImgInfoRq(char* buf, int nLen);
       //处理收到的书籍文字信息
       void dealBooksTextInfoRq(char* buf, int nLen);
       //处理搜索的书籍图片信息
       void dealSelectBooksImgInfoRq(char* buf, int nLen);
       //处理搜索的书籍文字信息
       void dealSelectBooksTextInfoRq(char* buf, int nLen);
       //处理获取的本机上传的书籍图片信息
       void dealMyBooksInfo(char* buf,int nLen);
       //处理获取的本机上传的书籍的文字信息
       void dealMyBooksTextInfo(char* buf, int nLen);
       //处理下架书籍的回复包
       void dealDeleteBookRs(char* buf, int nLen);
       //处理收到的更换头像的回复
       void dealAlterIconRs(char* buf, int nLen);

private:
//    FeiQDialog* p_mainWnd;   //主窗口指针
    INetMediator* m_pMediator;  //Udp中介者类指针
    INetMediator* m_pTcpMediator;  //Tcp中介者类指针
    Form* m_pVideoForm; //我的视频通话窗口指针
    FormFriend* Other_pVideoForm; //别人的视频通话窗口指针
    CMySound* m_pMySound;      //语音类的指针
    logindlg* m_ploginDlg;   //登录窗口的指针
    MainDialg* m_pMaindialog;  //主窗口的指针
    CSecondHandBooks* m_pBooks;   //二手书交易窗口的指针
    addNewBooks* m_pAddBooks;   //上架书籍窗口的指针
    pTcpFun dealTcpData[_DEF_PROTOCOL_COUNT];  //定义函数指针数组，绑定不同的数据处理函数，其中type为下标
     STRU_FILE_INFO_RQ* MyFileInfo;

    //管理id对应的聊天窗口
    QMap<long,useritem*> m_mapIdToUserItem;
    //管理id和书名对应的书籍窗口
    QMap<QString,Books*> m_mapIdBookNameToBooks;
    QMap<QString,Books*> m_mapSelectBooks;
    QMap<QString,MyBooksWdg*> m_mapMyBooks;

    //登录者的id
    int m_id;
    QString m_name;

    //本机上网所用公网ip
    string Inernet_ip;
    long Inernet_Ip;

    //服务器分配的本地主机的编号
//    long MyIp;

    bool LiveStop;   //生命包线程的标志位
    bool videoStop;  //发送摄像头数据的标志位
    int ee=0;


};

#endif // CKERNEL_H
