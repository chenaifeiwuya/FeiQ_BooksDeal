#ifndef MYVOID_H
#define MYVOID_H
#include<QAudio>
#include<QAudioFormat>
#include<QAudioInput>
#include<QAudioOutput>
#include<QIODevice>
#include<QObject>

class Myvoid:public QObject
{
       Q_OBJECT
public:
    Myvoid(QObject *parent = 0);
    QAudioInput *input;
        QIODevice *inputDevice;

        QAudioInput *output;
            QIODevice *outputDevice;
            //播放接收到的音频文件：
            void readyReadSlot(char* data,int lens);

            long ip;
private slots:
        void onReadyRead();

signals:
    //发送需要发送的音频信息给kernel
       void SIG_sendVoid(char* data, int lens);

};

#endif // MYVOID_H
