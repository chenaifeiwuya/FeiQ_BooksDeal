#ifndef FEIQDIALOG_H
#define FEIQDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class FeiQDialog; }
QT_END_NAMESPACE

class FeiQDialog : public QDialog
{
    Q_OBJECT



signals:
    //发送双击的哪一行的ip给kernel
    void SIG_userClicked(QString ip);
    //发送关闭窗口的信号
    void SIG_closeWnd();

public:
     explicit FeiQDialog(QWidget *parent=0);
    ~FeiQDialog();

    //添加好友
    void addFriend(QString ip, QString hostName);
    //删除好友
    void deleteFriend(QString ip);

    //重写关闭窗口函数
    void closeEvent(QCloseEvent* event);

private slots:
    //双击某一行
    void on_tw_friend_cellDoubleClicked(int row, int column);

private:
    Ui::FeiQDialog *ui;
};
#endif // FEIQDIALOG_H
