#include "addnewbooks.h"
#include "ui_addnewbooks.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QBuffer>


addNewBooks::addNewBooks(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addNewBooks)
{
    ui->setupUi(this);
}

addNewBooks::~addNewBooks()
{
    delete ui;
}

void addNewBooks::on_pb_bookImg_clicked()
{
    /*
           getOpenFileName函数说明
           函数原形： QStringList QFileDialog::getOpenFileNames(
           QWidget * parent = 0,
           const QString & caption = QString(),    //  打开文件对话框的标题
           const QString & dir = QString(),            //  查找目录
           const QString & filter = QString(),     //  设置需要过滤的文件格式
           QString * selectedFilter = 0,
           Options options = 0) [static]
           */
    QString FilePath = QFileDialog::getOpenFileName(this,"打开文件","/", "*.jpg ; *.png");  //GetOpenFileNameA()弹出一个选择文件的窗口，选择确认就返回真，取消就返回false
                                                                          //注意过滤器之间使用 ; 进行分隔
    if(true == FilePath.isEmpty() )  //如果没有选中文件，则返回
    {
        return ;
    }
    QFileInfo info(FilePath);  //获取指定路径的文件信息
    if (info.exists()) {
        qDebug() << info.size();   //文件大小
          qDebug() << info.fileName();  //文件名
    } else {
        qDebug() << "文件路径不存在！";
           return ;
    }



    img.load(FilePath);
    ui->pb_bookImg->setText("");
    ui->pb_bookImg->setIcon(QIcon(FilePath));  //将图片显示在buton上



     qDebug()<<FilePath;  //输出文件路径
     return ;
}

//上架按钮
void addNewBooks::on_pb_shangJia_clicked()
{

    if(img.isNull())
    {
        QMessageBox::information(this,"提示","请上传书籍图片信息");
        return ;
    }
    QString bookName = ui->le_bookName->text();
    if(bookName.remove(" ").isEmpty() || bookName.size() > 40)
    {
            QMessageBox::information(this,"提示","书籍名称为空或位数过长");
            return ;

    }
    QString bookArea = ui->le_bookArea->text();
    if(bookArea.remove(" ").isEmpty() || bookArea.size() > 30)
    {
            QMessageBox::information(this,"提示","书籍在哪个校区为空或位数过长");
            return ;

    }
    QString bookInfo = ui->le_bookInfo->text();
    if(bookInfo.remove(" ").isEmpty() || bookInfo.size() > 100)
    {
            QMessageBox::information(this,"提示","书籍出版社和版本相关信息为空或过长");
            return ;

    }
    QString bookPrice = ui->le_bookPrice->text();
    if(bookPrice.remove(" ").isEmpty() || bookPrice.size() > 5)
    {
            QMessageBox::information(this,"提示","书籍价格为空或过长");
            return ;

    }
    QString  QQ = ui->le_QQ->text();
    QString weChat = ui->le_weCHat->text();
    if(QQ.remove(" ").isEmpty() && weChat.remove(" ").isEmpty() ||QQ.size() > 15 || weChat.size() > 20)
    {
        QMessageBox::information(this,"提示","QQ和微信联系方式请至少填入一个或位数过长");
        return ;

    }
    QMessageBox question;
    question.setText("请问您要上架的物品为校内教育用书籍吗？如果是非书籍类物品请选择其他");
    question.addButton("    是的     ",QMessageBox::YesRole);
    question.addButton("    不是     ",QMessageBox::NoRole);
    question.addButton("其他非书籍类物品",QMessageBox::HelpRole);
    question.showNormal();
    int isSchool = question.exec();//返回值为所按按钮的索引  (就是0，1，2)
//    qDebug()<<isSchool<<"       sssssssssssssssssssssssssssssssss";
    //    if(QMessageBox::YesRole == isSchool)
    //    {
    //        isSchool = SCHOOL_BOOK;
    //    }
    //    else if(QMessageBox::NoRole == isSchool)
    //    {
    //        isSchool = NO_SCHOOL_BOOK;
    //    }
    //    else{
    //        isSchool = OTHER_GOODS;  //其他物品
    //    }
    qDebug()<<OTHER_GOODS;
    qDebug()<<isSchool;
    //先将图片压缩到固定大小
    //第一、二参数代表缩放后的尺寸，第三个参数代表忽略比例，也可调用IgnoreAspectRatio保持长宽比例，第四个参数代表平滑处理，使图片缩小时不失真。
    QImage image=img.scaled(300,500,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //qDebug()<<"大小为"<<image.size();
    //将图片转换为QByteArray，再经过信号传递给kernel
     QByteArray byte;   //QImage转QByteArray,这个要借助QBuffer类：
     QBuffer buff(&byte);
     buff.open(QIODevice::WriteOnly);
     image.save(&buff,"JPEG");
     qDebug()<<byte.size()<<"wwwwwww";
      qDebug()<<byte.size();


      //发信号告诉ckernel上传书籍信息
      Q_EMIT SIG_SendBookInfo(byte,bookName, bookArea,bookInfo,bookPrice,QQ,weChat,isSchool);


}
