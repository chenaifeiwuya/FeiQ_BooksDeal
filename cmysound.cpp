#include "cmysound.h"
#include<QByteArray>
#include<QDebug>

//静态成员变量类外初始化
CMySound* CMySound::MySignalSound;
CRITICAL_SECTION* CMySound::cs;


CMySound::CMySound():IsQuite(true)
{
    //初始化关键段
    cs=new CRITICAL_SECTION;
            InitializeCriticalSection(cs);
            format.setSampleRate(1600);//设置采样率值越大，一次采集的数据越多
                format.setChannelCount(1);   //设定声道数目，mono(平声道)的声道数目是1；stero(立体声)的声道数目是2
                format.setSampleSize(1);   //设置采样大小，值越大，一次采样数据越多
                format.setCodec("audio/pcm");   //编码器
                format.setSampleType(QAudioFormat::SignedInt);      //设置采样类型
                 format.setByteOrder(QAudioFormat::LittleEndian);    //设定高低位的，LittleEndian（低位优先）/LargeEndian(高位优先)
               info = QAudioDeviceInfo::defaultInputDevice();
               if(!info.isFormatSupported(format))                 //格式是否支持;
               {
                   format = info.nearestFormat(format);
               }
                 input =new QAudioInput(format,this);  //输入音频
                 output =new QAudioOutput(format, this);            //输出音频

}

CMySound::~CMySound()
{
    //删除关键段
//    DeleteCriticalSection(cs);
//    if(outputDevice)
//   delete outputDevice;
   delete output;
//    if(inputDevice)
//   delete inputDevice;
   delete input;

}

//获得该单例的接口
CMySound *CMySound::getMySignalSound()
{
    if(!MySignalSound)
    {
        //进入关键段
//        EnterCriticalSection(cs);
        if(!MySignalSound){
           MySignalSound = new CMySound;
        }
//离开关键段
//        LeaveCriticalSection(cs);
    }
    return MySignalSound;
}

//开始采集声音和播放声音
void CMySound::initMySound()
{
    inputDevice = input->start();//input开始读入输入的音频信号，写入QIODevice，这里是inputDevice
        outputDevice = output->start();//开始播放
        connect(inputDevice,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
            //槽函数，当inputDevice收到input写入的音频数据之后,调用onReadyRead函数，发送数据到目标主机上
        IsQuite = false;

}

//结束采集声音(销毁类时使用)
void CMySound::endMySound()
{
    delete input;
    delete inputDevice;
    delete output;
    delete outputDevice;
}



//音频通话暂停
void CMySound::endOutSound()
{
   input->stop();
   output->stop();
   IsQuite = true;
}

void CMySound::listenVoid(char *Sound, long len)
{
//    QByteArray data;
//    memcpy(data.data(),Sound,len);
    if(!IsQuite)
     outputDevice->write(Sound,len);              //播放音频数据(如果没有执行start函数就采集或者输出音频信号会报错)
}

void CMySound::onReadyRead()
{

    QByteArray byte;
        byte = inputDevice->readAll();
        qDebug()<<byte.size();

       // outputDevice->write(byte); //测试代码：测试能否播放
        if(byte.size() > 0)
        {
            char* soundInfo=new char[byte.size()];
            memcpy(soundInfo,byte.data(),byte.size());
            //将此音频信息发给kernel
            if(!IsQuite)  //如果未闭麦：
            Q_EMIT SIG_soundInfo(byte.size(),soundInfo);
        }
        else
            return ;

}
