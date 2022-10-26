#ifndef CKERNEL_H
#define CKERNEL_H
#include<QObject>
#include<QMap>
#include"feiqdialog.h"
#include"INetMediator.h"
#include"chatdialog.h"
#include"packDef.h"

#include<QDebug>
#include"feiqdialog.h"
#include"UdpMediator.h"
#include<QMessageBox>
#include"INet.h"
#include<winsock.h>
#include<QTime>
#include"tcpmediator.h"



class CKernel:public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = 0);
    ~CKernel();
signals:

public slots:
    //处理接收到的所有网络数据
    void slot_ReadData(long lSendIp,char* buf,int nLen);
    //处理双击某一行的信号
    void slot_userClicked(QString ip);
    //处理聊天内容
    void slot_sendMsg(QString ip,QString content);
    //处理关闭窗口的信号
    void slot_closeWnd();

    //处理发送文件的信号
    void slot_sendFile(QString ip,QFileInfo FileInfo );

    //发送文件线程
      static unsigned __stdcall SendFileBlockThread(void* IpVoid);

private:
    //处理上线请求
    void dealOnlineRq(long lSendIp,char* buf,int nLen);
    //处理上线回复
     void dealOnlineRs(long lSendIp,char* buf,int nLen);
     //处理聊天请求
      void dealChatRq(long lSendIp,char* buf,int nLen);
    //处理下线请求
       void dealOfflineRq(long lSendIp,char* buf,int nLen);
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
private:
    FeiQDialog* p_mainWnd;   //主窗口指针
    INetMediator* m_pMediator;  //中介者类指针
    INetMediator* m_pTcpMediator;  //Tcp中介者类指针


     STRU_FILE_INFO_RQ* crq;

    //管理ip对应的聊天窗口
    QMap<long,ChatDialog*> m_mapIpToChatdlg;

    //本机上网所用公网ip
    string Inernet_ip;
    long Inernet_Ip;

    //服务器分配的本地主机的编号
    long MyIp;

    bool LiveStop;   //生命包线程的标志位
};

#endif // CKERNEL_H
