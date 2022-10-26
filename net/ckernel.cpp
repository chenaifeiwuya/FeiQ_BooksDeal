#include "ckernel.h"
#include<stdio.h>
#include<iostream>
CKernel::CKernel(QObject *parent) : QObject(parent),Inernet_ip(""),LiveStop(false),videoStop(true),m_pMySound(NULL),m_id(0),
  Other_pVideoForm(NULL),m_pVideoForm(NULL)
{


    qDebug()<<__func__;

    //1：初始化登录窗口
    m_ploginDlg = new logindlg;
    //绑定信号与槽函数
connect(m_ploginDlg,SIGNAL(SIG_register(QString,QString,QString)),
        this,SLOT(slot_register(QString,QString,QString)));
connect(m_ploginDlg,SIGNAL(SIG_login(QString,QString)),
        this,SLOT(slot_Login(QString,QString)));
    m_ploginDlg->showNormal();
    //初始化主窗口
    m_pMaindialog = new MainDialg;
    m_pMaindialog ->setWindowTitle("我的聊天软件");
    connect(m_pMaindialog,SIGNAL(SIG_closeWnd()),
            this,SLOT(slot_closeWnd()));
    connect(m_pMaindialog,SIGNAL(SIG_addFriend()),
            this,SLOT(slot_addFriend()));
    connect(m_pMaindialog,SIGNAL(SIG_alterIcon(QByteArray)),
            this,SLOT(slot_alterIconRq(QByteArray)));
//    MyFileInfo = new STRU_FILE_INFO_RQ;
//    p_mainWnd=new FeiQDialog;
//    p_mainWnd->showNormal();



//    //绑定关闭窗口的信号和槽函数
//    connect(p_mainWnd,SIGNAL(SIG_closeWnd()),
//             this,SLOT(slot_closeWnd()));




    //2：初始化中介者类指针
    m_pMediator = new UdpMediator;  //Udp的中介者类
    m_pTcpMediator = new TcpMediator;   //Tcp的中介者类
    //3:打开udp网络
    if(!m_pMediator -> OpenNet(0)){
        QMessageBox::about(NULL,"提示","打开Udp网络失败");
        exit(0);
    }
    //打开tcp网络
    if(!m_pTcpMediator->OpenNet(0)){
        QMessageBox::about(NULL,"提示","打开Tcp网络失败");
        exit(0);
    }



    //在接收信号的类里面，发送信号的对象new出来以后
    //绑定接收数据的信号和槽函数
    connect(m_pMediator,SIGNAL(SIG_ReadData(long,char*,int)),
            this,SLOT(slot_ReadData(long,char*,int)));

    //TCP接收文件
    connect(m_pTcpMediator,SIGNAL(SIG_ReadTcpData(char*,int)),
            this,SLOT(slot_ReadTcpData(char*,int)));


    //初始化TCP数据处理函数指针数组：
    InitTcpDealFun();

    //初始化语音类
    m_pMySound = CMySound::getMySignalSound();
    m_pMySound->m_id = NO_USER;
    //绑定语音信号和语音发送函数
    connect(m_pMySound,SIGNAL(SIG_soundInfo(int,char*)),
            this,SLOT(slot_sendVoid(int,char*)));

    //初始化视频类
    m_pVideoForm = Form::getSignalForm();
    //绑定信号与槽函数：
    m_pVideoForm->surface_->id = NO_USER;   //将窗口设为无人使用状态
    //设置窗口信息
    m_pVideoForm->setWindowTitle("我的视频窗口");
    //绑定发送文件的信号和槽函数
    connect(m_pVideoForm->surface_ ,SIGNAL(SIG_myImgInfo(long,  QByteArray)),
             this,SLOT(slot_sendMyImg(long,QByteArray)));
    //绑定关闭视频窗口的信号
    connect(m_pVideoForm,SIGNAL(SIG_closewindow(long)),
          this,SLOT(slot_closeVideo(long)));
    Other_pVideoForm = new FormFriend;
    Other_pVideoForm->m_id = NO_USER;
    //绑定信号和槽函数
    //绑定信号（对方发来的视频信号与绘制窗口函数绑定）
    connect(this,SIGNAL(SIG_painWindow(QImage)),
          Other_pVideoForm,SLOT(SLOT_videoDisplay(QImage))  );
    //绑定关闭视频窗口的信号
    connect(Other_pVideoForm,SIGNAL(SIG_closeFormfriend(long)),
          this,SLOT(slot_closeVideo(long)));


    //初始化二手书交易窗口
    m_pBooks = new CSecondHandBooks;
    connect(m_pMaindialog,SIGNAL(SIG_openBooks()),    //打开二手书交易窗口信号和槽函数
            this, SLOT(slot_openBooks()));
    connect(m_pBooks,SIGNAL(SIG_getBooksInfo(int,int)),   //绑定获取书籍信息请求的信号和槽函数
            this,SLOT(slot_getBooksInfo(int,int)));
    connect(m_pBooks,SIGNAL(SIG_getSelectBooksInfo(int,int,QString,bool)),   //绑定搜索书籍的信号和槽函数
            this,SLOT(slot_selectBooksInfo(int,int,QString,bool)));
    connect(m_pBooks,SIGNAL(SIG_getMyBooksInfo()),
            this,SLOT(slot_getMyBooksInfo()));
    m_pBooks->init();   //初始化二手书交易窗口，实际是发送一次所有类型物品的请求信息


    //初始化书籍上架窗口
    m_pAddBooks = new addNewBooks;
    connect(m_pBooks,SIGNAL(SIG_sellBooks()),   //打开书籍商家窗口信号和槽函数
            this, SLOT(slot_openSellBooks()));
    //绑定发送书籍上架信息信号和槽函数
    connect(m_pAddBooks,SIGNAL(SIG_SendBookInfo(QByteArray,QString,QString,QString,QString,QString,QString,int)),
           this, SLOT(slot_sendBooInfo(QByteArray,QString,QString,QString,QString,QString,QString,int)));

//    //发送一个广播上线请求
//    STRU_ONLINE rq;
//    gethostname(rq.name,sizeof(rq.name)); //获取主机名
//    qDebug()<<rq.name;
//    m_pMediator->SendData(INADDR_BROADCAST,(char*)&rq,sizeof(rq));

    //告诉服务器自己上线了
//        STRU_ONLINE rq;
//        gethostname(rq.name,sizeof(rq.name)); //获取主机名
//        rq.ISendIp = m_id; //将自己的序号填入，告诉udp服务器自己是谁
//        qDebug()<<rq.name;


//        GetInternetIP(Inernet_ip);  //获取本机上网所使用的公网ip
//    //    char buf[_DEF_MAX_SIZE];
//        Inernet_Ip = inet_addr(Inernet_ip.c_str());
//    //    string a="183.228.237.239";
//    //        Inernet_Ip = inet_addr(a.c_str());
//        rq.ISendIp = Inernet_Ip ;  //上线请求包中的ip等于自己的公网ip   （公网ip暂时无法获取成功，作废）

//        m_pMediator->SendData( inet_addr(ServerIP) ,(char*)&rq,sizeof(rq));


}

CKernel::~CKernel()
{
    qDebug()<<__func__;  //打印输出：在那个函数中


}


//处理接收到的所有网络数据
void CKernel::slot_ReadData(long lSendIp, char* buf,int nLen)
 {
    qDebug()<<__func__;
    //1：取出协议头
    int type = *(int *)buf;

    //2:根据协议头判断是那种数据包
    switch(type){
    case _DEF_PROTOCOL_ONLINE_RQ:
//        dealOnlineRq(lSendIp, buf, nLen);
        break;
    case _DEF_PROTOCOL_ONLINE_RS:
//          dealOnlineRs(lSendIp, buf, nLen);
        break;
    case _DEF_PROTOCOL_CHAT_RQ:
          dealChatRq(lSendIp, buf, nLen);
        break;
    case _DEF_PROTOCOL_OFFLINE_RQ:
//          dealOfflineRq(lSendIp, buf, nLen);
        break;
    case _DEF_PROTOCOL_FILE_INFO_RQ:
          dealFileRq(lSendIp, buf, nLen);   //处理文件信息请求
        break;
    case _DEF_PROTOCOL_FILE_INFO_RS:
          dealFileRs(lSendIp, buf, nLen);    //处理文件信息回复
        break;
    case _DEF_PROTOCOL_FILE_BLOCK_RQ:
        dealFileBlockRq(buf);
        break;
    case _DEF_VIDEO_TRAN:
        dealVideoTran(lSendIp, buf, nLen);
        break;
    case _DEF_VOID_TRAN:
        dealVoidInfo(lSendIp, buf, nLen);
        break;
          default:
//          dealVideoTran(lSendIp, buf, nLen);
          break;
    }

    //3:回收空间
    delete[] buf;

}
//处理双击某一行的信号
//void CKernel::slot_userClicked(QString ip)
//{
//    qDebug()<< __func__<<ip;
//    //找到该人对应的聊天窗口并显示
//    long lIp=inet_addr(ip.toStdString().c_str());
//    if(m_mapIpToChatdlg.find(lIp) != m_mapIpToChatdlg.end())
//    {
//        ChatDialog* chat=m_mapIpToChatdlg[lIp];
//        chat->showNormal();
//    }
//}

//处理聊天内容
void CKernel::slot_sendMsg(int id, QString content)
{
    qDebug()<<__func__;
    //发送聊天请求给对端
    STRU_CHAT rq;
    rq.m_id=m_id;  //将自己的编号填入
    rq.ISendIp=id;    //将目标主机ip地址存入包中
    strcpy(rq.content, content.toStdString().c_str());
    m_pMediator->SendData(0,(char*)&rq,sizeof(rq));
}


//关闭窗口
//void CKernel::slot_closeWnd()
//{
//    qDebug()<<__func__;
//    //结束生命线程
//    LiveStop=true;
//    //1：发送下线请求
//    STRU_OFFLINE rq;
//    rq.m_id=m_id;
//    m_pMediator->SendData(INADDR_BROADCAST, (char*)&rq, sizeof(rq));

//    //回收资源
////    if(p_mainWnd){
////        p_mainWnd->hide();
////        delete p_mainWnd;
////        p_mainWnd = NULL;
////    }

//    if(m_pMediator){
//        m_pMediator->CloseNet();
//        delete m_pMediator;
//        m_pMediator = NULL;
//    }

//    for(auto ite = m_mapIpToChatdlg.begin(); ite!=m_mapIpToChatdlg.end() ;)
//    {
//        ChatDialog* chat =*ite;
//        chat->hide();
//        delete chat;
//        chat=NULL;
//        ite = m_mapIpToChatdlg.erase(ite);
//    }

//    //3:结束进程
//        exit(0);
//}

//处理发送文件信号的槽函数
void CKernel::slot_sendFile(QString ip, QFileInfo FileInfo)
{
    qDebug()<<__func__;


    STRU_FILE_INFO_RQ *crq=new STRU_FILE_INFO_RQ;  //用于发送文件块儿的时候
//    crq->ISendIp=inet_addr(ip.toStdString().c_str());
    crq->m_id=m_id;
    qDebug()<<crq->ntype;
    //配置协议头信息
    //制作文件唯一标识
    std::string fName=FileInfo.fileName().toStdString();//文件名  QString转换成string

   strncpy_s(crq->szFileName,sizeof(crq->szFileName),fName.c_str(),_MAX_FILE_PATH);//将const char*复制到char[];
//cout<<rq.szFileName<<"ssssss"<<endl;
    QString fTime=QTime::currentTime().toString("hh:mm:ss");   //时间

    std::string fMask=fName+fTime.toStdString();//生成文件唯一标识

    //填入文件路径信息：
     std::string fPath=FileInfo.filePath().toStdString();//文件名  QString转换成string
        strncpy_s(crq->szFilePath,sizeof(crq->szFilePath),fPath.c_str(),_MAX_FILE_PATH);//将const char*复制到char[];


//    cout<<fMask<<"sss"<<endl;
//    rq.szFileID=fMask.c_str();
        //填入文件唯一标识
    strncpy_s(crq->szFileID,sizeof(crq->szFileID),fMask.c_str(),_MAX_FILE_PATH);
//    cout<<rq.szFileID<<endl;
     gethostname(crq->name,sizeof(crq->name)); //获取主机名

     crq->szFileSize=FileInfo.size();

     memcpy(MyFileInfo,crq,sizeof(STRU_FILE_INFO_RQ));   //将rq的值给crq复制一份儿。

     crq->ISendIp=inet_addr((ip.toStdString().c_str()));   //将目标主机的ip地址存入包中告诉服务器

    m_pMediator->SendData(inet_addr((ip.toStdString().c_str())),(char*)crq,sizeof(STRU_FILE_INFO_RQ));

}

//发送视频通话请求
void CKernel::slot_videoRq(long id)
{
    qDebug()<<__func__;
    STRU_VIDEO_RQ rq;
    strcpy(rq.name,m_name.toStdString().c_str());
    rq.m_id=m_id;
    qDebug()<<rq.m_id;
//    rq.ISendIp=inet_addr(ip.toStdString().c_str());
//    rq.type=_DEF_VIDEO_RQ;
     rq.ISendIp = id;
     m_pTcpMediator->SendData(/*inet_addr((ip.toStdString().c_str()))*/0,(char*)&rq,sizeof(rq));
    //显示自己的视频窗口，但是此时摄像头还没有启动
     m_pVideoForm->show();

     //测试显示窗口是否管用
//     qDebug()<<"8888888888888888";
//     Other_pVideoForm = new FormFriend;
//     QString ip__ = QString::number(2);
//     Other_pVideoForm->setWindowTitle(ip__);
//     Other_pVideoForm->showNormal();


}

//发送语音通话请求
void CKernel::slot_SoundRq(long id)
{

    m_pMySound->m_id = id;
    STRU_VOID_RQ rq;
    rq.ISendIp = id;
    rq.m_id=m_id;
    strcpy(rq.name, m_name.toStdString().c_str());
    m_pTcpMediator->SendData(0,(char*)&rq, sizeof(rq));
}

//发送关闭语音通话的申请
void CKernel::slot_closeVoid(long id)
{
//     m_pMySound->endMySound();  //关闭声音采集
     m_pMySound->endOutSound();   //暂停声音播放
     m_pMySound->m_id = NO_USER;  //将语音指针设置无人使用的状态
    STRU_CLOSE_VOID rq;
    rq.ISendIp = id;
    rq.m_id = m_id;
    m_pTcpMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//发送关闭视频通话申请
void CKernel::slot_closeVideo(long id)
{
    //暂时只发送语音通话关闭，不发送视频通话关闭
    m_pMySound->endOutSound();   //暂停声音播放
    m_pMySound->m_id = NO_USER;  //将语音指针设置无人使用的状态
   STRU_CLOSE_VOID rq;
   rq.ISendIp = id;
   rq.m_id = m_id;
   m_pTcpMediator->SendData(0,(char*)&rq,sizeof(rq));
//   //回收视频类
//   if(m_pVideoForm)
//    delete m_pVideoForm;
//   m_pVideoForm = NULL;
//   if(Other_pVideoForm)
//   delete Other_pVideoForm;
//   Other_pVideoForm = NULL;


   //关闭摄像头，并将视频窗口隐藏
   m_pVideoForm->hide();
   m_pVideoForm->surface_->id = NO_USER;
   Other_pVideoForm->hide();
   Other_pVideoForm->m_id = NO_USER;
   m_pVideoForm->cameraStop();

}

//处理需要发送的书籍信息
void CKernel::slot_sendBooInfo(QByteArray byte, QString name, QString Area, QString info, QString price, QString QQ, QString weChat, int isSchool)
{
    //TODO:将数据转码
    char *bookNameGb = NULL;
    char *bookAreaGb = NULL;
    char *bookInfoGb = NULL;
    char *bookPriceGb = NULL;
   utf8_Gb2312_Change(name,bookNameGb,sizeof(name));
   utf8_Gb2312_Change(Area,bookAreaGb,sizeof(Area));
   utf8_Gb2312_Change(info,bookInfoGb,sizeof(info));
   utf8_Gb2312_Change(price,bookPriceGb,sizeof(price));

   qDebug()<<bookNameGb;
qDebug()<<bookInfoGb;
    int id = m_id;
    PackType type = _DEF_PROTOCOL_TCP_ADD_BOOKS_IMG_RQ;
    QByteArray byteId = QByteArray((char*)&id,4);
    QByteArray byteType = QByteArray((char*)&type,4);
//    char bookName[_DEF_BOOK_NAME_SIZE];
//    strcpy(bookName,bookNameGb);
     QByteArray byteName = QByteArray(bookNameGb,_DEF_BOOK_NAME_SIZE);
     byte.push_front(byteName);
    byte.push_front(byteId);
    byte.push_front(byteType);
    //通过tcp将书籍图片信息发送给服务器
    m_pTcpMediator->SendData(0,byte.data(),byte.size());
    //将书籍文字信息发送给服务器
    STRU_TCP_ADD_BOOKS rq;
    strcpy(rq.bookName,bookNameGb);
    strcpy(rq.bookInfo,bookInfoGb);
    strcpy(rq.bookArea,bookAreaGb);
    strcpy(rq.bookPrice,bookPriceGb);
    strcpy(rq.QQ,QQ.toStdString().c_str());
    strcpy(rq.weChat,weChat.toStdString().c_str());
    rq.id = m_id;
    rq.isSchool = isSchool;
    m_pTcpMediator->SendData(0,(char*)&rq,sizeof(rq));


    delete []bookNameGb;
    bookNameGb = NULL;
    delete []bookAreaGb;
    bookAreaGb = NULL;
    delete []bookInfoGb;
    bookInfoGb = NULL;
    delete []bookPriceGb;
    bookPriceGb = NULL;
}

//处理需要发送的视频图片信息
void CKernel::slot_sendMyImg(long id, QByteArray content)
{
    qDebug()<<__func__;


        qDebug()<<__func__;
    qDebug()<<content.size();
//    STRU_VIDEO_TRAN rq; //视频通话信息包
//    rq.ISendIp = id;
//    rq.m_id = m_id;
//    rq.content=QByteArray(content.data(),content.size());//将content赋值到rq中，用QByteArray的构造函数，长度为content.size();
//    rq.content=QString(content); //QByteArray转QString
//    rq.size=content.size();

//        char* buf = (char*)&rq;
    int t=1012;
    long ip=id;

    QByteArray type=QByteArray((char*)&t,4);   //将t存入QByteArray，
        QByteArray targetid=QByteArray((char*)&ip,4);

        content.push_front(targetid);//第一个参数为数据类型，第二个参数为数据目标主机id，（因为此处无法将QByteArray放到结构体中发送，因为那只会发送其数据首地址而非数据本身
        //所以将信息全部放到QByteArray中）
        content.push_front(type);  //在QByteArray头部插入数据
        //控制发送图片信息的频率
        ee++;
        if(1 < ee)
        {
            ee=0;
        //QByteArray中的数据是以QByteArray::data()地址开始的，即QByteArray::data()返回的是数据首部地址
            m_pMediator->SendData(/*inet_addr((ip.toStdString().c_str()))*/0,(char*)content.data(),content.size());
        }



//测试


}

//处理需要发送的音频/语音信息
void CKernel::slot_sendVoid(int lens, char *data)
{
    qDebug()<<__func__;
    STRU_VOID_TRAN rq;
    strcpy(rq.voidData,data);
    rq.lens = lens;
    rq.m_id = m_id;
    rq.ISendIp = m_pMySound->m_id;   //填入对方id
   m_pMediator->SendData(0,(char*)&rq,sizeof(rq));
//        m_pMySound->listenVoid(rq.voidData,rq.lens);
   qDebug()<<__func__;
       delete []data;
   data = NULL;
}

//打开二手书交易窗口，并发消息给服务器，让服务器发送书籍信息给自己
void CKernel::slot_openBooks()
{
    m_pBooks->showNormal();

}

//发送获取二手书信息请求
void CKernel::slot_getBooksInfo(int isSchool , int num)
{
    STRU_TCP_BOOKS_INFO_RQ rq;
    rq.isSchool = isSchool;
    rq.num = num;
    m_pTcpMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//发送搜索二手书信息的请求
void CKernel::slot_selectBooksInfo(int isSchool, int num, QString bookName , bool isClear)
{
    //发送之前先将搜索窗口清空
    if(CLEAR_SELECT_WDG == isClear)   //判断是否需要清空
    {
         for(auto ite = m_mapSelectBooks.begin(); ite != m_mapSelectBooks.end(); )
        {
            Books* bookWdg = *ite;
            delete bookWdg;
            bookWdg = NULL;
            ite = m_mapSelectBooks.erase(ite);
        }
    }
    STRU_TCP_BOOKS_INFO_RQ rq;
    rq.isSelect = SELECT_BOOK;
    rq.isSchool = isSchool;
    rq.num = num;
    //转码
    char* bookNameGb = NULL;
    utf8_Gb2312_Change(bookName,bookNameGb,sizeof(bookName));
    strcpy(rq.bookName,bookNameGb);
    m_pTcpMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//获取本机上架物品的所有信息
void CKernel::slot_getMyBooksInfo()
{
    STRU_TCP_SELECT_MY_BOOKS_INFO_RQ rq;
    rq.id = m_id;
    m_pTcpMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//发送物品下架的请求
void CKernel::slot_deleteTargetBooks(int isSchool,QString bookName)
{
    STRU_TCP_DELETE_BOOKS_INFO_RQ rq ;
    //转码
    char *bookNameGb = NULL;
    utf8_Gb2312_Change(bookName,bookNameGb,sizeof(bookName));
    strcpy(rq.bookName,bookNameGb);
    rq.id = m_id;
    rq.isSchool = isSchool;
    m_pTcpMediator->SendData(0, (char*)&rq, sizeof(rq));

}

//发送更换头像申请
void CKernel::slot_alterIconRq(QByteArray img)
{
    //发送更换头像申请
    //将协议头以及id插入到图片信息前面去
    int typeInt = _DEF_PROTOCOL_TCP_ALTER_ICON_RQ;
    int idInt = m_id;
    QByteArray id = QByteArray((char*)&idInt,4);
    QByteArray type = QByteArray((char*)&typeInt,4);
    QByteArray info = type + id + img;
    m_pTcpMediator->SendData(0,info.data(),info.size());

    return;

}

//打开上传二手书信息窗口
void CKernel::slot_openSellBooks()
{
    m_pAddBooks->showNormal();
}

// //处理需要发送的音频信息
//void CKernel::slot_sendVoid(char *data, int lens)
//{
//    STRU_VOID_TRAN rq;
//    rq.m_id=m_id;
//    rq.ISendIp=m_pVoid->ip;
//    rq.lens=lens;
//    memcpy(rq.voidData,data,lens);
//    rq.m_id=m_id;
//    m_pMediator->SendData(0,(char*)&rq,sizeof(rq));
//}

//==============TCP===================================================================================
void CKernel::slot_ReadTcpData(char *buf, int nLen)
{
    qDebug()<<__func__;
    //1:取出协议头
        PackType type = *(PackType*)buf;

        //2:判断协议头是否在数组下标的有效范围内
        if (type >= _DEF_PROTOCOL_BASE + 1 && type <= _DEF_PROTOCOL_BASE  + _DEF_PROTOCOL_COUNT) {
            //3:计算数组下标，取出下标对应的函数地址
            pTcpFun pf = TCP_TYPE_PFUN(type);
            //4:判断指针不为空，就调用指针指向的函数
            if (pf) {
                (this->*pf)(buf, nLen);   //注意这个this不能漏掉了！！！
            }
        }
        //3:回收空间
        delete[] buf;
}

//处理注册界面发来的注册信号
void CKernel::slot_register(QString name, QString tel, QString password)
{
    qDebug()<<__func__;
    //先将中文转码：
    char* pName = NULL;
    utf8_Gb2312_Change(name,pName,sizeof(name));
    //将信息打包装进结构体中发给服务器
    STRU_TCP_REGISTER_RQ rq;


    memcpy(rq.name, pName,100);  //将昵称复制到rq中
    strcpy(rq.password,password.toStdString().c_str());
    strcpy(rq.tel,tel.toStdString().c_str());

     //将注册信息通过tcp中介者发送给服务器
    m_pTcpMediator->SendData(0,(char*)&rq,sizeof(rq));

}


//处理登录界面发来的登录信号
void CKernel::slot_Login(QString account, QString password)
{
    STRU_TCP_LOGIN_RQ rq;
    strcpy(rq.password, password.toStdString().c_str());
    strcpy(rq.tel,account.toStdString().c_str());
    //将注册信息通过tcp中介者发送给服务器
    m_pTcpMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//处理主窗口发来的窗口关闭信号
void CKernel::slot_closeWnd()
{
    qDebug()<<__func__;
    //发送下线请求
    STRU_TCP_OFFLINE_RQ rq;
    rq.userId=m_id;
      m_pTcpMediator->SendData(0,(char*)&rq,sizeof(rq));
    LiveStop = true;
        if(m_pMediator)
        {
            delete m_pMediator;
            m_pMediator=NULL;
        }

        if(m_pTcpMediator)
        {
            delete m_pTcpMediator;
            m_pTcpMediator=NULL;
        }

        if(m_pMySound)
        {
           delete m_pMySound;
        }

        for(auto ite = m_mapIdToUserItem.begin(); ite != m_mapIdToUserItem.end() ; )   //回收聊天窗口
        {
                    useritem* user =*ite;
                    user->hide();
                    delete user;
                    user=NULL;
                    ite = m_mapIdToUserItem.erase(ite);
        }


        for(auto ite = m_mapIdBookNameToBooks.begin(); ite != m_mapIdBookNameToBooks.end() ; )   //回收书籍窗口
        {
                    Books* book =*ite;
                    book->hide();
                    delete book;
                    book=NULL;
                    ite = m_mapIdBookNameToBooks.erase(ite);
        }

        if(m_pVideoForm)
        {
            delete m_pVideoForm;
            m_pVideoForm = NULL;
        }
        if(Other_pVideoForm)
        {
            delete Other_pVideoForm;
            Other_pVideoForm = NULL;
        }
    //    if(MyFileInfo)
    //    {
    //        delete MyFileInfo;
    //        MyFileInfo = NULL;
    //    }

}

//添加好友
void CKernel::slot_addFriend()
{
    qDebug()<<__func__;
    //1:弹出一个窗口，让用户填写好友的昵称
    QString friendName = QInputDialog::getText(m_pMaindialog,"添加好友", "输入好友昵称");

    //校验用户的输入是否合法
    if(friendName.length() > 20){
        QMessageBox::about(m_pMaindialog,"提示","昵称不合法");
        return ;
    }

    //:3：判断是不是自己，是自己的昵称不需要添加好友
    if(friendName == m_name){
        QMessageBox::about(m_pMaindialog,"提示","不能添加自己为好友");
        return ;
    }

    //4:判断是不是已经是好友了
    for(auto ite = m_mapIdToUserItem.begin(); ite!= m_mapIdToUserItem.end(); ite++){
        useritem* item = *ite;
        if(item->m_name == friendName){
            QMessageBox::about(m_pMaindialog, "提示", "已经是好友啦，不需要再添加啦");
            return ;
        }
    }

    //5:发送添加好友请求给服务器
    STRU_TCP_ADD_FRIEND_RQ rq;
    //名字需要查数据库，需要转码
//    utf8ToGb2312(friendName, rq.friendName, sizeof(rq,friendName));
    char* name=NULL;
    utf8_Gb2312_Change(friendName,name,sizeof(friendName));
    strcpy(rq.friendName,name);
    rq.userId = m_id;
    strcpy(rq.userName, m_name.toStdString().c_str());
    m_pTcpMediator->SendData(0,(char*)&rq, sizeof(rq));
}








//=============UDP=======================================================================================
//处理上线请求
//void CKernel::dealOnlineRq(long lSendIp, char *buf, int nLen)
//{
//    qDebug()<< __func__;
//    //1:拆包
//    STRU_ONLINE* rq = (STRU_ONLINE*)buf;

//    //2：添加到好友列表中
//    p_mainWnd->addFriend(QString(INet::GetIpString(rq->m_id).c_str()), rq->name);
//    //3:创建一个与该人聊天的窗口（隐藏）
//     ChatDialog* chat = new ChatDialog;
//     //设置窗口信息
//     chat->setInfo(INet::GetIpString(rq->m_id).c_str());


//     //绑定发送聊天数据的信号和槽函数
//     connect(chat,SIGNAL(SIG_sendMsg(QString,QString)),
//             this,SLOT(slot_sendMsg(QString,QString)));

//     //绑定发送文件的信号和槽函数
//     connect(chat,SIGNAL(SIG_sendFile(QString,QFileInfo)),
//              this,SLOT(slot_sendFile(QString,QFileInfo)));

//     //绑定发送视频通话的信号和槽函数
//     connect(chat,SIGNAL(SIG_videoRq(QString)),
//              this,SLOT(slot_videoRq(QString)));

//     //4：把聊天窗口放到map中管理
//     m_mapIpToChatdlg[rq->m_id]=chat;


//     //由于服务器是通过内网穿透才能被访问的，所以服务器获得的ip地址都是一个，现在在上线请求包里添加本机的公网ip地址+主机名来判断该上线请求包是不是本地主机发出的。
//     //因为不同的主机上网所用的公网ip也许是同一个，所以需要增加本地主机名作为判断依据；

//    //5：判断是不是自己，如果是自己就结束   （暂时直接通过主机名来判断是不是自己）
//     int i=0;
//     //获取主机名字
//     char nameBuf[100]="";
//     gethostname(nameBuf, 100);

//        if(! strcmp(nameBuf,rq->name ) )
//        {
//            //如果是自己则将服务器分配的本地主机的编号填入
//            m_id=rq->m_id;
//            return ;
//        }
//        //双重判断
//        //首先获取本机ip列表，再根据本机ip列表判断

//        //2:根据主机名获取ip地址列表
////        struct hostent *remoteHost = gethostbyname(nameBuf);
////        while(remoteHost->h_addr_list[i] != 0){

////            if( *(u_long *) remoteHost->h_addr_list[i] == rq->ISendIp)
////                return;
////            i++;

////        }

//    //6：不是自己，发送上线回复
//    STRU_ONLINE rs;
//    rs.m_id=m_id;    //将自己的编号填入
//    rs.ISendIp = rq->m_id; //将目标主机的编号填入
//    rs.type = _DEF_PROTOCOL_ONLINE_RS;
//    gethostname(rs.name,sizeof(rs.name));
//    m_pMediator->SendData(rq->m_id,(char*)&rs,sizeof(rs));
//}

////处理上线回复
//void CKernel::dealOnlineRs(long lSendIp, char *buf, int nLen)
//{
//    qDebug()<<__func__;
//    //1:拆包
//    STRU_ONLINE* rq=(STRU_ONLINE*)buf;

//    //2：添加到好友列表中
//    p_mainWnd->addFriend(QString(INet::GetIpString(rq->m_id).c_str()),rq->name);

//    //3：创建一个与改任的聊天窗口（但是不显示）
//    ChatDialog* chat = new ChatDialog;
//    //绑定发送聊天数据的信号和槽函数
//    connect(chat,SIGNAL(SIG_sendMsg(QString,QString)),
//            this,SLOT(slot_sendMsg(QString,QString)));

//    //绑定发送文件的信号和槽函数
//    connect(chat,SIGNAL(SIG_sendFile(QString,QFileInfo)),
//             this,SLOT(slot_sendFile(QString,QFileInfo)));

//    //绑定发送视频通话的信号和槽函数
//    connect(chat,SIGNAL(SIG_videoRq(QString)),
//             this,SLOT(slot_videoRq(QString)));

//    //设置窗口信息
//    chat->setInfo(QString(INet::GetIpString(rq->m_id).c_str()));

//    //4：把聊天窗口放到map中管理
//    m_mapIpToChatdlg[rq->m_id]=chat;
//}

//处理聊天请求
void CKernel::dealChatRq(long lSendIp, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1:拆包
    STRU_CHAT* rq=(STRU_CHAT*)buf;

    //2：判断聊天窗口是否存在
    if(m_mapIdToUserItem.count(rq->m_id)>0){   //包里的m_id是发送者的id
        //3：取出聊天窗口，设置聊天内容到窗口上:
        m_mapIdToUserItem[rq->m_id]->setContent(rq->content);
    }
}

//处理视频通话请求
void CKernel::dealVideoRq( char *buf, int nLen)
{
    qDebug()<<__func__;
    //1:拆包，看是谁发来的
    STRU_VIDEO_RQ* rq=( STRU_VIDEO_RQ*)buf;
    qDebug()<<rq->m_id;
    QString name = QString(rq->name);
    if(NO_USER == m_pMySound->m_id)
    {
    QString info = QString("  给您发来了视频通话的请求，是否接收？ ");
    QString infor = name+info;
    int inf = QMessageBox::information(NULL,"提示",infor,QMessageBox::Yes | QMessageBox::No );

    /*
        STRU_VOID_RQ *rq=(STRU_VOID_RQ*)buf;
    STRU_VOID_RS rs;
    rs.ISendIp = rq->m_id;
    rs.m_id = m_id;
  strcpy(rs.name,m_name.toStdString().c_str());
    if(NO_USER != m_pMySound->m_id)   //如果有人在使用语音指针
    {
        rs.IsAccept = BUSY;
        m_pTcpMediator->SendData(0,(char*)&rs,sizeof(rs));
        return;
    }*/

    if(QMessageBox::Yes == inf )   //同意该视频通话请求
    {
        STRU_VIDEO_RS rs;
        strcpy(rs.name,m_name.toStdString().c_str());
        rs.m_id = m_id;
        rs.ISendIp = rq->m_id;
        rs.IsAccept = true;
         m_pTcpMediator->SendData(0,(char*)&rs,sizeof(STRU_FILE_INFO_RQ));
//================================================================================
//         //开启自己的摄像头：

         //form是视频通话窗口
         m_pVideoForm->surface_->id = rq->m_id;
//         QString ip_ = QString::number(rq->m_id);
         Sleep(10);
          m_pVideoForm->cameraStart();
         m_pVideoForm->showNormal();

         //并开启别人的视频通话窗口
         qDebug()<<"8888888888888888";

         Other_pVideoForm->m_id = rq->m_id;
         Other_pVideoForm->displayStart();
         Other_pVideoForm->setWindowTitle(QString("【%1】的视频窗口").arg(QString(rq->name)));
         Other_pVideoForm->showNormal();


         m_pMySound = CMySound::getMySignalSound();  //语音
         m_pMySound->initMySound();
          m_pMySound->m_id=rq->m_id;
         //创建语音类
//         m_pVoid = new Myvoid;
         //绑定信号和槽函数
//         m_pVoid->ip=rq->m_id;
//         connect(m_pVoid,SIGNAL(SIG_sendVoid(char*, int)),
//               this,SLOT(slot_sendVoid(char*,int)));
    }
    return;
    }
      //拒绝该视频通话请求 或者本机正在与其他主机通话

        STRU_VIDEO_RS rs;
        rs.m_id = m_id;
        rs.ISendIp = rq->m_id;
        strcpy(rs.name,m_name.toStdString().c_str());
        qDebug()<<rs.ISendIp;
        rs.IsAccept = false;
         m_pTcpMediator->SendData(0,(char*)&rs,sizeof(STRU_FILE_INFO_RQ));


}

//处理视频通话回复
void CKernel::dealVideoRs( char *buf, int nLen)
{
    qDebug()<<__func__;
        //1：拆包查看是谁发来的
    STRU_VIDEO_RS* rs=( STRU_VIDEO_RS*)buf;
    //2:查看是否同意
    if(rs->IsAccept)
    {
        //=================================================================
//        //开启别人的视频通话窗口
        QString ip__ = QString::number(rs->m_id);
        Other_pVideoForm->setWindowTitle(QString("【%1】的视频窗口").arg(QString(rs->name)));
        Other_pVideoForm->m_id = rs->m_id;
        Other_pVideoForm->displayStart();
        Other_pVideoForm->showNormal();

        //开始发送视频图片信息
        m_pVideoForm->surface_->id = rs->m_id;
        m_pVideoForm->cameraStart();
        m_pMySound = CMySound::getMySignalSound();
        m_pMySound->initMySound();
        m_pMySound->m_id=rs->m_id;

        //===================================================
        //创建语音类s

//        m_pVoid = new Myvoid;
//             m_pVoid->ip=rs->m_id;
//        //绑定信号和槽函数
//        connect(m_pVoid,SIGNAL(SIG_sendVoid(char*, int)),
//              this,SLOT(slot_sendVoid(char*,int)));

    }
    else{
        //:销毁form对象  form.ui是视频通话窗口
        QMessageBox::information(NULL,"提示","对方正忙或者拒绝了您的视频通话申请");
    }
}

//处理接收到的语音通话请求
void CKernel::dealVoidRq(char *buf, int nLen)
{
        STRU_VOID_RQ *rq=(STRU_VOID_RQ*)buf;
    STRU_VOID_RS rs;
    rs.ISendIp = rq->m_id;
    rs.m_id = m_id;
  strcpy(rs.name,m_name.toStdString().c_str());
    if(NO_USER != m_pMySound->m_id)   //如果有人在使用语音指针
    {
        rs.IsAccept = BUSY;
        m_pTcpMediator->SendData(0,(char*)&rs,sizeof(rs));
        return;
    }

    //1：判断是谁发来的并显示(注意：发送语音童话请求的槽函数还未绑定，此函数暂未映射到函数指针数组中)
    int result = QMessageBox::information(m_pMaindialog,"提示",QString("【%1】给您发来了语音童话申请").arg(rq->name),QMessageBox::Yes | QMessageBox::No);
             //2：发送语音通话请求的回复

    if(QMessageBox::Yes == result)
    {
        rs.IsAccept = ACCEPT;
        m_pMySound->initMySound();
        m_pMySound->m_id=rq->m_id;
    }
    else{
        rs.IsAccept = REFUSE;
    }
        //发送回复
    m_pTcpMediator->SendData(0,(char*)&rs,sizeof(rs));

}

//处理接收到的语音请求回复
void CKernel::dealVoidRs(char *buf, int nLen)
{
    STRU_VOID_RS* rs = (STRU_VOID_RS*)buf;
    if(ACCEPT == rs->IsAccept)
    {
        //开始发送语音包
        m_pMySound->initMySound();
        m_pMySound->m_id = rs->m_id;
    }
    else if(REFUSE == rs->IsAccept){
        QMessageBox::information(m_pMaindialog,"提示",QString("【%1】拒绝了您的语音通话申请").arg(rs->name));
    }
    else{
         QMessageBox::information(m_pMaindialog,"提示",QString("【%1】忙线中").arg(rs->name));
    }
}

//处理接收到的挂断语音申请
void CKernel::dealCloseVoid(char *buf, int nLen)
{
    QMessageBox::information(m_pMaindialog,"提示","对方已经挂断语音通话");
    m_pMySound->endOutSound();
    m_pMySound->m_id = NO_USER;
}

//处理接收到的上架回复信息
void CKernel::dealAddBookRs(char *buf, int nLen)
{
    STRU_TCP_ADD_BOOKS_RS *rs = (STRU_TCP_ADD_BOOKS_RS*)buf;
    char *bookNameGb = new char[_DEF_BOOK_NAME_SIZE];
    strcpy(bookNameGb,rs->bookName);
    QString bookName;
    utf8_Gb2312_Change(bookName,bookNameGb,_DEF_BOOK_NAME_SIZE);
    if( SUBMIT_SUCCESS == rs->result)
    {
        QString text = QString("书籍《%1》上架成功！").arg(bookName);
        QMessageBox::information(NULL,"提示",text);
    }
    else if(REPEAT_SUBMIT == rs->result)
    {
                QString text = QString("请勿重复上架书籍《%1》，此次上架失败！").arg(bookName);
        QMessageBox::information(NULL,"提示",text);
    }
    else
    {
         QString text = QString("书籍《%1》上架失败,请检查数据是否合法或练习我们！").arg(bookName);
        QMessageBox::information(NULL,"提示",text);
    }

}

//处理收到的书籍图片信息
void CKernel::dealBooksImgInfoRq(char *buf, int nLen)
{

    qDebug()<<__func__;
    //顺序是：type,bookid,isSchool,img,大小为4，5，5
        char bookid[5] = "";
        char isSchool[5] = "";
        memcpy(bookid,buf+4,5);
        memcpy(isSchool,buf+4+5,5);
        QString idb = QString(bookid);
        QString isSchoolstring = QString(isSchool);
        QString idString =  isSchoolstring + idb;
    //将bookid以及isSchool拼接到一起作为键值，窗口为实值存入一个map中便于管理
    //首先要判断map中有没有这个键值

    if(0 < m_mapIdBookNameToBooks.count(idString))
    {
        //如果map中已经有了就不保存，直接返回
        return;
    }
    else{
        Books* bookWdg = new Books;
           FILE* fp;
            char imgPath[_DEF_IMG_PATH_SIZE] = "";
            strcpy(imgPath,".//img//");
            strcat(imgPath,idString.toStdString().c_str());
            strcat(imgPath,".jpg");  //拼好路径
            if(0 == fopen_s(&fp,imgPath,"rb")) //以二进制读打开，如果成功则说明已有该文件，则返回
            {
                strcpy(bookWdg->imgPath, imgPath);  //将路径信息复制过去
                  bookWdg->setImgInfo();
             m_mapIdBookNameToBooks[idString] = bookWdg;  //将窗口添加到map中

            }    //如果打不开，则写入
            else if(0 == fopen_s(&fp,imgPath,"wb"))   //以二进制写的方式打开
            {
                //如果打开成功，将图片信息写入
                int number = fwrite(buf + 14,nLen  - 14,1,fp);   //前面的是其他信息
                fclose(fp);  //关闭文件
                if(number > 0)
                {
                    strcpy(bookWdg->imgPath, imgPath);  //将路径信息复制过去
                      bookWdg->setImgInfo();
                 m_mapIdBookNameToBooks[idString] = bookWdg;  //将窗口添加到map中
                }
            }

         int school = atoi(isSchool);
         m_pBooks->addBooksWdg(bookWdg,school);       //将窗口添加到secondhandbooks的对应窗口中
         return ;

    }





}

//处理收到的书籍文字信息
void CKernel::dealBooksTextInfoRq(char *buf, int nLen)
{
    qDebug()<<__func__;
    //首先拆包
    STRU_TCP_BOOKS_TEXT_INFO_RS* rs=(STRU_TCP_BOOKS_TEXT_INFO_RS*)buf;
    //转换编码格式
    QString bookNameUt;
    QString bookAreaUt;
    QString bookPriceUt;
    QString bookInfoUt;
    char *bookName = new char[_DEF_BOOK_NAME_SIZE];
    char *bookArea = new char[_DEF_BOOK_AREA_SIZE];
    char *bookPrice = new char[_DEF_BOOK_PRICE_SIZE];
    char *bookInfo = new char[_DEF_BOOK_INFO_SIZE];

    strcpy(bookName,rs->bookName);
    strcpy(bookArea,rs->bookArea);
    strcpy(bookPrice,rs->bookPrice);
    strcpy(bookInfo,rs->bookInfo);

    utf8_Gb2312_Change(bookNameUt,bookName,_DEF_BOOK_NAME_SIZE);
    utf8_Gb2312_Change(bookAreaUt,bookArea,_DEF_BOOK_AREA_SIZE);
    utf8_Gb2312_Change(bookPriceUt,bookPrice,_DEF_BOOK_PRICE_SIZE);
    utf8_Gb2312_Change(bookInfoUt,bookInfo,_DEF_BOOK_INFO_SIZE);


    //查看是哪一本书的信息
//    char id[5] = "";     //id大小标准为5
//    itoa(rs->id,id,10);
//    QString id_string = QString(id);
    char bookid[5]="";
    itoa(rs->bookid,bookid,10);   //将int型转换为字符串,10是10进制
    char isSchool[5]="";
    itoa(rs->isSchool,isSchool,10);
    QString bookidstring = QString(isSchool)+QString(bookid);
    if( 0 < m_mapIdBookNameToBooks.count(bookidstring))   //如果map中存在，则将信息存入到对应的书籍窗口中去
    {
        Books* bookWdg = m_mapIdBookNameToBooks[bookidstring];
        bookWdg->setTextInfo(bookNameUt,bookAreaUt,bookPriceUt,bookInfoUt,rs->QQ,rs->weChat);   //设置书籍文字信息
        bookWdg = NULL;
    }


    delete []bookName;
    bookName = NULL;
    delete []bookArea;
    bookArea = NULL;
    delete []bookPrice;
    bookPrice = NULL;
    delete []bookInfo;
    bookInfo = NULL;
}

//处理搜索的书籍图片信息
void CKernel::dealSelectBooksImgInfoRq(char *buf, int nLen)
{
    qDebug()<<__func__;
    //顺序是：type,bookid,isSchool,img,大小为4，5，5
        char bookid[5] = "";
        char isSchool[5] = "";
        memcpy(bookid,buf+4,5);
        memcpy(isSchool,buf+4+5,5);
        QString idb = QString(bookid);
        QString isSchoolstring = QString(isSchool);
        QString idString =  isSchoolstring + idb;


    if(0 < m_mapSelectBooks.count(idString))
    {
        //如果map中已经有了就不保存，直接返回
        return;
    }
    else{
        Books* bookWdg = new Books;
           FILE* fp;
            char imgPath[_DEF_IMG_PATH_SIZE] = "";
            strcpy(imgPath,".//img//");
            strcat(imgPath,idString.toStdString().c_str());
            strcat(imgPath,".jpg");  //拼好路径
            if(0 == fopen_s(&fp,imgPath,"rb")) //以二进制读打开，如果成功则说明已有该文件，则返回
            {
                strcpy(bookWdg->imgPath, imgPath);  //将路径信息复制过去
                  bookWdg->setImgInfo();
             m_mapSelectBooks[idString] = bookWdg;  //将窗口添加到map中

            }    //如果打不开，则写入
            else if(0 == fopen_s(&fp,imgPath,"wb"))   //以二进制写的方式打开
            {
                //如果打开成功，将图片信息写入
                int number = fwrite(buf + 14,nLen  - 14,1,fp);   //前面的是其他信息
                fclose(fp);  //关闭文件
                if(number > 0)
                {
                    strcpy(bookWdg->imgPath, imgPath);  //将路径信息复制过去
                      bookWdg->setImgInfo();
                 m_mapSelectBooks[idString] = bookWdg;  //将窗口添加到map中
                }
            }

         int school = atoi(isSchool);
         m_pBooks->addBooksWdg(bookWdg,SELECT_BOOK);       //将窗口添加到secondhandbooks的对应窗口中
        bookWdg = NULL;
         return ;

    }
}

//处理搜索的书籍文字信息
void CKernel::dealSelectBooksTextInfoRq(char *buf, int nLen)
{
    qDebug()<<__func__;
    //首先拆包
    STRU_TCP_BOOKS_TEXT_INFO_RS* rs=(STRU_TCP_BOOKS_TEXT_INFO_RS*)buf;
    //转换编码格式
    QString bookNameUt;
    QString bookAreaUt;
    QString bookPriceUt;
    QString bookInfoUt;
    char *bookName = new char[_DEF_BOOK_NAME_SIZE];
    char *bookArea = new char[_DEF_BOOK_AREA_SIZE];
    char *bookPrice = new char[_DEF_BOOK_PRICE_SIZE];
    char *bookInfo = new char[_DEF_BOOK_INFO_SIZE];

    strcpy(bookName,rs->bookName);
    strcpy(bookArea,rs->bookArea);
    strcpy(bookPrice,rs->bookPrice);
    strcpy(bookInfo,rs->bookInfo);

    utf8_Gb2312_Change(bookNameUt,bookName,_DEF_BOOK_NAME_SIZE);
    utf8_Gb2312_Change(bookAreaUt,bookArea,_DEF_BOOK_AREA_SIZE);
    utf8_Gb2312_Change(bookPriceUt,bookPrice,_DEF_BOOK_PRICE_SIZE);
    utf8_Gb2312_Change(bookInfoUt,bookInfo,_DEF_BOOK_INFO_SIZE);


    //查看是哪一本书的信息
//    char id[5] = "";     //id大小标准为5
//    itoa(rs->id,id,10);
//    QString id_string = QString(id);
    char bookid[5]="";
    itoa(rs->bookid,bookid,10);   //将int型转换为字符串,10是10进制
    char isSchool[5]="";
    itoa(rs->isSchool,isSchool,10);
    QString bookidstring = QString(isSchool)+QString(bookid);
    if( 0 < m_mapSelectBooks.count(bookidstring))   //如果map中存在，则将信息存入到对应的书籍窗口中去
    {
        Books* bookWdg = m_mapSelectBooks[bookidstring];
        bookWdg->setTextInfo(bookNameUt,bookAreaUt,bookPriceUt,bookInfoUt,rs->QQ,rs->weChat);   //设置书籍文字信息
        bookWdg = NULL;
    }


    delete []bookName;
    bookName = NULL;
    delete []bookArea;
    bookArea = NULL;
    delete []bookPrice;
    bookPrice = NULL;
    delete []bookInfo;
    bookInfo = NULL;
}

//处理获取的本机上架的书籍的图片信息
void CKernel::dealMyBooksInfo(char *buf, int nLen)
{
    qDebug()<<__func__;
    //顺序是：type,bookid,isSchool,img,大小为4，5，5
        char bookid[5] = "";
        char isSchool[5] = "";
        memcpy(bookid,buf+4,5);
        memcpy(isSchool,buf+4+5,5);
        QString idb = QString(bookid);
        QString isSchoolstring = QString(isSchool);
        QString idString =  isSchoolstring + idb;


    if(0 < m_mapMyBooks.count(idString))
    {
        //如果map中已经有了就不保存，直接返回
        return;
    }
    else{
        MyBooksWdg* bookWdg = new MyBooksWdg;
        //绑定信号和槽函数
        connect(bookWdg,SIGNAL(SIG_deleteBookInfo(int,QString)),
                this,SLOT(slot_deleteTargetBooks(int,QString)));
           FILE* fp;
            char imgPath[_DEF_IMG_PATH_SIZE] = "";
            strcpy(imgPath,".//img//");     //处于debug模式时不能使用.//img//img//，能写入，但是icon读的时候会出错，目前原因不详。
            strcat(imgPath,idString.toStdString().c_str());
            strcat(imgPath,".jpg");  //拼好路径
            if(0 == fopen_s(&fp,imgPath,"rb")) //以二进制读打开，如果成功则说明已有该文件，则返回
            {
                strcpy(bookWdg->imgPath, imgPath);  //将路径信息复制过去
                bookWdg->setImgInfo();
             m_mapMyBooks[idString] = bookWdg;  //将窗口添加到map中

            }    //如果打不开，则写入
            else if(0 == fopen_s(&fp,imgPath,"wb"))   //以二进制写的方式打开
            {
                //如果打开成功，将图片信息写入
                int number = fwrite(buf + 14,nLen  - 14,1,fp);   //前面的是其他信息
                fclose(fp);  //关闭文件
                if(number > 0)
                {
                    strcpy(bookWdg->imgPath, imgPath);  //将路径信息复制过去
                      bookWdg->setImgInfo();
                 m_mapMyBooks[idString] = bookWdg;  //将窗口添加到map中
                }
            }

         int school = atoi(isSchool);
         m_pBooks->addMyBooksWdg(bookWdg,MY_BOOK);       //将窗口添加到secondhandbooks的对应窗口中
         bookWdg = NULL;
         return ;

    }
}

//处理获取的本机上架的书籍的文字信息
void CKernel::dealMyBooksTextInfo(char *buf, int nLen)
{
    qDebug()<<__func__;
    //首先拆包
    STRU_TCP_BOOKS_TEXT_INFO_RS* rs=(STRU_TCP_BOOKS_TEXT_INFO_RS*)buf;
    //转换编码格式
    QString bookNameUt;
    QString bookAreaUt;
    QString bookPriceUt;
    QString bookInfoUt;
    char *bookName = new char[_DEF_BOOK_NAME_SIZE];
    char *bookArea = new char[_DEF_BOOK_AREA_SIZE];
    char *bookPrice = new char[_DEF_BOOK_PRICE_SIZE];
    char *bookInfo = new char[_DEF_BOOK_INFO_SIZE];

    strcpy(bookName,rs->bookName);
    strcpy(bookArea,rs->bookArea);
    strcpy(bookPrice,rs->bookPrice);
    strcpy(bookInfo,rs->bookInfo);

    utf8_Gb2312_Change(bookNameUt,bookName,_DEF_BOOK_NAME_SIZE);
    utf8_Gb2312_Change(bookAreaUt,bookArea,_DEF_BOOK_AREA_SIZE);
    utf8_Gb2312_Change(bookPriceUt,bookPrice,_DEF_BOOK_PRICE_SIZE);
    utf8_Gb2312_Change(bookInfoUt,bookInfo,_DEF_BOOK_INFO_SIZE);


    //查看是哪一本书的信息
//    char id[5] = "";     //id大小标准为5
//    itoa(rs->id,id,10);
//    QString id_string = QString(id);
    char bookid[5]="";
    itoa(rs->bookid,bookid,10);   //将int型转换为字符串,10是10进制
    char isSchool[5]="";
    itoa(rs->isSchool,isSchool,10);
    QString bookidstring = QString(isSchool)+QString(bookid);
    if( 0 < m_mapMyBooks.count(bookidstring))   //如果map中存在，则将信息存入到对应的书籍窗口中去
    {
        MyBooksWdg* bookWdg = m_mapMyBooks[bookidstring];
        bookWdg->setTextInfo(rs->isSchool ,bookNameUt,bookAreaUt,bookPriceUt,bookInfoUt);   //设置书籍文字信息
        bookWdg = NULL;
    }


    delete []bookName;
    bookName = NULL;
    delete []bookArea;
    bookArea = NULL;
    delete []bookPrice;
    bookPrice = NULL;
    delete []bookInfo;
    bookInfo = NULL;
}

//处理下架包的回复信息
void CKernel::dealDeleteBookRs(char *buf, int nLen)
{

    qDebug()<<__func__;
    //TODO:调试下架书籍！！！！
    STRU_TCP_DELETE_BOOKS_INFO_RS * rs = (STRU_TCP_DELETE_BOOKS_INFO_RS*)buf;

    //转码：
    QString bookName;
    char *bookNameGb = new char[_DEF_BOOK_NAME_SIZE];
    strcpy(bookNameGb,rs->bookName);
    utf8_Gb2312_Change(bookName,bookNameGb,_DEF_BOOK_NAME_SIZE);

    //如果下架失败
    if(DELETE_DEFAULT == rs->isDeleteSuccess)
    {
        QString info = QString("《%1》下架失败!").arg(bookName);
        QMessageBox::information(NULL,"提示",info);
        return ;
    }
    QString Path = ".//img//";
    QString jpg = ".jpg";
    char isSchool[5];
    char id_[5];
    itoa(rs->isSchool,isSchool,10);
    itoa(rs->bookid,id_,10);
    QString isSchoolUt = QString(isSchool);
    QString id =QString(id_);
    QString bookPath = Path + isSchoolUt + id +jpg;  //因为qt中图片地址主体为isSchool +bookid
    QString map_id  = isSchoolUt +id;    //map中的主键是isSchool+bookid
    if(0 < m_mapMyBooks.count(map_id))
    {
        auto ite = m_mapMyBooks.find(map_id);
        MyBooksWdg* mybook = *ite;
        mybook->showNormal();
        delete mybook;
        mybook = NULL;
        m_mapMyBooks.erase(ite);
    }
}

//处理接收到的更换头像的回复
void CKernel::dealAlterIconRs(char *buf, int nLen)
{
    STRU_TCP_ALTER_ICON_RS* rs = (STRU_TCP_ALTER_ICON_RS*)buf;
    if( true == rs->isSuccess)
    {
        QMessageBox::information(NULL,"提示","头像更换成功！");
    }
    else
    {
        QMessageBox::information(NULL,"提示","头像更换失败，请重试！");

    }
}

//处理接收到的视频信息
void CKernel::dealVideoTran(long lSendIp, char *buf, int nLen)
{
    qDebug()<<__func__;
    qDebug()<<nLen;


//        STRU_VIDEO_TRAN* Rq = (STRU_VIDEO_TRAN*)buf;
//    int a=0;
//qDebug()<<"ffffffff";
//   QByteArray buff=QByteArray(buf+16,Rq->size);
//   QBuffer buffer(&buff);


//    QImageReader reader(&buffer,"JPEG");
//    QImage image = reader.read();
//Q_EMIT SIG_painWindow(image);

if(nLen > 1000)
{
       QByteArray buff=QByteArray(buf+8,nLen-8); //前8为分别放的是包类型type和目标主机id
       QBuffer buffer(&buff);


        QImageReader reader(&buffer,"JPEG");
        QImage image = reader.read();
//        Other_pVideoForm->SLOT_videoDisplay(image);
    Q_EMIT SIG_painWindow(image);
}

}
//处理接收到的音频文件

//void CKernel::dealVoidTran(long lSendIp, char *buf, int nLen)
//{
    //1：拆包：
//    STRU_VOID_TRAN* rq = (STRU_VOID_TRAN*)buf;

//    m_pVoid->readyReadSlot(rq->voidData,rq->lens);
//}

//处理下线请求
//void CKernel::dealOfflineRq(long lSendIp, char *buf, int nLen)
//{
//    qDebug() << __func__;

//    //拆包：
//    STRU_OFFLINE* rq=(STRU_OFFLINE*)buf;


//    //1：从好友列表里面删除好友
//    if(p_mainWnd)
//    p_mainWnd->deleteFriend(INet::GetIpString(rq->m_id).c_str());
//  qDebug()<<"sssss";
//    //2:回收此好友的聊天窗口
//    auto ite=m_mapIpToChatdlg.find(rq->m_id);   //用auto定义迭代器是自动匹配类型，不用定义
//    if(ite != m_mapIpToChatdlg.end()){
//        ChatDialog* chat = m_mapIpToChatdlg[rq->m_id];
//        chat->hide();   //回收窗口的步骤就是先隐藏
//        delete chat;
//        chat=NULL;
//        m_mapIpToChatdlg.erase(ite);
//        qDebug()<<"lllll";
//    }
//}

//处理文件信息请求
void CKernel::dealFileRq(long lSendIp, char *buf, int nLen)
{
    //拆包，获取文件信息
    STRU_FILE_INFO_RQ* FileInfo=(STRU_FILE_INFO_RQ*)buf;
    QString name=QString(FileInfo->name);  //获取发送方主机名
    QString FileName=QString(FileInfo->szFileName);  //获取文件名
    QString FileSize=QString::number((FileInfo->szFileSize)/1000);  //获取文件大小,单位KB
    QString message1=QString("    给您发来文件请求，是否接收该文件?文件名：");
    QString message2=QString("      文件大小：");
    QString message3=QString(" KB");
    QString accFile=name+message1+FileName+message2+FileSize+message3;
    int clicked=QMessageBox::information(NULL,"文件请求",accFile,QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);//返回按了哪个按钮
    if(QMessageBox::Yes == clicked)
    {
        //首先确定保存路径：
        QString path = QFileDialog::getSaveFileName(NULL,"Save File","/");
        qDebug()<<path;
        //发送文件信息请求的回复
        STRU_FILE_INFO_RS rs;
       rs.ISendIp=FileInfo->m_id;   //将目标主机的ip地址告诉服务器
       rs.m_id = m_id;
         gethostname(rs.name,sizeof(rs.name)); //获取主机名

         memcpy(rs.szFileID,FileInfo->szFileID,sizeof(FileInfo->szFileID));//rs.szFileID = FileInfo->szFileID;//文件唯一标识
        //发送文件信息回复
         m_pMediator->SendData(lSendIp,(char*)&rs,sizeof(rs));
    }
    else{
        qDebug()<<"拒绝";
        //发送文件信息请求拒绝的回复
        STRU_FILE_INFO_RS rs;
        rs.ISendIp=FileInfo->m_id;   //将目标主机的ip地址告诉服务器
        rs.m_id=m_id;
        gethostname(rs.name,sizeof(rs.name)); //获取主机名
        rs.nRes=_file_refuse;
        memcpy(rs.szFileID,FileInfo->szFileID,sizeof(FileInfo->szFileID));//rs.szFileID = FileInfo->szFileID;//文件唯一标识
        //发送文件信息回复
         m_pMediator->SendData(lSendIp,(char*)&rs,sizeof(rs));




         //初始化Tcp网络




    }
}

//处理文件信息回复
void CKernel::dealFileRs(long lSendIp, char *buf, int nLen)
{
    qDebug()<<__func__;
    //拆包
    STRU_FILE_INFO_RS* FileAnswer = (STRU_FILE_INFO_RS*)buf;
    qDebug()<<FileAnswer->nRes;
    if(_file_refuse == FileAnswer->nRes)
    {
        qDebug()<<"拒绝"<<endl;
        QString info=QString("  拒绝了您的文件传输请求!");
        QString name=QString(FileAnswer->name);
        QString infor=name+info;
        QMessageBox::information(NULL,"提示",infor);
        return ;
    }
    else if(_file_accept == FileAnswer->nRes)
    {
        //开始发送文件块儿(用TCP)
             QMessageBox::information(NULL,"提示","文件已开始传输");
            strcpy(MyFileInfo->szFileID,FileAnswer->szFileID);    //需要在此时将文件的唯一标识填入到文件块包里去。

            if(!m_pTcpMediator->OpenNet(lSendIp))   //与tcp服务器进行连接
            {
                QMessageBox::information(NULL,"提示","网络连接失败！");
                return;
            }

            _beginthreadex(NULL, 0, &SendFileBlockThread, (void*)this, 0, NULL);


    }
}

//发送文件块线程
unsigned CKernel::SendFileBlockThread(void *IpVoid)
{
    CKernel* pThis= (CKernel*)IpVoid;
//    QString path=QString(pThis->crq->szFilePath);
    FILE* pFile=fopen(pThis->MyFileInfo->szFilePath,"r");
        //发送文件块
        STRU_FILE_BLOCK_RQ blockRq;
//        blockRq.ISendIp=inet_addr((ip.toStdString().c_str()));   //将目标主机的ip地址告诉服务器
        long long nPos = 0;  //表示文件读到哪个位置了
        int nReadLen = 0;

        while(true){
            //读文件发送
            /*参数：
             *1：缓冲区 2：一次读多少 3：读多少次  4：文件指针
            */
            //：返回值：成功读多少次，用于反应读出多少字节
            nReadLen = fread(blockRq.szFileContent,1,_DEF_FILE_CONTENT_SIZE,pFile);
            blockRq.nBlockSize = nReadLen;  //文件块大小


            //发送
            pThis->m_pTcpMediator->SendData(0,(char*)&blockRq ,sizeof(blockRq));

            //触发条件退出
            if(nPos >= pThis->MyFileInfo->szFileSize || nReadLen < _DEF_FILE_CONTENT_SIZE)
            {
                fclose(pFile);
                break;
            }
            Sleep(1);

        }
    return 1;
}

//处理文件块儿
void CKernel::dealFileBlockRq(char *buf)
{
    //TODO:存储文件

    //上面接收文件信息请求后要初始化Tcp网络
    qDebug()<<__func__;
}

int CKernel::GetInternetIP(string &Inernet_ip)
{

        Inernet_ip.resize(32);
        TCHAR szTempPath[_MAX_PATH] = {0}, szTempFile[MAX_PATH] = {0};
        std::string buffer;
        GetTempPath(MAX_PATH, szTempPath);
        UINT nResult = GetTempFileName(szTempPath, /*_T*/("~ex"), 0, szTempFile);

        /* #pragma comment(lib, “Urlmon.lib”)的目的是引入 URLDownloadToFile 这个函数 */

        int ret=URLDownloadToFile(NULL,/*_T*/("http://www.ip138.com/ip2city.asp"),szTempFile,BINDF_GETNEWESTVERSION,NULL);
        if (ret == S_FALSE)
            return 0;
        FILE *fp;
        if (fopen_s(&fp,szTempFile,("rb"))!=0){
            return 0;
        }
        fseek(fp,0,SEEK_END);//得到文件大小
        int ilength=ftell(fp);
        fseek(fp,0,SEEK_SET);
        if(ilength>0)
        {
            buffer.resize(ilength);
            fread(&buffer[0],sizeof(TCHAR),ilength,fp);
            fclose(fp);
            DeleteFile(/*_T*/("ip.ini"));

             char* str_ip = strstr(&buffer[0], "[");
             if (str_ip !=NULL)
             {
                 sscanf(str_ip+1, "%[^]]", &Inernet_ip[0], 32);
             }
            return 1;
        }
        else
        {
            fclose(fp);
            return 0;
        }

}

//因为路由器中端口与私网ip的映射是动态的，所以需要一个线程每隔一段时间就向服务器发送一条消息，告诉服务器自己的地址信息的变化！
unsigned CKernel::LiveThread(void *IpVoid)
{
    CKernel* pThis = (CKernel*)IpVoid;
    //因为此包内并无实际内容，所以直接用其他结构体代替一下，然后将type改一下即可
    STRU_OFFLINE rq;
    rq.type=_DEF_PROTOCOL_LIVE;
    rq.m_id=pThis->m_id;
    while(!pThis->LiveStop)
    {
        qDebug()<<"此线程id为"<<GetCurrentThreadId();
        pThis->m_pMediator->SendData(0,(char*)&rq,sizeof(rq));
        qDebug()<<"aaaaaaaaaaaaaaaa";
        Sleep(10000);  //经过实测，间隔时间应该设置在10s左右
    }
}

//处理接收到的音频文件
CKernel::dealVoidInfo(long lSendIp, char *buf, int nLen)
{
    STRU_VOID_TRAN *rq=(STRU_VOID_TRAN*)buf;
    m_pMySound->listenVoid(rq->voidData,rq->lens);
}

//============TCP=========================================================================================
//初始化函数指针数组

void CKernel::InitTcpDealFun()
{
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_REGISTER_RS) = &CKernel::dealRegisterRs;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_LOGIN_RS) = &CKernel::dealLoginRs;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_FRIEND_INFO) = &CKernel::dealFriendInfoRq;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_OFFLINE_RQ) = &CKernel::dealOfflineRq;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_ADD_FRIEND_RQ) = &CKernel::dealaddFriendRq;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_ADD_FRIEND_RS) = &CKernel::dealaddFriendRs;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_VIDEO_RQ) = &CKernel::dealVideoRq;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_VIDEO_RS) = &CKernel::dealVideoRs;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_VOID_RQ) = &CKernel::dealVoidRq;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_VOID_RS) = &CKernel::dealVoidRs;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_CLOSE_VOID_RQ) = &CKernel::dealCloseVoid;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_ADD_BOOKS_RS ) = &CKernel::dealAddBookRs;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_BOOKS_IMG_INFO_RS ) = &CKernel::dealBooksImgInfoRq;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_BOOKS_TEXT_INFO_RS ) = &CKernel::dealBooksTextInfoRq;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_SELECT_BOOKS_IMG_INFO_RS ) = &CKernel::dealSelectBooksImgInfoRq;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_SELECT_BOOKS_TEXT_INFO_RS ) = &CKernel::dealSelectBooksTextInfoRq;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_IMG_INFO_RS ) = &CKernel::dealMyBooksInfo;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_SELECT_MY_BOOKS_TEXT_INFO_RS ) = &CKernel::dealMyBooksTextInfo;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_DELETE_BOOKS_RS ) = &CKernel::dealDeleteBookRs;
    TCP_TYPE_PFUN(_DEF_PROTOCOL_TCP_ALTER_ICON_RS ) = &CKernel::dealAlterIconRs;





}

//utf8和gb2312编码的互相转换
void CKernel::utf8_Gb2312_Change(QString &utf8, char *&gb2312, int len)   //此处utf8用引用，因为是即是输入也是输出参数
{                             //注意：因为gb2312也是引用，所以传进来的指针不能是const指针，例如栈区的数组指针！

    //new一个转码对象
    QTextCodec* gb2312code = QTextCodec::codecForName("gb2312");
    if(!gb2312) //如果gb2312是空，则表示是QString转gb2312
    {
            //先给指针分配空间
               gb2312=new char[100];
            //从utf8转到gb2312
            QByteArray ba = gb2312code -> fromUnicode(utf8);
            //往目标空间中拷贝转码后的数据
            strcpy_s(gb2312,ba.size()+1/* gb2312Len */,ba.data());   //char*的结束位并没有算到QByteArray的数据长度中去，所以如果使用ba.size()要加1
        }
    else{  //否则则是gb2312转QString
        //gb2312转utf8,QString就是utf8编码方式的，gb2312使用char*存储
            //从gb2312转到utf8返回值是QString
            utf8 = gb2312code->toUnicode(gb2312);



    }
}

//处理注册回复
void CKernel::dealRegisterRs(char *buf, int nLen)
{

    qDebug()<<__func__;

    //1：拆包
    STRU_TCP_REGISTER_RS* rs = (STRU_TCP_REGISTER_RS*)buf;

    //根据注册结果提示用户
    switch(rs->result){
    case register_sucess:
        QMessageBox::about(NULL, "提示", "注册成功");
        break;
    case user_is_exist:
           QMessageBox::about(NULL, "提示", "注册失败，用户已经存在");
        break;
    case parameter_error:
           QMessageBox::about(NULL, "提示", "注册失败，输入内容错误");
        break;
    default:
        break;
    }

}

//处理登陆回复
void CKernel::dealLoginRs(char *buf, int nLen)
{
    qDebug()<<__func__;
    //1:拆包：
    STRU_TCP_LOGIN_RS* rs = (STRU_TCP_LOGIN_RS*)buf;
    if( user_not_exist == rs->result || password_error == rs->result )
    {
        QMessageBox::about(m_ploginDlg,"提示","用户不存在或密码输入错误！");
        return ;
    }
    else if(login_success == rs->result){    //登录成功
        m_id = rs->userId;

        m_pMaindialog->showNormal();
        m_ploginDlg->hide();

        //如果登陆成功的话则告诉udp服务器自己上线了
          STRU_ONLINE rq;
    //        gethostname(rq.name,sizeof(rq.name)); //获取主机名
            rq.m_id = m_id; //将自己的序号填入，告诉udp服务器自己是谁
            m_pMediator->SendData(0,(char*)&rq, sizeof(rq));
            //TODO:并且开启生命线程

      _beginthreadex(NULL, 0, &LiveThread, (void*)this, 0, NULL);
    }

}

//处理好友信息请求
void CKernel::dealFriendInfoRq(char *buf, int nLen)
{
    qDebug()<<__func__;
    //1:拆包
    STRU_TCP_FRIEND_INFO* rq = (STRU_TCP_FRIEND_INFO*)buf;
    //将中文转码
    QString name;
    QString feeling;
    char *buf_name=new char[100];
    char *buf_feeling = new char[100];
    strcpy(buf_name,rq->name);
    strcpy(buf_feeling,rq->feeling);
    utf8_Gb2312_Change(name,buf_name,0);
    utf8_Gb2312_Change(feeling,buf_feeling,0);


    //首先判断这个包里的信息是自己的还是朋友的
    if( m_id != rq->userId && 0 == m_mapIdToUserItem.count(rq->userId) )  //如果是朋友的信息且不在列表中
    {
        //设置信息到useritem中
        useritem* user = new useritem;
        user->addFriendInfo(rq->iconId,rq->status,name,feeling,rq->userId);
        qDebug()<<feeling;
        //将其添加到map中管理
        m_mapIdToUserItem[rq->userId] = user;
        //将其添加到主窗口中
        m_pMaindialog->addFriendWidget(user);

    //绑定聊天窗口的信号与槽函数
        connect(user->m_chatDialog,SIGNAL(SIG_sendMsg(int,QString)),
                 this,SLOT(slot_sendMsg(int,QString)));

            connect(user->m_chatDialog,SIGNAL(SIG_videoRq(long)),
                     this,SLOT(slot_videoRq(long)));

            connect(user->m_chatDialog,SIGNAL(SIG_SoundRq(long)),
                     this,SLOT(slot_SoundRq(long)));

            connect(user->m_chatDialog,SIGNAL(SIG_CloseVoid(long)),
                     this,SLOT(slot_closeVoid(long)));

    }
    else if(1 == m_mapIdToUserItem.count(rq->userId))  //如果已经在列表中了
    {
//       m_mapIdToUserItem[rq->userId]->addFriendInfo(rq->iconId,rq->status,name,feeling,rq->userId);  //更新信息
        useritem* user = m_mapIdToUserItem[rq->userId];
        //将控件从原窗口移出，即将父类设为NULL
        m_pMaindialog->removeWidgt(user);
        //更新user信息
        m_mapIdToUserItem[rq->userId]->addFriendInfo(rq->iconId,rq->status,name,feeling,rq->userId);  //更新信息
        //从新添加进窗口
        m_pMaindialog->addFriendWidget(user);

    }
    else{   //如果是自己的信息
        m_pMaindialog->addMyInfo(m_id, name, feeling);
        m_name=name;
    }

    delete []buf_name;
    buf_name = NULL;
    delete []buf_feeling;
    buf_feeling = NULL;

}

//处理下线请求：TCP
void CKernel::dealOfflineRq(char *buf, int nLen)
{
    qDebug()<<__func__;
    STRU_TCP_OFFLINE_RQ* rq =(STRU_TCP_OFFLINE_RQ*)buf;
    //判断下线者是否在列表中
   if(0 < m_mapIdToUserItem.count(rq->userId))
   {
       //将其头像置为灰色
      useritem *user =  m_mapIdToUserItem[rq->userId];
      user->offLine();
      //将控件从原窗口移出，即将父类设为NULL
      m_pMaindialog->removeWidgt(user);
      //从新添加进窗口
      m_pMaindialog->addFriendWidget(user);

   }
}

//处理好友添加请求
void CKernel::dealaddFriendRq(char *buf, int nLen)
{
    STRU_TCP_ADD_FRIEND_RQ* rq = (STRU_TCP_ADD_FRIEND_RQ*)buf;
    QString name=rq->userName;
    QString info = QString("【%1】请求添加您为好友").arg(name);
    qDebug()<<info;
    int result = QMessageBox::information(m_pMaindialog ,"  收到新的好友请求 ",info,QMessageBox::Yes | QMessageBox::No );
     STRU_TCP_ADD_FRIEND_RS rs;
     strcpy(rs.friendName,m_name.toStdString().c_str());
     rs.friendId=rq->userId;
     rs.userId = m_id;
     if(QMessageBox::No == result)
    {
        //TODO:发送添加好友回复，拒绝
      rs.result=user_refuse;
      qDebug()<<"no";
        m_pTcpMediator->SendData(0,(char*)&rs,sizeof(rs));
    }
    else{
        //TODO:发送添加好友回复：同意
        rs.result=add_success;
        qDebug()<<"yes";
          m_pTcpMediator->SendData(0,(char*)&rs,sizeof(rs));
     }
}

//处理添加好友回复
void CKernel::dealaddFriendRs(char *buf, int nLen)
{
    STRU_TCP_ADD_FRIEND_RS* rs = (STRU_TCP_ADD_FRIEND_RS*)buf;
    //查看是否添加成功
    if(add_success == rs->result)
    {
        QMessageBox::information(m_pMaindialog,"提示",QString("您已成功添加【%1】为好友啦").arg(rs->friendName));
        //服务器发送好友信息
    }
    else{
        QMessageBox::information(m_pMaindialog,"提示",QString("【%1】拒绝了您的好友请求").arg(rs->friendName));

    }
}
