#include "serialthread.h"


SerialThread::SerialThread(QObject *parent) :
    QThread(parent)
{
}
/*
void SerialThread::TestSignal(int a)
{
    std::printf("child printf main's int = %d\n",a);
}*/

void SerialThread::run()
{
    //触发信号
    emit TestSignal(123);

}

