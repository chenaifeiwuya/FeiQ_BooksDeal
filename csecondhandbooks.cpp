#include "csecondhandbooks.h"
#include "ui_csecondhandbooks.h"
#include "packDef.h"
#include <QDebug>
#include<QMessageBox>



CSecondHandBooks::CSecondHandBooks(QWidget *parent) :
    QWidget(parent),selectWdgType(SCHOOL_BOOK),
    ui(new Ui::CSecondHandBooks)
{
    ui->setupUi(this);
    //给层new对象
       m_Layout_School = new QVBoxLayout;
       m_Layout_NoSchool = new QVBoxLayout;
       m_Layout_Other = new QVBoxLayout;
       m_Layout_Select = new QVBoxLayout;
       m_Layout_MyBook = new QVBoxLayout;

       //设置小控件距离大控件上下左右的间距
       m_Layout_School->setContentsMargins(0, 0, 0, 0);
       m_Layout_NoSchool->setContentsMargins(0, 0, 0, 0);
       m_Layout_Other->setContentsMargins(0, 0, 0, 0);
       m_Layout_Select->setContentsMargins(0, 0, 0, 0);
       m_Layout_MyBook->setContentsMargins(0, 0, 0, 0);


       //每个控件的间距
       m_Layout_School->setSpacing(3);
       m_Layout_NoSchool->setSpacing(3);
       m_Layout_Other->setSpacing(3);
       m_Layout_Select->setSpacing(3);
       m_Layout_MyBook->setSpacing(3);


       //把层设置到控件上
       ui->wdg_list_school ->setLayout(m_Layout_School);
       ui->wdg_list_noschool ->setLayout(m_Layout_NoSchool);
       ui->wdg_list_other ->setLayout(m_Layout_Other);
       ui->wdg_list_select ->setLayout(m_Layout_Select);
       ui->wdg_list_mybook ->setLayout(m_Layout_MyBook);


}

CSecondHandBooks::~CSecondHandBooks()
{
    delete ui;
}

//添加书籍窗口
void CSecondHandBooks::addBooksWdg(Books *bookWdg, int isSchool)
{
    if(SELECT_BOOK == isSchool)   //如果是搜索的书籍信息
    {
        //添加进来后将scollArea的子控件长度拉长
        QSize size =  ui->scrolArea_Select->size();
        ui->scrolArea_Select->setFixedHeight(size.height()+200);
        m_Layout_Select->addWidget(bookWdg);
                bookWdg->showNormal();
                return;
    }
    else{   //如果不是

    if( SCHOOL_BOOK == isSchool )  //如果是教育书籍
    {


        //添加进来后将scollArea的子控件长度拉长
        QSize size =  ui->scrollArea_School->size();
        ui->scrollArea_School->setFixedHeight(size.height()+200);
        m_Layout_School->addWidget(bookWdg);
                bookWdg->showNormal();
    }
    else if(NO_SCHOOL_BOOK == isSchool)   //非教育书籍
    {


        //添加进来后将scollArea的子控件长度拉长
        QSize size =  ui->scrollArea_NoSchool->size();
        ui->scrollArea_NoSchool->setFixedHeight(size.height()+200);
        m_Layout_NoSchool->addWidget(bookWdg);
                bookWdg->showNormal();
    }
    else      //其它
    {


        //添加进来后将scollArea的子控件长度拉长
        QSize size =  ui->scorllArea_other->size();
        ui->scorllArea_other->setFixedHeight(size.height()+200);
        m_Layout_Other->addWidget(bookWdg);
                bookWdg->showNormal();
        qDebug()<<"           sdasdasdasasd "<<size.height();

    }

    }
}

//添加自己上架的书的窗口
void CSecondHandBooks::addMyBooksWdg(MyBooksWdg *bookWdg, int isSchool)
{

        //添加进来后将scollArea的子控件长度拉长
        QSize size =  ui->scrollArea_Mybook->size();
        ui->scrollArea_Mybook->setFixedHeight(size.height()+200);
        m_Layout_MyBook->addWidget(bookWdg);
                bookWdg->showNormal();
                return;
}

void CSecondHandBooks::init()
{
    Q_EMIT SIG_getBooksInfo(ALL_BOOK_AND_OTHER, 0);    //发送书籍信息请求给ckernel，请求内容为全部物品！
}


//打开上传书籍窗口
void CSecondHandBooks::on_pb_sellBooks_clicked()
{
    //发信号告诉ckernel，让ckernel去办
    Q_EMIT SIG_sellBooks();
}

//加载更多其他物品的信息
void CSecondHandBooks::on_pushButton_clicked()
{
    //首先得出此按钮所在区域目前有多少本书籍
    QSize size = ui->wdg_list_other->size();
    int num = size.height()  / 200 ;
    //发信号给kernel，让kernel去办
    Q_EMIT SIG_getBooksInfo(OTHER_GOODS , num);
}

//加载更多非校内书籍
void CSecondHandBooks::on_pushButton_2_clicked()
{
    //首先得出此按钮所在区域目前有多少本书籍
    QSize size= ui->wdg_list_noschool->size();
    int num = size.height()  / 200 ;
    //发信号给kernel，让kernel去办
    Q_EMIT SIG_getBooksInfo(NO_SCHOOL_BOOK , num);
}

//加载更多校内书籍
void CSecondHandBooks::on_pushButton_3_clicked()
{
    //首先得出此按钮所在区域目前有多少本书籍
    QSize size= ui->wdg_list_school->size();
    int num = size.height()  / 200 ;
    //发信号给kernel，让kernel去办
    Q_EMIT SIG_getBooksInfo(SCHOOL_BOOK , num);
}


//搜索教学用书籍
void CSecondHandBooks::on_pb_selectBooks_clicked()
{
    QString bookName = ui-> le_bookName->text();
    if(bookName.remove(" ").isEmpty())
    {
        QMessageBox::information(this,"提示","请输入您要搜索的书名");
        return ;

    }
//    else if(bookName.remove(" ").size >= _DEF_BOOK_NAME_SIZE)
//    {
//        QMessageBox::information(this,"提示","您输入的信息过多，请精简");
//        return ;
//    }
    ui->scrolArea_Select->setFixedHeight(469);   //大小还原
    selectWdgType = SCHOOL_BOOK;  //保存搜索类型
    select_bookName = bookName;    //保存搜索名
    Q_EMIT SIG_getSelectBooksInfo(SCHOOL_BOOK,0,bookName,CLEAR_SELECT_WDG);
    //点击搜索后自动跳转到搜索面板Tab
    ui->Tab_displayBooks->setCurrentIndex(3);

}

//搜索非教学用书籍
void CSecondHandBooks::on_pb_select_otherbook_clicked()
{
    QString bookName = ui-> le_bookName->text();
    if(bookName.remove(" ").isEmpty())
    {
        QMessageBox::information(this,"提示","请输入您要搜索的书名");
        return ;

    }
//    else if(bookName.remove(" ").size >= _DEF_BOOK_NAME_SIZE)
//    {
//        QMessageBox::information(this,"提示","您输入的信息过多，请精简");
//        return ;
//    }
    ui->scrolArea_Select->setFixedHeight(469);   //大小还原
    selectWdgType = NO_SCHOOL_BOOK;  //保存搜索类型
    select_bookName = bookName;     //保存搜索名
    Q_EMIT SIG_getSelectBooksInfo(NO_SCHOOL_BOOK,0,bookName,CLEAR_SELECT_WDG);
    //点击搜索后自动跳转到搜索面板Tab
    ui->Tab_displayBooks->setCurrentIndex(3);
}

//搜索其他
void CSecondHandBooks::on_pb_select_other_goods_clicked()
{
    QString bookName = ui-> le_bookName->text();
    if(bookName.remove(" ").isEmpty())
    {
        QMessageBox::information(this,"提示","请输入您要搜索的书名");
        return ;
    }
//    else if(bookName.remove(" ").size >= _DEF_BOOK_NAME_SIZE)
//    {
//        QMessageBox::information(this,"提示","您输入的信息过多，请精简");
//        return ;
//    }

    ui->scrolArea_Select->setFixedHeight(469);   //大小还原
    selectWdgType = OTHER_GOODS;  //保存搜索类型
    select_bookName = bookName;     //保存搜索名
    Q_EMIT SIG_getSelectBooksInfo(OTHER_GOODS,0,bookName,CLEAR_SELECT_WDG);
    //点击搜索后自动跳转到搜索面板Tab
    ui->Tab_displayBooks->setCurrentIndex(3);
}

//加载更多搜索按钮
void CSecondHandBooks::on_pushButton_4_clicked()
{
    //首先得出此按钮所在区域目前有多少本书籍
    QSize size= ui->scrolArea_Select->size();
    int num = size.height()  / 200 ;
    qDebug()<<size.height()<<"              a sadasddddddddddddddddddddd";
    //发信号给kernel，让kernel去办
    Q_EMIT SIG_getSelectBooksInfo(selectWdgType,num,select_bookName,NO_CLEAR_SELECT_WDG);
}

//加载我上架的所有物品
void CSecondHandBooks::on_pb_MyBooks_clicked()
{
    Q_EMIT SIG_getMyBooksInfo();
}

//我上架的所有物品一键下架
void CSecondHandBooks::on_pb_deleteAllMybooks_clicked()
{

}
