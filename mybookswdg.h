#ifndef MYBOOKSWDG_H
#define MYBOOKSWDG_H

#include <QWidget>
#include"packDef.h"

namespace Ui {
class MyBooksWdg;
}

class MyBooksWdg : public QWidget
{
    Q_OBJECT

public:
    explicit MyBooksWdg(QWidget *parent = 0);
    ~MyBooksWdg();
    void setTextInfo(int isSchool,QString bookNameUt, QString bookAreaUt, QString bookPriceUt, QString bookInfoUt);   //设置书籍文字信息
    void setImgInfo();
private slots:
    void on_pb_deleteBook_clicked();

signals:
    void SIG_deleteBookInfo(int isSchool,QString bookName);

public:
         char imgPath[_DEF_IMG_PATH_SIZE];

private:
    Ui::MyBooksWdg *ui;
    int isSchool;   //记录书籍是哪个种类
    QString bookName;
};

#endif // MYBOOKSWDG_H
