#ifndef FORM_H
#define FORM_H
#include <QtWidgets>
#include <QVideosurfaceFormat>
#include"MyVideoSurface.h"
#include <QWidget>
//多媒体 摄像头和设置信息
#include <QCamera>
#include <QCameraInfo>
#include <QImage>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    QCamera* camera_;
    MyVideoSurface* surface_;
    QSize sizeHint() const;
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);


private:
    explicit Form(QWidget *parent = nullptr);
    Form(Form const&);
    Form& operator=(Form const&);

public:
    ~Form();
    static Form* getSignalForm();
    void cameraStart();
    void cameraStop();
private slots:



    void on_pushButton_clicked();
signals:
    void SIG_closewindow(long id);

private:
    Ui::Form *ui;
    //重写关闭窗口函数
    void closeEvent(QCloseEvent *event);


};

#endif // FORM_H
