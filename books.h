#ifndef BOOKS_H
#define BOOKS_H

#include <QWidget>
#include<QImage>
#include "packDef.h"


namespace Ui {
class Books;
}

class Books : public QWidget
{
    Q_OBJECT

public:
    explicit Books(QWidget *parent = 0);
    ~Books();

    void setImgInfo();
    void setTextInfo(QString bookNameUt,QString bookAreaUt,QString bookPriceUt,QString bookInfoUt,char *QQ,char *weChat);

public:
     char imgPath[_DEF_IMG_PATH_SIZE];
     char book_QQ [_DEF_QQ_SIZE];
     char book_weChat[_DEF_WECHAT_SIZE];

private slots:
     void on_pb_connectBoss_clicked();

private:
    Ui::Books *ui;
};

#endif // BOOKS_H
