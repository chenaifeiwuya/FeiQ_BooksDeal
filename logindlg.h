#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QWidget>

namespace Ui {
class logindlg;
}

class logindlg : public QWidget
{
    Q_OBJECT
signals:
   //告诉kernel注册信息的信号
    void SIG_register(QString name, QString tel, QString password);
    //告诉kernel登录信息的信号
    void SIG_login(QString account, QString password);
public:
    explicit logindlg(QWidget *parent = 0);
    ~logindlg();

    virtual bool eventFilter(QObject *watched, QEvent *event);   //重写父类事件处理函数
private slots:
    void on_pb_register_clicked();

    void on_pb_login_clicked();

    void on_pb_forgetPW_clicked();

private:
    Ui::logindlg *ui;
};

#endif // LOGINDLG_H
