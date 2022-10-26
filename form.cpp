#include "form.h"
#include "ui_form.h"



Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    qDebug()<<__func__;
    ui->setupUi(this);
    camera_ = new QCamera;
    surface_ = new MyVideoSurface(this);
    camera_->setViewfinder(surface_);
}

Form::~Form()
{
    qDebug()<<__func__;
    delete camera_;
    delete surface_;
    delete ui;

}

//获取改单例模式的对象
Form *Form::getSignalForm()
{

    return new Form;
}

//启动摄像头
void Form::cameraStart()
{
    camera_->start();  //摄像头启动
}

//暂停摄像头
void Form::cameraStop()
{
    camera_->stop();
}


QSize Form::sizeHint() const
{
    return surface_->surfaceFormat().sizeHint();
}

void Form::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
       if (surface_->isActive()) {
           const QRect videoRect = surface_->videoRect();
           if (!videoRect.contains(event->rect())) {
               QRegion region = event->region();
               region = region.subtracted(videoRect);
               QBrush brush = palette().background();
//               for (const QRect &rect : region){
//                   painter.fillRect(rect, brush);
//               }
//               QVector<QRect>::iterator ite=region.b
           }
           surface_->paint(&painter);//在主窗口绘制

       } else {
           painter.fillRect(event->rect(), palette().background());
       }
}

void Form::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
        surface_->updateVideoRect();
}

//结束视频通话的槽函数按钮
void Form::on_pushButton_clicked()
{
    qDebug()<<__func__;

    //发送信号给kernel，让kernel告诉对面视频通话结束

}

//重写关闭窗口函数
void Form::closeEvent(QCloseEvent *event)
{
    qDebug()<<__func__;
    Sleep(10);
//发信号告诉ckernel，让ckernel结束视频通话
    Q_EMIT SIG_closewindow(surface_->id);
}
