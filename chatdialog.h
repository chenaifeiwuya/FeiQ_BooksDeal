#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QWidget>
#include<QDebug>
#include<winsock.h>
#include<qfiledialog.h>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QWidget
{
    Q_OBJECT

signals:
    //发送聊天数据
    void SIG_sendMsg( int id, QString content);
    //发送文件
    void SIG_sendFile(int ip,QFileInfo FileInfo);
    //处理发送视频通话请求的信号
    void SIG_videoRq(long id);
    //发送语音通话信号
    void SIG_SoundRq(long id);
    //发信号让ckernel挂断语音
    void SIG_CloseVoid( long id );

public:
    explicit ChatDialog( QWidget *parent = nullptr);
    ~ChatDialog();

    //把接收到的聊天信息显示到窗口上
    void setContent(QString name,QString content);

    //获取文件信息
    QFileInfo OpenFile();
private slots:
    void on_pb_send_clicked();

    void on_pb_tool3_clicked();

    void on_pb_tool2_clicked();  //视频通话按钮


    void on_pb_tool1_clicked();  //语音按钮

    void on_pb_closeVoid_clicked();

private:
    Ui::ChatDialog *ui;

public:
    int m_id;
};

#endif // CHATDIALOG_H
