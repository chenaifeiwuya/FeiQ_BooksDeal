#include "maindialg.h"
#include "ui_maindialg.h"
#include<QIcon>
#include<QMessageBox>
#include<QCloseEvent>
#include"packDef.h"
#include<QImage>
#include <QFileDialog>
#include<QBuffer>

MainDialg::MainDialg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainDialg)
{
    ui->setupUi(this);
    //给层new对象
       m_layout = new QVBoxLayout;
       m_layout_notOline = new QVBoxLayout;
       //设置小控件距离大控件上下左右的间距
       m_layout->setContentsMargins(0, 0, 0, 0);
       m_layout_notOline->setContentsMargins(0, 0, 0, 0);
       //每个控件的间距
       m_layout->setSpacing(3);
       m_layout_notOline->setSpacing(3);
       //把层设置到控件上
       ui->wdg_list->setLayout(m_layout);
       ui->wdg_list_notOline->setLayout(m_layout_notOline);
       //将个性签名的lineEdit设置成透明的：
       ui->le_userFeeling->setStyleSheet("background:transparent;border-width:0;border-style:outset");   //将lineEdit变成透明的
       //添加菜单项，指定父控件，父控件负责子控件的回收
       m_menu = new QMenu(this);
       m_menu->addAction("添加好友");
       m_menu->addAction("添加群组");
//       m_menu->addAction("系统设置");
       //绑定菜单项的点击信号和槽函数
       connect(m_menu,SIGNAL(triggered(QAction*)),   //menu的固定信号，就是点击了哪个菜单项
               this,SLOT(slot_dealMenu(QAction*)));

       movie = new QMovie(".//gif//China.gif");   //初始化movie对象，播放gif动画
       ui->label->setMovie(movie);
       //播放加载的动画
       movie->start();

}

MainDialg::~MainDialg()
{
    delete movie;
    delete ui;
}

//添加自己的信息
void MainDialg::addMyInfo(int id, QString name, QString feeling)
{
        m_id = id;
        ui->lb_userName->setText(name);
        ui->le_userFeeling->setText(feeling);
        ui->le_userFeeling->setFont(QFont( "Timers" , 10 ,  QFont::Bold) );  //设置字体和大小
        QString idString;
        char idchar[5];
        itoa(id,idchar,10);
        idString = QString(idchar);
        QString iconPath = QString(".//icon//%1.jpg").arg(idString);   //用户头像路径为根据用户id得出的固定路径
        qDebug()<<iconPath;
       // ui->pb_userIcon->setIcon(QIcon(iconPath));
}

//设置自己的头像
void MainDialg::setMyIcon(QString iconPath)
{

}

//添加好友到列表上
void MainDialg::addFriendWidget(useritem *item)
{

    if( status_online == item->m_status)
    m_layout->addWidget(item);  //如果在线则添加到第一个窗口
    else
    m_layout_notOline->addWidget(item);    //不在线则添加到第二个窗口

}

//重写关闭窗口函数
void MainDialg::closeEvent(QCloseEvent *event)
{
    //弹出一个提示窗口，确认是否关闭
    if(QMessageBox::Yes == QMessageBox::question(this, "提示", "是否确认关闭？")){
        //发送关闭窗口信号给kernel,让kernel去回收资源和关闭窗口
        Q_EMIT SIG_closeWnd();
    }else{
        //忽略用户操作
        event->ignore();
    }
}

//将某控件与他的父类窗口练习断开
void MainDialg::removeWidgt(useritem *user)
{
    /*如图， 最外层是widget， 有一个layout，然后往这个layout里面添加3个widget，
     * 用完了在某个时候用layout.removeWidget(widget1)把他们移除掉，然而事实却是仍然存在，
     * 其实这个widget1的父类其实是最外层的widget，所以移除也很简单，设置它们的父类为空即可

        */
    user->setParent(NULL);
}


//
void MainDialg::on_pushButton_clicked()
{
    //1:获取鼠标的点击位置
    QPoint p = QCursor::pos();
    //2:计算菜单栏的绝对高度--》所有菜单项添加完以后的高度
    QSize size = m_menu->sizeHint();  //获得菜单项的绝对高度以及宽度

    //3:在点击位置向上显示一个菜单栏
    m_menu->exec(QPoint(p.x() -size.width() ,p.y() /*- size.height()*/));
}

void MainDialg::slot_dealMenu(QAction *action)
{
    if("添加好友" == action->text())
    {
        //处理添加好友，给kernel发信号
        Q_EMIT SIG_addFriend();
        qDebug() << "slot_dealMenu: 添加好友";
    }else if("添加群组" == action->text()){
        //添加群组
    }
}

//二手书交易按钮
void MainDialg::on_pb_second_handBooks_clicked()
{
    //发信号告诉ckernel打开二手书交易窗口
    Q_EMIT SIG_openBooks();
}

//更换头像
void MainDialg::on_pb_userIcon_clicked()
{

    QMessageBox::information(this,"提示","您选择的头像应为正方形,请提前裁剪，否则会出现与头像框比例不一致的问题！");
    //打开文件夹，选择图片
    QImage img;
    QString FilePath = QFileDialog::getOpenFileName(this,"打开文件","/", "*.jpg ; *.png");  //GetOpenFileNameA()弹出一个选择文件的窗口，选择确认就返回真，取消就返回false
                                                                          //注意过滤器之间使用 ; 进行分隔
    if(true == FilePath.isEmpty() )  //如果没有选中文件，则返回
    {
        return ;
    }
    QFileInfo info(FilePath);  //获取指定路径的文件信息
    if (info.exists()) {
        qDebug() << info.size();   //文件大小
          qDebug() << info.fileName();  //文件名
    } else {
        qDebug() << "文件路径不存在！";
           return ;
    }


        qDebug()<<FilePath;
        FILE* fp;
    char *imgInfo = new char[info.size()];   //用于保存图片数据
    if(0 == fopen_s(&fp,FilePath.toStdString().c_str(),"rb"))
    {
         //如果能够打开
        //写入imgInfo
        int size = info.size();
       int num= fread(imgInfo,size,1,fp);    //num是写入数据块的个数
        if(num == 1)  //如果写入数据长度等于文件信息长度
            qDebug()<<"写入成功";
        fclose(fp);
    }
    else{
        qDebug()<<"保存失败";
        perror("保存失败");    //如果打开失败则查看是什么错误
        return ;
    }



    //将图片复制到img文件夹中

    char IconPath[30] = ".//icon//";  //头像路径
    char id[5];
    itoa(m_id,id,10);
    QString idString = QString(id);
    strcat(IconPath,idString.toStdString().c_str());
    strcat(IconPath,".jpg");
    if(0 == fopen_s(&fp,IconPath,"wb"))
    {
        int num = fwrite(imgInfo ,info.size(),1,fp);
        qDebug()<<"num="<<num;
        if (num == 1)
        {
            qDebug()<<"写入头像成功";
                    fclose(fp);
        }
        else{
             qDebug()<<"写入头像失败";
              perror("保存失败");    //如果写入失败则查看是什么错误
                     fclose(fp);
                     return;
        }

    }
    else{
        qDebug()<<"写入头像失败";
         perror("保存失败");    //如果写入失败则查看是什么错误
        return;
    }
    ui->pb_userIcon->setText("");
    ui->pb_userIcon->setIcon(QIcon(FilePath));  //将图片显示在buton上

    //将图片信息通过信号发送给ckernel

        img.load(FilePath);
    //先将图片压缩到固定大小
    //第一、二参数代表缩放后的尺寸，第三个参数代表忽略比例，也可调用IgnoreAspectRatio保持长宽比例，第四个参数代表平滑处理，使图片缩小时不失真。
    QImage image=img.scaled(60,60,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //qDebug()<<"大小为"<<image.size();
    //将图片转换为QByteArray，再经过信号传递给kernel
     QByteArray byte;   //QImage转QByteArray,这个要借助QBuffer类：
     QBuffer buff(&byte);
     buff.open(QIODevice::WriteOnly);
     image.save(&buff,"JPEG");
     qDebug()<<byte.size()<<"wwwwwww";
      qDebug()<<byte.size();


     qDebug()<<FilePath;  //输出文件路径


    //将图片信息上传给服务器（发信号给ckernel）
    Q_EMIT SIG_alterIcon(byte);
     delete []imgInfo;
      imgInfo = NULL;

      return;
}
