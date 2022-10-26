#ifndef CSECONDHANDBOOKS_H
#define CSECONDHANDBOOKS_H

#include <QWidget>
#include<QBoxLayout>
#include<books.h>
#include"packDef.h"
#include"mybookswdg.h"

namespace Ui {
class CSecondHandBooks;
}

class CSecondHandBooks : public QWidget
{
    Q_OBJECT

public:
    explicit CSecondHandBooks(QWidget *parent = 0);
    ~CSecondHandBooks();
    void addBooksWdg(Books *bookWdg,int isSchool);  //添加进窗口
    void addMyBooksWdg(MyBooksWdg *bookWdg, int isSchool);   //添加自己上架的书的窗口
    void init();

private slots:
    void on_pb_sellBooks_clicked();
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pb_selectBooks_clicked();

    void on_pb_select_otherbook_clicked();

    void on_pb_select_other_goods_clicked();

    void on_pushButton_4_clicked();

    void on_pb_MyBooks_clicked();

    void on_pb_deleteAllMybooks_clicked();

signals:
    void SIG_sellBooks();//打开上传书籍窗口
    void SIG_getBooksInfo(int isSchool , int num);   //获取书籍信息
    void SIG_getSelectBooksInfo(int isSchool, int num, QString bookName , bool isClear);   //搜索书籍
    void SIG_getMyBooksInfo();   //获取我上架的所有物品的信息
    void SIG_deletMyTargetBookInfo(QString bookName);   //下架目标书籍
private:
    Ui::CSecondHandBooks *ui;
    QBoxLayout* m_Layout_School;   //动态添加控件需要用到画布，即层
    QBoxLayout* m_Layout_NoSchool;   //动态添加控件需要用到画布，即层
    QBoxLayout* m_Layout_Other;   //动态添加控件需要用到画布，即层
    QBoxLayout* m_Layout_Select;   //动态添加控件需要用到画布，即层
    QBoxLayout* m_Layout_MyBook;   //动态添加控件需要用到画布，即层

        int selectWdgType;   //保存当前搜索窗口搜索的是什么类型的数据
        QString select_bookName;   //保存搜索窗口搜索的名字
//    //发送获取书籍信息请求给服务器
//    STRU_TCP_BOOKS_INFO_RQ rq;
//    rq.id = m_id;
//    m_pTcpMediator->SendData(0,(char*)&rq,sizeof(rq));

};

#endif // CSECONDHANDBOOKS_H
