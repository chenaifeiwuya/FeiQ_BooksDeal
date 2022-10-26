#ifndef ADDNEWBOOKS_H
#define ADDNEWBOOKS_H

#include <QWidget>
#include <QImage>
#include "packDef.h"

namespace Ui {
class addNewBooks;
}

class addNewBooks : public QWidget
{
    Q_OBJECT

public:
    explicit addNewBooks(QWidget *parent = 0);
    ~addNewBooks();

private slots:
    void on_pb_bookImg_clicked();
    void on_pb_shangJia_clicked();

signals:
    void SIG_SendBookInfo(QByteArray byte,QString name, QString Are,QString info,QString price,QString QQ,QString weChat,int isSchool);


private:
    Ui::addNewBooks *ui;
    QImage img;
};

#endif // ADDNEWBOOKS_H
