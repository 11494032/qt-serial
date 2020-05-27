#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QThread>
class SerialThread : public QThread
{
    Q_OBJECT
public:
    explicit SerialThread(QObject *parent = 0);

protected:
    void run();

signals:
    void TestSignal(int);

private:
  //  Msg msg;
};

#endif // SERIALTHREAD_H
