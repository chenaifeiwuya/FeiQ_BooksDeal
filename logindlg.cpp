#include "logindlg.h"
#include "ui_logindlg.h"
#include<QEvent>
#include<QKeyEvent>
#include<QMessageBox>

logindlg::logindlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::logindlg)
{
    ui->setupUi(this);
    //为输入电话号码，密码的lineEdit安装时间过滤器（监控），参数：监控者（谁去处理事件）
    ui->le_account->installEventFilter(this);
    ui->le_password->installEventFilter(this);
    ui->le_tel->installEventFilter(this);
    ui->le_register_password->installEventFilter(this);
}

logindlg::~logindlg()
{
    delete ui;
}

//重写父类的事件函数
bool logindlg::eventFilter(QObject *watched, QEvent *event)
{
    //电话号码输入框的：
    if(
            (watched == ui->le_account || watched == ui->le_tel) &&
            event->type() == QEvent::KeyPress)  //如果发生事件的对象是两个输入账号的lineEdit
      {
        QKeyEvent *pKey = (QKeyEvent*)event;   //强转为具体类型的事件                                                                                            //并且是键盘按下事件
        if(
                (Qt::Key_0 <= pKey -> key() && Qt::Key_9 >= pKey -> key()) ||
                Qt::Key_Backspace == pKey -> key()                  //放行这个键用于删除输入的值重新输入

                )
        {
            //放行：即交还给父类去处理
        }
        else{
            return true; //拦截
        }
    }

    //密码输入框
    if(
            (watched == ui->le_password || watched == ui->le_register_password) &&
            event->type() == QEvent::KeyPress)  //如果发生事件的对象是两个输入账号的lineEdit
      {
        QKeyEvent *pKey = (QKeyEvent*)event;   //强转为具体类型的事件                                                                                            //并且是键盘按下事件
        if(
                (Qt::Key_0 <= pKey -> key() && Qt::Key_9 >= pKey -> key()) ||
                  (Qt::Key_A<=pKey->key() && pKey->key()<=Qt::Key_Z) ||
                Qt::Key_Backspace == pKey -> key()                  //放行这个键用于删除输入的值重新输入

                )
        {
            //放行：即交还给父类去处理
        }
        else{
            return true; //拦截
        }
    }
     return QWidget::eventFilter(watched,event);  //交还给父类去处理
}

//一键注册按钮
void logindlg::on_pb_register_clicked()
{


    //：获取昵称，电话号，密码三个信息，发给kernel
    QString name=ui->le_name->text();
    QString tel = ui->le_tel->text();
    QString password = ui->le_register_password->text();
    //TODO:检验数据合法性：
    //电话号码中不能有空格只能出现数字，
    //密码中不能出现中文和空格，
    //昵称不能超过6位
    QString TempName = name;
    QString TempTel = tel;
    QString TempPassword = password;
    if(TempName.size()>6)  //昵称不能超过6位
    {
        QMessageBox::about(NULL,"提示","昵称不能超过6位");
        return;
    }
    if(TempName.remove(" ").size() !=  name.size())
    {
        QMessageBox::about(NULL,"提示","昵称中不能含有空格");
        return;
    }

    if(TempTel.size() != 11)
    {
        QMessageBox::about(NULL,"提示","电话号码必须为11位");
        return;
    }
    if(TempTel.remove(" ").size() !=  tel.size())
    {
        QMessageBox::about(NULL,"提示","电话号码中不能含有空格");
        return;

    }
    if(TempPassword.remove(" ").size() !=  password.size())
    {
        QMessageBox::about(NULL,"提示","电话号码中不能含有空格");
        return;

    }

    Q_EMIT SIG_register(name, tel, password);

}

//登录界面登录按钮
void logindlg::on_pb_login_clicked()
{
    //TODO:检验数据合法性
    //电话号，密码信息，发给kernel
    QString tel = ui->le_account->text();
    QString password = ui->le_password->text();
    //TODO:检验数据合法性：
    //电话号码中不能有空格只能出现数字，
    //密码中不能出现中文和空格，
    QString TempTel = tel;
    QString TempPassword = password;

    if(TempTel.size() != 11)
    {
        QMessageBox::about(NULL,"提示","电话号码必须为11位");
        return;
    }
    if(TempTel.remove(" ").size() !=  tel.size())
    {
        QMessageBox::about(NULL,"提示","电话号码中不能含有空格");
        return;

    }
    if(TempPassword.remove(" ").size() !=  password.size())
    {
        QMessageBox::about(NULL,"提示","电话号码中不能含有空格");
        return;

    }

    Q_EMIT SIG_login(tel, password);
}

//登录界面忘记密码按钮
void logindlg::on_pb_forgetPW_clicked()
{
    QMessageBox::about(this,"提示","抱歉客官，本功能尚未开通哦！");
}
