#ifndef USERITEM_H
#define USERITEM_H

#include <QWidget>
#include"chatdialog.h"

namespace Ui {
class useritem;
}

class useritem : public QWidget
{
    Q_OBJECT

public:
    explicit useritem(QWidget *parent = 0);
    ~useritem();
    //添加好友信息
    void addFriendInfo(int iconId, int status, QString name, QString feeling, int id);
    //显示聊天信息
      void setContent(QString content);
      //下线
      void offLine();

private slots:
    void on_pb_icon_clicked();

signals:
    void SIG_chatToFriend(int id);
private:
    Ui::useritem *ui;
    int m_id;
    int m_iconId;

    QString m_feeling;

public:
        QString m_name;
        int m_status;
public:
        ChatDialog* m_chatDialog;  //对应的聊天窗口指针
};

#endif // USERITEM_H
