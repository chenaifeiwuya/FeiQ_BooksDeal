#include "feiqdialog.h"
#include "ui_feiqdialog.h"
#include<QDebug>
#include"QString"
#include<QMessageBox>
#include<QCloseEvent>


FeiQDialog::FeiQDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FeiQDialog)
{
    ui->setupUi(this);
}

FeiQDialog::~FeiQDialog()
{
    delete ui;
}

//添加好友
void FeiQDialog::addFriend(QString ip, QString hostName)
{
    //总行数+1
    ui->tw_friend->setRowCount(ui->tw_friend->rowCount()+1);
    //在新增加的行里面加上ip和主机名
    QTableWidgetItem* itemIp=new QTableWidgetItem(ip);
    ui->tw_friend->setItem(ui->tw_friend->rowCount()-1,0,itemIp);
    QTableWidgetItem* itemHostName=new QTableWidgetItem(hostName);
    ui->tw_friend->setItem(ui->tw_friend->rowCount() -1,1,itemHostName);
}

//删除好友
void FeiQDialog::deleteFriend(QString ip)
{
    //遍历每一行，比较ip地址，ip地址相等就删除那一行
    for(int i=0;i  < ui->tw_friend->rowCount();){
        if(ip == ui ->tw_friend->item(i,0)->text())
    {
            ui->tw_friend->removeRow(i);
        }else{
        i++;
    }
}
    //重绘
    this->repaint();
}

//重写关闭窗口函数
void FeiQDialog::closeEvent(QCloseEvent *event)
{
    //弹出一个提示窗口，确认是否关闭
    if(QMessageBox::Yes == QMessageBox::question(this, "提示", "是否确认关闭？"))
    {
        //发送关闭窗口信号给kernel，让kernel去回收资源和关闭窗口
        Q_EMIT SIG_closeWnd();
    }else{
        //忽略用户操作
        event->ignore();  //如果不忽略，则还是会关闭（返回父函数）.
    }
}

//双击某一行
void FeiQDialog::on_tw_friend_cellDoubleClicked(int row, int column)
{


    //qDebug() << QString("(%1,%2)").arg(row).arg(column);    //arg()用于填充字符串中的%1,%2…为给定的参数

    //取出双击的哪一行的ip
    QString ip = ui-> tw_friend->item(row,0)->text();
    qDebug()<<ip;
    //发送一个信号给kernel，通知它点击的是哪一行，也就是把ip发给kernel
    Q_EMIT SIG_userClicked(ip);


}

