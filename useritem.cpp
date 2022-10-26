#include "useritem.h"
#include "ui_useritem.h"
#include"packDef.h"
#include<QIcon>
#include<QBitmap>

useritem::useritem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::useritem)
{
    ui->setupUi(this);
    m_chatDialog = new ChatDialog;
    qDebug()<<m_id;


}

useritem::~useritem()
{
    delete ui;
    m_chatDialog->hide();
    delete m_chatDialog;
}

//添加好友信息
void useritem::addFriendInfo(int iconId, int status, QString name, QString feeling, int id)
{
    m_chatDialog->m_id=id;

    m_id = id;
    m_iconId = iconId;
    m_status = status;
    m_name = name;
    m_feeling = feeling;
    ui->lb_name->setText(name);
    ui->lb_feeling->setText(feeling);

    QString iconPath=QString(":/tx/%1.png").arg(iconId);
    //判断好友的状态
    if( status_online == status)
    {
        //好友在线，头像亮显
//        ui->pb_icon->setPixmap(QPixmap(iconPath));
         ui->pb_icon->setIcon(QIcon(iconPath));
    }
    else{
        //不在线，灰色图案
        QBitmap bmp;
        bmp.load(iconPath);
//         ui->lb_icon->setPixmap(bmp);
        ui->pb_icon->setIcon(bmp);

    }

     m_chatDialog->setWindowTitle(QString("与【%1】的聊天窗口").arg(m_name));//将窗口标题加上
    //重绘
     this->repaint();
}

//将聊天信息显示到聊天窗口上
void useritem::setContent( QString content)
{
    m_chatDialog->setContent(m_name, content);
    m_chatDialog->showNormal();
}

//下线
void useritem::offLine()
{
        qDebug()<<__func__;
    m_status = status_offline;
       QString iconPath=QString(":/tx/%1.png").arg(m_iconId);
       QBitmap bmp;
       bmp.load(iconPath);
//         ui->lb_icon->setPixmap(bmp);
       ui->pb_icon->setIcon(bmp);

}

void useritem::on_pb_icon_clicked()
{
//    Q_EMIT SIG_chatToFriend(m_id);  //发信号让kernel打开聊天窗口
    m_chatDialog->showNormal();
}
