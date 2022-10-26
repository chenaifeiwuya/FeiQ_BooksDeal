#ifndef MAINDIALG_H
#define MAINDIALG_H

#include <QWidget>
#include"useritem.h"
#include<QVBoxLayout>
#include<QMenu>
#include<QMovie>

namespace Ui {
class MainDialg;
}

class MainDialg : public QWidget
{
    Q_OBJECT

public:
    explicit MainDialg(QWidget *parent = 0);
    ~MainDialg();
    //将自己的信息添加上
    void addMyInfo(int iconId, QString name, QString feeling);
    //设置自己的头像
    void setMyIcon(QString iconPath);
    //添加好友到好友列表中
    void addFriendWidget(useritem* friendWidget);

    //重写关闭窗口函数
    void closeEvent(QCloseEvent *event);
    //将某控件与他的父类窗口连接断开
    void removeWidgt(useritem* user);

signals:
   void SIG_closeWnd();
   void SIG_addFriend();
   void SIG_openBooks(); //打开二手书交易窗口指针
   void SIG_alterIcon(QByteArray img);


private slots:

   void on_pushButton_clicked();
   //处理菜单项的点击事件
   void slot_dealMenu(QAction* action);

   void on_pb_second_handBooks_clicked();

   void on_pb_userIcon_clicked();   //更换头像按钮

private:
    Ui::MainDialg *ui;
    QVBoxLayout* m_layout;  //  //QVBoxLayout垂直布局的层(要在大控件中装小控件就需要布局（画布）)
    QVBoxLayout* m_layout_notOline;
    QMenu *m_menu;
    int m_id;
    QMovie *movie;
};

#endif // MAINDIALG_H
