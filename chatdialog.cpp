#include "chatdialog.h"
#include "ui_chatdialog.h"
#include"ckernel.h"
#include<QTime>
#include"form.h"




ChatDialog::ChatDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);

}

ChatDialog::~ChatDialog()
{
    delete ui;
}

//把接收到的聊天信息显示到窗口上
void ChatDialog::setContent(QString name, QString content)
{
    //显示到浏览器窗口上，格式【ip】时间...
    ui->tb_chat->append(QString("【%1】 %2").arg(name).arg(QTime::currentTime().toString("hh:mm:ss")));  //append()在text edit的最后添加一个新的段落。
    ui->tb_chat->append(content);   //insertPlainText()
}
//设置窗口信息
//void ChatDialog::setInfo(QString ip)
//{
//    qDebug()<<__func__;
//    //1:保存IP地址
//    qDebug()<<ip;
//    m_ip=ip;//2：设置窗口的title
//    setWindowTitle(QString("与【%1】的聊天窗口").arg(m_ip));
//}

//打开需要发送的文件
QFileInfo ChatDialog::OpenFile()
{
    //1:获取文件信息
    //文件浏览窗口
//    char FILE_PATH[1024] = "";
//    OPENFILENAME file = {0}; //结构体给初始值
//    file.lStructSize = sizeof(file); //结构体大小
//   // file.lpstrFilter="Win32 executable files\0*.exe\0\0";  //过滤器
//    file.lpstrFile = (LPWSTR)FILE_PATH; //open文件的路径存储在这个文件的缓冲区
//    file.nMaxFile=sizeof(FILE_PATH)/sizeof(*FILE_PATH);  //缓冲区能存的字符数
//    file.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST |OFN_FILEMUSTEXIST; //标志， 文件浏览|路劲必须存在|文件必须存在
//    // 每个选项使用 | 运算组合起来。

//    OPENFILENAMEA ofn;       // common dialog box structure
//    char szFile[260];       // buffer for file name
//    HWND hwnd;              // owner window
//    HANDLE hf;              // file handle

//    // Initialize OPENFILENAME
//    ZeroMemory(&ofn, sizeof(ofn));
//    ofn.lStructSize = sizeof(ofn);
//   ofn.hwndOwner = hwnd;
//    ofn.lpstrFile = (LPSTR)szFile;
//    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
//    // use the contents of szFile to initialize itself.
//    ofn.lpstrFile[0] = '\0';
//    ofn.nMaxFile = sizeof(szFile);
//    ofn.lpstrFilter = (LPSTR)"All\0*.*\0Text\0*.TXT\0";
//    ofn.nFilterIndex = 1;
//    ofn.lpstrFileTitle = NULL;
//    ofn.nMaxFileTitle = 0;
//    ofn.lpstrInitialDir = NULL;
//    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.

    QString FilePath = QFileDialog::getOpenFileName(this,"打开文件","/");  //GetOpenFileNameA()弹出一个选择文件的窗口，选择确认就返回真，取消就返回false

    QFileInfo info(FilePath);  //获取指定路径的文件信息
    if (info.exists()) {
        qDebug() << info.size();   //文件大小
          qDebug() << info.fileName();  //文件名
    } else {
        qDebug() << "文件路径不存在！";

    }

     qDebug()<<FilePath;  //输出文件路径
     return info;
//    if(false == flag)
//    {
//        return ;
//    }

    /*
    *第一个参数 this，指代父窗口为当前组件，并在当前父窗口下弹出一个子框口对话框
    *第二个参数tr("文件选取")，用于指定弹出的对话框标题为"文件选取"
    *第三个参数"C:"，指定对话框显示时默认打开的目录是C盘
    *第四个参数tr("文本文件(*txt)")，指文本过滤器
    */
//    QFileDialog::getOpenFileName(this, tr("文件选取"), "C:", NULL/*tr("文本文件(*txt)")*/);


}


//发送消息
void ChatDialog::on_pb_send_clicked()
{
    qDebug()<<__func__;
    //1:取出输入的数据，并判断数据合法性（判断是不是空或者全是空格）
        qDebug()<<" assa";
    QString content=ui->te_chat->toPlainText();  //取纯文本

    if(content.isEmpty() || content.remove(" ").isEmpty()){
        return;
    }
        qDebug()<<" afffffssa";

    //2:把编辑窗口清空
     content = ui->te_chat->toHtml();  //取带格式的内容
    ui->te_chat->setText("");//输入窗口置空


    //3：显示到浏览器窗口上，格式【我】时间...
    ui->tb_chat->append(QString("【我】 %1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
qDebug()<<" sasaassakkkk";
    //把数据发给kernel
    Q_EMIT SIG_sendMsg(m_id,content);
}

//发送文件
void ChatDialog::on_pb_tool3_clicked()
{
    //1：选择需要发送的文件
    QFileInfo FileInfo=OpenFile();
    if(NULL == FileInfo.filePath())
        return;
    //2：发送信号，将文件信息传给ckernel解决发送问题。
     Q_EMIT SIG_sendFile(m_id,FileInfo);

}

//视频通话按钮的槽函数
void ChatDialog::on_pb_tool2_clicked()
{
    //发信号告诉kernel，让kernel发送视频通话请求的信号
        Q_EMIT SIG_videoRq(m_id);
    QMessageBox::information(this,"提示","正在等待对方同意您的视频通话申请");
}

//语音按钮
void ChatDialog::on_pb_tool1_clicked()
{
    //发信号告诉kernel，让kernel发送视频通话请求的信号
        Q_EMIT SIG_SoundRq(m_id);
    QMessageBox::information(this,"提示","正在等待对方同意您的语音通话申请");
}

//挂断语音
void ChatDialog::on_pb_closeVoid_clicked()
{
   Q_EMIT SIG_CloseVoid(m_id);
}
