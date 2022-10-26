#ifndef FORMFRIEND_H
#define FORMFRIEND_H

#include <QWidget>
#include<process.h>
#include<iostream>
#include<winsock.h>


namespace Ui {
class FormFriend;
}

class FormFriend : public QWidget
{
    Q_OBJECT

    public:
    explicit FormFriend(QWidget *parent = 0);
    ~FormFriend();

    //线程函数，绘制视频窗口
     static unsigned __stdcall PaintThread(void* IpVoid);
      void videoDisplay();

private:
      //重写关闭窗口函数
      void closeEvent(QCloseEvent *event);
public:
      void displayStart();

public slots:
    void SLOT_videoDisplay(QImage image);

signals:
    SIG_closeFormfriend(long id);

private:
    Ui::FormFriend *ui;
    QImage image;
    bool isStop;
    bool disPlay;
    HANDLE m_handle;
public:
    long m_id;

};

#endif // FORMFRIEND_H
