#include "myvoid.h"
#include<QDebug>
#include<packDef.h>

Myvoid::Myvoid(QObject *parent)
{


    QAudioFormat format;
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);

    input = new QAudioInput(format, this);
    inputDevice = input->start();   //input开始读入输入的音频信号，写入QIODevice，这里是inputDevice
    connect(inputDevice, SIGNAL(readyRead()),this,SLOT(onReadyRead()));

 //播放音频相关
    output = new QAudioOutput(format, this);

    outputDevice = output->start(); //开始播放

}

void Myvoid::onReadyRead()
{
    qDebug()<<__func__;
    char data[1024];
    //读取音频
    int lens=inputDevice->read(data,1024);
    Q_EMIT SIG_sendVoid(data,lens);
}

//播放接收到的音频文件
void Myvoid::readyReadSlot(char* data,int lens)
{
   outputDevice->write(vp.data,vp.lens);

}
