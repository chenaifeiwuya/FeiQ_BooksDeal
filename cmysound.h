#ifndef CMYSOUND_H
#define CMYSOUND_H
#include<QObject>
#include<Windows.h>

#include<QAudio>
#include<QAudioFormat>
#include<QAudioInput>
#include<QAudioOutput>
#include<QIODevice>

class CMySound:public QObject
{
    Q_OBJECT
private:
    //所有构造函数私有
        CMySound();
        CMySound(CMySound const&);  //拷贝构造函数
        CMySound& operator=(CMySound const&);//等号重载私有
public:
        ~CMySound();
private:
               static CRITICAL_SECTION* cs;//关键段
public:
    static CMySound* MySignalSound;
    QAudioInput *input;
    QAudioDeviceInfo info;
        QAudioFormat format;
    QIODevice *inputDevice;
    QAudioOutput *output;
    QIODevice *outputDevice;

        QAudioFormat formatout;
    long m_id;
    bool IsQuite;

public:
   static CMySound* getMySignalSound();
    void initMySound(); //开始采集声音和播放声音

    void endMySound();  //delete声音采集和输出

//        void initOutSound();  //初始化别人的声音
        void endOutSound();  //暂停采集声音和输出
        void listenVoid(char *Sound, long len);  //播放音频

private slots:
    void onReadyRead();

signals:
       //将采集到的音频信息发送给kernel
    void SIG_soundInfo(int lens,char* sound);
};


#endif // CMYSOUND_H
