#include "ckernel.h"
#include<stdio.h>
CKernel::CKernel(QObject *parent) : QObject(parent),Inernet_ip(""),LiveStop(false)
{
    qDebug()<<__func__;
    //1：初始化主窗口

    p_mainWnd=new FeiQDialog;
    p_mainWnd->showNormal();

    //绑定发送双击某一行的信号和槽函数
    connect(p_mainWnd,SIGNAL(SIG_userClicked(QString)),
            this,SLOT(slot_userClicked(QString)));

    //绑定关闭窗口的信号和槽函数
    connect(p_mainWnd,SIGNAL(SIG_closeWnd()),
             this,SLOT(slot_closeWnd()));




    //2：初始化中介者类指针
    m_pMediator = new UdpMediator;  //Udp的中介者类
    m_pTcpMediator = new TcpMediator;   //Tcp的中介者类
    //3:打开网络
    if(!m_pMediator -> OpenNet(0)){
        QMessageBox::about(p_mainWnd,"提示","打开网络失败");
        exit(0);
    }


    //在接收信号的类里面，发送信号的对象new出来以后
    //绑定接收数据的信号和槽函数
    connect(m_pMediator,SIGNAL(SIG_ReadData(long,char*,int)),
            this,SLOT(slot_ReadData(long,char*,int)));

    //接收文件
    connect(m_pTcpMediator,SIGNAL(SIG_ReadData(long,char*,int)),
            this,SLOT(slot_ReadData(long,char*,int)));


//    //发送一个广播上线请求
//    STRU_ONLINE rq;
//    gethostname(rq.name,sizeof(rq.name)); //获取主机名
//    qDebug()<<rq.name;
//    m_pMediator->SendData(INADDR_BROADCAST,(char*)&rq,sizeof(rq));

    //告诉服务器自己上线了
        STRU_ONLINE rq;
        gethostname(rq.name,sizeof(rq.name)); //获取主机名
        qDebug()<<rq.name;
//        GetInternetIP(Inernet_ip);  //获取本机上网所使用的公网ip
//    //    char buf[_DEF_MAX_SIZE];
//        Inernet_Ip = inet_addr(Inernet_ip.c_str());
//    //    string a="183.228.237.239";
//    //        Inernet_Ip = inet_addr(a.c_str());
//        rq.ISendIp = Inernet_Ip ;  //上线请求包中的ip等于自己的公网ip   （公网ip暂时无法获取成功，作废）
        m_pMediator->SendData( inet_addr(ServerIP) ,(char*)&rq,sizeof(rq));
        //开启生命线程，不停发送包让服务器不断修正地址信息
        _beginthreadex(NULL, 0, &LiveThread, (void*)this, 0, NULL);

   // m_pMediator->SendData(INADDR_BROADCAST, "hello world", strlen("hello world")+1);

}

CKernel::~CKernel()
{
    qDebug()<<__func__;  //打印输出：在那个函数中
    //回收资源
    if(p_mainWnd){
        p_mainWnd->hide();
        delete p_mainWnd;
        p_mainWnd = NULL;
    }
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
        dealOnlineRq(lSendIp, buf, nLen);
        break;
    case _DEF_PROTOCOL_ONLINE_RS:
          dealOnlineRs(lSendIp, buf, nLen);
        break;
    case _DEF_PROTOCOL_CHAT_RQ:
          dealChatRq(lSendIp, buf, nLen);
        break;
    case _DEF_PROTOCOL_OFFLINE_RQ:
          dealOfflineRq(lSendIp, buf, nLen);
        break;
    case _DEF_PROTOCOL_FILE_INFO_RQ:
          dealFileRq(lSendIp, buf, nLen);   //处理文件信息请求
    case _DEF_PROTOCOL_FILE_INFO_RS:
          dealFileRs(lSendIp, buf, nLen);    //处理文件信息回复
    case _DEF_PROTOCOL_FILE_BLOCK_RQ:
        dealFileBlockRq(buf);

          default:
          break;
    }

    //3:回收空间
    delete[] buf;

}
//处理双击某一行的信号
void CKernel::slot_userClicked(QString ip)
{
    qDebug()<< __func__<<ip;
    //找到该人对应的聊天窗口并显示
    long lIp=inet_addr(ip.toStdString().c_str());
    if(m_mapIpToChatdlg.find(lIp) != m_mapIpToChatdlg.end())
    {
        ChatDialog* chat=m_mapIpToChatdlg[lIp];
        chat->showNormal();
    }
}

//处理聊天内容
void CKernel::slot_sendMsg(QString ip, QString content)
{
    qDebug()<<__func__;
    //发送聊天请求给对端
    STRU_CHAT rq;
    rq.MyIp=MyIp;  //将自己的编号填入
    rq.ISendIp=inet_addr((ip.toStdString().c_str()));    //将目标主机ip地址存入包中
    strcpy(rq.content, content.toStdString().c_str());
    m_pMediator->SendData(inet_addr((ip.toStdString().c_str())),(char*)&rq,sizeof(rq));
}


//关闭窗口
void CKernel::slot_closeWnd()
{
    qDebug()<<__func__;
    //结束生命线程
    LiveStop=true;
    //1：发送下线请求
    STRU_OFFLINE rq;
    rq.MyIp=MyIp;
    m_pMediator->SendData(INADDR_BROADCAST, (char*)&rq, sizeof(rq));

    //回收资源
    if(p_mainWnd){
        p_mainWnd->hide();
        delete p_mainWnd;
        p_mainWnd = NULL;
    }

    if(m_pMediator){
        m_pMediator->CloseNet();
        delete m_pMediator;
        m_pMediator = NULL;
    }

    for(auto ite = m_mapIpToChatdlg.begin(); ite!=m_mapIpToChatdlg.end() ;)
    {
        ChatDialog* chat =*ite;
        chat->hide();
        delete chat;
        chat=NULL;
        ite = m_mapIpToChatdlg.erase(ite);
    }

    //3:结束进程
        exit(0);
}

//处理发送文件信号的槽函数
void CKernel::slot_sendFile(QString ip, QFileInfo FileInfo)
{
    qDebug()<<__func__;
    STRU_FILE_INFO_RQ rq;

    crq=new STRU_FILE_INFO_RQ;  //用于发送文件块儿的时候


    //配置协议头信息
    //制作文件唯一标识
    std::string fName=FileInfo.fileName().toStdString();//文件名  QString转换成string

   strncpy_s(rq.szFileName,sizeof(rq.szFileName),fName.c_str(),_MAX_FILE_PATH);//将const char*复制到char[];
//cout<<rq.szFileName<<"ssssss"<<endl;
    QString fTime=QTime::currentTime().toString("hh:mm:ss");   //时间

    std::string fMask=fName+fTime.toStdString();//生成文件唯一标识

    //填入文件路径信息：
     std::string fPath=FileInfo.filePath().toStdString();//文件名  QString转换成string
        strncpy_s(rq.szFilePath,sizeof(rq.szFilePath),fPath.c_str(),_MAX_FILE_PATH);//将const char*复制到char[];


//    cout<<fMask<<"sss"<<endl;
//    rq.szFileID=fMask.c_str();
        //填入文件唯一标识
    strncpy_s(rq.szFileID,sizeof(rq.szFileID),fMask.c_str(),_MAX_FILE_PATH);
//    cout<<rq.szFileID<<endl;
     gethostname(rq.name,sizeof(rq.name)); //获取主机名

     rq.szFileSize=FileInfo.size();

     memcpy(crq,&rq,sizeof(rq));   //将rq的值给crq复制一份儿。

     rq.ISendIp=inet_addr((ip.toStdString().c_str()));   //将目标主机的ip地址存入包中告诉服务器

    m_pMediator->SendData(inet_addr((ip.toStdString().c_str())),(char*)&rq,sizeof(rq));

}



//处理上线请求
void CKernel::dealOnlineRq(long lSendIp, char *buf, int nLen)
{
    qDebug()<< __func__;
    //1:拆包
    STRU_ONLINE* rq = (STRU_ONLINE*)buf;

    //2：添加到好友列表中
    p_mainWnd->addFriend(QString(INet::GetIpString(rq->MyIp).c_str()), rq->name);
    //3:创建一个与该人聊天的窗口（隐藏）
     ChatDialog* chat = new ChatDialog;
     //设置窗口信息
     chat->setInfo(INet::GetIpString(rq->MyIp).c_str());


     //绑定发送聊天数据的信号和槽函数
     connect(chat,SIGNAL(SIG_sendMsg(QString,QString)),
             this,SLOT(slot_sendMsg(QString,QString)));

     //绑定发送文件的信号和槽函数
     connect(chat,SIGNAL(SIG_sendFile(QString,QFileInfo)),
              this,SLOT(slot_sendFile(QString,QFileInfo)));

     //4：把聊天窗口放到map中管理
     m_mapIpToChatdlg[rq->MyIp]=chat;


     //由于服务器是通过内网穿透才能被访问的，所以服务器获得的ip地址都是一个，现在在上线请求包里添加本机的公网ip地址+主机名来判断该上线请求包是不是本地主机发出的。
     //因为不同的主机上网所用的公网ip也许是同一个，所以需要增加本地主机名作为判断依据；

    //5：判断是不是自己，如果是自己就结束   （暂时直接通过主机名来判断是不是自己）
     int i=0;
     //获取主机名字
     char nameBuf[100]="";
     gethostname(nameBuf, 100);

        if(! strcmp(nameBuf,rq->name ) )
        {
            //如果是自己则将服务器分配的本地主机的编号填入
            MyIp=rq->MyIp;
            return ;
        }
        //双重判断
        //首先获取本机ip列表，再根据本机ip列表判断

        //2:根据主机名获取ip地址列表
//        struct hostent *remoteHost = gethostbyname(nameBuf);
//        while(remoteHost->h_addr_list[i] != 0){

//            if( *(u_long *) remoteHost->h_addr_list[i] == rq->ISendIp)
//                return;
//            i++;

//        }

    //6：不是自己，发送上线回复
    STRU_ONLINE rs;
    rs.MyIp=MyIp;    //将自己的编号填入
    rs.ISendIp = rq->MyIp; //将目标主机的编号填入
    rs.type = _DEF_PROTOCOL_ONLINE_RS;
    gethostname(rs.name,sizeof(rs.name));
    m_pMediator->SendData(rq->MyIp,(char*)&rs,sizeof(rs));
}
//处理上线回复
void CKernel::dealOnlineRs(long lSendIp, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1:拆包
    STRU_ONLINE* rq=(STRU_ONLINE*)buf;

    //2：添加到好友列表中
    p_mainWnd->addFriend(QString(INet::GetIpString(rq->MyIp).c_str()),rq->name);

    //3：创建一个与改任的聊天窗口（但是不显示）
    ChatDialog* chat = new ChatDialog;
    //绑定发送聊天数据的信号和槽函数
    connect(chat,SIGNAL(SIG_sendMsg(QString,QString)),
            this,SLOT(slot_sendMsg(QString,QString)));

    //绑定发送文件的信号和槽函数
    connect(chat,SIGNAL(SIG_sendFile(QString,QFileInfo)),
             this,SLOT(slot_sendFile(QString,QFileInfo)));

    //设置窗口信息
    chat->setInfo(QString(INet::GetIpString(rq->MyIp).c_str()));

    //4：把聊天窗口放到map中管理
    m_mapIpToChatdlg[rq->MyIp]=chat;
}

//处理聊天请求
void CKernel::dealChatRq(long lSendIp, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1:拆包
    STRU_CHAT* rq=(STRU_CHAT*)buf;

    //2：判断聊天窗口是否存在
    if(m_mapIpToChatdlg.count(rq->MyIp)>0){   //包里的MyIp是发送者的
        //3：取出聊天窗口，设置聊天内容到窗口上:
        ChatDialog* chat = m_mapIpToChatdlg[rq->MyIp];
        chat->setContent(INet::GetIpString(rq->MyIp).c_str(),rq->content);
       //4:显示出聊天窗口
      chat->showNormal();
    }
}

//处理下线请求
void CKernel::dealOfflineRq(long lSendIp, char *buf, int nLen)
{
    qDebug() << __func__;

    //拆包：
    STRU_OFFLINE* rq=(STRU_OFFLINE*)buf;


    //1：从好友列表里面删除好友
    if(p_mainWnd)
    p_mainWnd->deleteFriend(INet::GetIpString(rq->MyIp).c_str());
  qDebug()<<"sssss";
    //2:回收此好友的聊天窗口
    auto ite=m_mapIpToChatdlg.find(rq->MyIp);   //用auto定义迭代器是自动匹配类型，不用定义
    if(ite != m_mapIpToChatdlg.end()){
        ChatDialog* chat = m_mapIpToChatdlg[rq->MyIp];
        chat->hide();   //回收窗口的步骤就是先隐藏
        delete chat;
        chat=NULL;
        m_mapIpToChatdlg.erase(ite);
        qDebug()<<"lllll";
    }
}

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
//        rs.ISendIp=inet_addr((lSendIp.toStdString().c_str()));   //将目标主机的ip地址告诉服务器
         gethostname(rs.name,sizeof(rs.name)); //获取主机名

         memcpy(rs.szFileID,FileInfo->szFileID,sizeof(FileInfo->szFileID));//rs.szFileID = FileInfo->szFileID;//文件唯一标识
        //发送文件信息回复
         m_pMediator->SendData(lSendIp,(char*)&rs,sizeof(rs));
    }
    else{
        //发送文件信息请求拒绝的回复
        STRU_FILE_INFO_RS rs;
//        rs.ISendIp=inet_addr((lSendIp.toStdString().c_str()));   //将目标主机的ip地址告诉服务器
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
    //拆包
    STRU_FILE_INFO_RS* FileAnswer = (STRU_FILE_INFO_RS*)buf;
    if(_file_refuse == FileAnswer->nRes)
    {
        QString info=QString("  拒绝了您的文件传输请求!");
        QString name=QString(FileAnswer->name);
        QString infor=name+info;
        QMessageBox::information(NULL,"提示",infor);
        return ;
    }
    else
    {
        //开始发送文件块儿(用TCP)

            strcpy(crq->szFileID,FileAnswer->szFileID);    //需要在此时将文件的唯一标识填入到文件块包里去。

            if(!m_pTcpMediator->OpenNet(lSendIp))
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
    FILE* pFile=fopen(pThis->crq->szFilePath,"r");
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
            if(nPos >= pThis->crq->szFileSize || nReadLen < _DEF_FILE_CONTENT_SIZE)
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
    rq.MyIp=pThis->MyIp;
    while(!pThis->LiveStop)
    {
        pThis->m_pMediator->SendData(0,(char*)&rq,sizeof(rq));
        Sleep(10000);  //经过实测，间隔时间应该设置在10s左右
    }
}
