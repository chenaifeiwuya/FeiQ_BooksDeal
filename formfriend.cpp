#include "formfriend.h"
#include "ui_formfriend.h"
#include <QDebug>

FormFriend::FormFriend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormFriend),isStop(false),disPlay(true)
{
    ui->setupUi(this);
//    m_handle=(HANDLE)_beginthreadex(NULL, 0, &PaintThread, (void*)this, 0, NULL);
}

FormFriend::~FormFriend()
{
//    isStop = true;
//    if (m_handle) {
//        if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100)) {
//            TerminateThread(m_handle, -1);
//        }
//        CloseHandle(m_handle);
//        m_handle = NULL;
//    }


    delete ui;
}

//线程函数，绘制窗口
unsigned FormFriend::PaintThread(void *IpVoid)  //暂时不使用线程函数
{
//        qDebug()<<__func__;
//    FormFriend* pThis = (FormFriend*)IpVoid;
//    pThis->videoDisplay();
}

//线程函数调用函数，展示对方的图片数据
void FormFriend::videoDisplay()
{
//    qDebug()<<__func__;
//    while(!isStop)
//    {
//    ui->label->setPixmap(QPixmap::fromImage(image));
//       ui->label->resize(image.width(),image.height());
    //    }
}

//重写关闭窗口函数
void FormFriend::closeEvent(QCloseEvent *event)
{
    qDebug()<<__func__;
    disPlay = false;
    //发信号告诉ckernel，让ckernel去回收资源
    Q_EMIT SIG_closeFormfriend(m_id);
}

void FormFriend::displayStart()
{
    disPlay = true;
}


void FormFriend::SLOT_videoDisplay(QImage img)
{
    if(disPlay)
    {
    qDebug()<<__func__;
    QImage newImg = img.scaled(ui->label->width(), ui->label->height()); //将图片设置成label的大小
        ui->label->setPixmap(QPixmap::fromImage(newImg));
    }
//           ui->label->resize(img.width(),img.height());  //将label设置成图片的大小
//    memcpy(&image,&img,sizeof(img));
//    qDebug()<<image.size()<<"qqqqqqq";
}
