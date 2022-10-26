#include "books.h"
#include "ui_books.h"
#include<stdio.h>
#include<QDebug>
#include<QMessageBox>

Books::Books(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Books)
{
    ui->setupUi(this);
    memset(imgPath,0,_DEF_IMG_PATH_SIZE);
}

Books::~Books()
{
    delete ui;
}

//设置图片信息
void Books::setImgInfo()
{
    ui->pb_bookImg->setIcon(QIcon(imgPath));
}

//设置文字信息
void Books::setTextInfo(QString bookNameUt,QString bookAreaUt,QString bookPriceUt,QString bookInfoUt,char *QQ,char *weChat)
{
    qDebug()<<__func__;
       ui->lb_bookName->setText(bookNameUt);
       ui->txt_bookInfo->setText(bookInfoUt);
       ui->lb_area->setText(bookAreaUt);
       ui->lb_Price->setText(bookPriceUt);
       strcpy(book_QQ,QQ);
       strcpy(book_weChat,weChat);
}

//联系卖家按钮
void Books::on_pb_connectBoss_clicked()
{
    QString info = QString("QQ:%1    微信:%2").arg(book_QQ).arg(book_weChat);
    QMessageBox::about(this,"他的联系方式",info);
}
