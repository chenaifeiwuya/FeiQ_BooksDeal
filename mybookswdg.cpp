#include "mybookswdg.h"
#include "ui_mybookswdg.h"

MyBooksWdg::MyBooksWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyBooksWdg)
{
    ui->setupUi(this);
}

MyBooksWdg::~MyBooksWdg()
{
    delete ui;
}

//设置图片
void MyBooksWdg::setImgInfo()
{
        ui-> pb_bookImg->setIcon(QIcon(imgPath));
}

//设置文字信息
void MyBooksWdg::setTextInfo(int School,QString bookNameUt, QString bookAreaUt, QString bookPriceUt, QString bookInfoUt)
{
    isSchool = School ;  //保存类别信息
    bookName = bookNameUt;  //保存名字
    ui->lb_bookName->setText(bookNameUt);
    ui->lb_bookArea->setText(bookAreaUt);
    ui->lb_bookPrice->setText(bookPriceUt);
    ui->txt_bookInfo->setText(bookInfoUt);
}

//删除该书籍
void MyBooksWdg::on_pb_deleteBook_clicked()
{
    Q_EMIT SIG_deleteBookInfo(isSchool,bookName);
    //TODO:调试下架功能
}
